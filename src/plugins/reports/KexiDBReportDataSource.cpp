/*
* Kexi Report Plugin
* Copyright (C) 2007-2017 by Adam Pigg <adam@piggz.co.uk>
* Copyright (C) 2017-2018 Jarosław Staniek <staniek@kde.org>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Copyright (C) 2026 アケネＪ / Akenejie
 *
 * Modified by akenejie and released under the GNU Affero General Public License v3.0.
 * See the LICENSE.AGPLv3 file in the project root for more details.
 */

#include "KexiDBReportDataSource.h"
#include "kexireportpart.h"
#include <kexiutils/utils.h>
#include <kexiqueryparameters.h>
#include <core/akexiproject.h>

#include <KDbConnection>
#include <KDbOrderByColumn>
#include <KDbQuerySchema>
#include <KDbNativeStatementBuilder>

#include <QDomDocument>
#include <QDebug>

class Q_DECL_HIDDEN KexiDBReportDataSource::Private
{
public:
    explicit Private(KexiReportPartTempData *data)
      : cursor(0), tempData(data), originalSchema(0), copySchema(0)
    {
    }
    ~Private()
    {
        delete copySchema;
        delete originalSchema;
    }


    QString objectName;

    KDbCursor *cursor;
    KexiReportPartTempData *tempData;
    KDbQuerySchema *originalSchema;
    KDbQuerySchema *copySchema;
    KDbEscapedString schemaSql;
    QList<QVariant> currentParams;
};

KexiDBReportDataSource::KexiDBReportDataSource(const QString &objectName, const QString &pluginId,
                                               KexiReportPartTempData *data)
    : d(new Private(data))
{
    d->objectName = objectName;
    getSchema(pluginId);
}

void KexiDBReportDataSource::setSorting(const QList<SortedField>& sorting)
{
    if (d->copySchema) {
        if (sorting.isEmpty())
            return;
        KDbOrderByColumnList order;
        for (int i = 0; i < sorting.count(); i++) {
            if (!order.appendField(d->tempData->dataConnection(), d->copySchema, sorting[i].field(),
                                   KDbOrderByColumn::fromQt(sorting[i].order())))
            {
                qWarning() << "Cannot set sort field" << i << sorting[i].field();
                return;
            }
        }
        d->copySchema->setOrderByColumnList(order);
    } else {
        qWarning() << "Unable to sort null schema";
    }
}

void KexiDBReportDataSource::addCondition(const QString &field, const QVariant &value, const QString& relation)
{
    if (d->copySchema) {
        KDbField *fld = d->copySchema->findTableField(field);
        if (fld) {
            if (relation.length() == 1) {
                QString errorMessage;
                QString errorDescription;
                if (!d->copySchema->addToWhereExpression(fld, value, KDbToken(relation.toLatin1()[0]),
                                                         &errorMessage, &errorDescription))
                {
                    qWarning() << "Invalid expression cannot be added to WHERE:" << fld
                               << relation << value;
                    qWarning() << "addToWhereExpression() failed, message=" << errorMessage
                               << "description=" << errorDescription;
                }
            } else {
                qWarning() << "Invalid relation passed in:" << relation;
            }
        }
    } else {
        qDebug() << "Unable to add expresstion to null schema";
    }
}

KexiDBReportDataSource::~KexiDBReportDataSource()
{
    close();
    delete d;
}

bool KexiDBReportDataSource::open()
{
    KDbConnection *dataConn = d->tempData->dataConnection();
    if ( dataConn && d->cursor == 0 )
    {
        if ( d->objectName.isEmpty() )
        {
            return false;
        }
        else if ( d->copySchema)
        {
            //qDebug() << "Opening cursor.."
            //         << KDbConnectionAndQuerySchema(dataConn, *d->copySchema);
            bool ok;
            KexiUtils::WaitCursorRemover remover;
            d->currentParams = KexiQueryParameters::getParameters(0, dataConn, d->originalSchema, &ok);
            if (!ok) {
                return false;
            }

            d->cursor = dataConn->executeQuery(d->copySchema, d->currentParams, KDbCursor::Option::Buffered);
        }


        if ( d->cursor )
        {
            qDebug() << "Moving to first record..";
            return d->cursor->moveFirst();
        }
        else
            return false;
    }
    return false;
}

bool KexiDBReportDataSource::close()
{
    if (d->cursor) {
        KDbConnection *dataConn = d->tempData->dataConnection();
        const bool ok = d->cursor->close();
        dataConn->deleteCursor(d->cursor);
        d->cursor = nullptr;
        return ok;
    }
    return true;
}

bool KexiDBReportDataSource::getSchema(const QString& pluginId)
{
    KexiProject *prj = d->tempData->project();
    KDbConnection *metaConn = d->tempData->connection();
    KDbConnection *dataConn = d->tempData->dataConnection();
    if (metaConn && dataConn) {
        KDbTableSchemaChangeListener::unregisterForChanges(dataConn, d->tempData);
        delete d->originalSchema;
        d->originalSchema = 0;
        delete d->copySchema;
        d->copySchema = 0;

        KDbTableSchema *table = nullptr;
        KDbQuerySchema *query = nullptr;
        if ((pluginId.isEmpty() || pluginId == "akenejie.akexi-project.table")
                && (table = prj->tableSchema(d->objectName)))
        {
            qDebug() << d->objectName <<  "is a table..";
            d->originalSchema = new KDbQuerySchema(table);
        }
        else if ((pluginId.isEmpty() || pluginId == "akenejie.akexi-project.query")
                 && (query = metaConn->querySchema(d->objectName)))
        {
            qDebug() << d->objectName <<  "is a query..";
            qDebug() << KDbConnectionAndQuerySchema(metaConn, *query);
            d->originalSchema = new KDbQuerySchema(*query, metaConn);
        }

        if (d->originalSchema) {
            const KDbNativeStatementBuilder builder(dataConn, KDb::DriverEscaping);
            KDbEscapedString sql;
            if (builder.generateSelectStatement(&sql, d->originalSchema, d->currentParams)) {
                qDebug() << "Original:" << sql;
            } else {
                qDebug() << "Original: ERROR";
                return false;
            }
            qDebug() << KDbConnectionAndQuerySchema(dataConn, *d->originalSchema);

            d->copySchema = new KDbQuerySchema(*d->originalSchema, dataConn);
            qDebug() << KDbConnectionAndQuerySchema(dataConn, *d->copySchema);
            if (builder.generateSelectStatement(&d->schemaSql, d->copySchema, d->currentParams)) {
                qDebug() << "Copy:" << d->schemaSql;
            } else {
                qDebug() << "Copy: ERROR";
                return false;
            }
            if (table) {
                KDbTableSchemaChangeListener::registerForChanges(dataConn, d->tempData, table);
            } else if (query) {
                KDbTableSchemaChangeListener::registerForChanges(dataConn, d->tempData, query);
            }
        }
        return true;
    }
    return false;
}

QString KexiDBReportDataSource::sourceName() const
{
    return d->objectName;
}

int KexiDBReportDataSource::fieldNumber ( const QString &fld ) const
{
    if (!d->cursor || !d->cursor->query()) {
        return -1;
    }
    const KDbQueryColumnInfo::Vector fieldsExpanded(d->cursor->query()->fieldsExpanded(
        d->tempData->dataConnection(), KDbQuerySchema::FieldsExpandedMode::Unique));
    for (int i = 0; i < fieldsExpanded.size(); ++i) {
        if (0 == QString::compare(fld, fieldsExpanded[i]->aliasOrName(), Qt::CaseInsensitive)) {
            return i;
        }
    }
    return -1;
}

QStringList KexiDBReportDataSource::fieldNames() const
{
    if (!d->originalSchema) {
        return QStringList();
    }
    QStringList names;
    const KDbQueryColumnInfo::Vector fieldsExpanded(d->originalSchema->fieldsExpanded(
        d->tempData->dataConnection(), KDbQuerySchema::FieldsExpandedMode::Unique));
    for (int i = 0; i < fieldsExpanded.size(); i++) {
        //! @todo in some Kexi mode captionOrAliasOrName() would be used here (more user-friendly)
        names.append(fieldsExpanded[i]->aliasOrName());
    }
    return names;
}

QVariant KexiDBReportDataSource::value (int i) const
{
    if ( d->cursor )
        return d->cursor->value ( i );

    return QVariant();
}

QVariant KexiDBReportDataSource::value ( const QString &fld ) const
{
    int i = fieldNumber ( fld );

    if (d->cursor && i >= 0)
        return d->cursor->value ( i );

    return QVariant();
}

bool KexiDBReportDataSource::moveNext()
{
    if ( d->cursor )
        return d->cursor->moveNext();

    return false;
}

bool KexiDBReportDataSource::movePrevious()
{
    if ( d->cursor ) return d->cursor->movePrev();

    return false;
}

bool KexiDBReportDataSource::moveFirst()
{
    if ( d->cursor ) return d->cursor->moveFirst();

    return false;
}

bool KexiDBReportDataSource::moveLast()
{
    if ( d->cursor )
        return d->cursor->moveLast();

    return false;
}

qint64 KexiDBReportDataSource::at() const
{
    if ( d->cursor )
        return d->cursor->at();

    return 0;
}

qint64 KexiDBReportDataSource::recordCount() const
{
    if (d->copySchema) {
        return d->tempData->dataConnection()->recordCount(d->copySchema);
    }

    return 1;
}

double KexiDBReportDataSource::runAggregateFunction(const QString &function, const QString &field,
                                                    const QMap<QString, QVariant> &conditions)
{
    double numberResult = 0.0;
    if (d->schemaSql.isEmpty()) {
        qWarning() << "No query for running aggregate function" << function << field;
        return numberResult;
    }
    KDbEscapedString whereSql;
    KDbConnection *conn = d->tempData->dataConnection();
    if (!conditions.isEmpty()) {
        for (QMap<QString, QVariant>::ConstIterator it = conditions.constBegin();
             it != conditions.constEnd(); ++it)
        {
            if (!whereSql.isEmpty()) {
                whereSql.append(" AND ");
            }
            KDbQueryColumnInfo *cinfo = d->copySchema->columnInfo(conn, it.key());
            if (!cinfo) {
                qWarning() << "Could not find column" << it.key() << "for condition" << it.key()
                           << "=" << it.value();
                return numberResult;
            }
            whereSql.append(
                KDbEscapedString(conn->escapeIdentifier(cinfo->aliasOrName()))
                + " = "
                + conn->driver()->valueToSql(cinfo->field(), it.value()));
        }
        whereSql.prepend(" WHERE ");
    }

    const KDbEscapedString sql = KDbEscapedString("SELECT " + function + "(" + field + ") FROM ("
                                                   + d->schemaSql + ")" + whereSql);
    QString stringResult;
    const tristate res = conn->querySingleString(sql, &stringResult);
    if (res != true) {
        qWarning() << "Failed to execute query for running aggregate function" << function << field;
        return numberResult;
    }
    bool ok;
    numberResult = stringResult.toDouble(&ok);
    if (!ok) {
        qWarning() << "Result of query for running aggregate function" << function << field
                   << "is not a number (" << stringResult << ")";
        return numberResult;
    }
    return numberResult;
}

QStringList KexiDBReportDataSource::dataSourceNames() const
{
    //Get the list of tables and queries available in the project
    KDbConnection *dataConn = d->tempData->dataConnection();
    KDbConnection *metaConn = d->tempData->connection();
    QStringList qs;
    if (dataConn && dataConn->isConnected()) {
        // List tables from the data connection (.sqlite3 in akexi mode)
        QList<int> tids = dataConn->tableIds();
        qs << "";
        for (int i = 0; i < tids.size(); ++i) {
            KDbTableSchema* tsc = dataConn->tableSchema(tids[i]);
            if (tsc)
                qs << tsc->name();
        }
    }
    if (metaConn && metaConn->isConnected()) {
        // List queries from the metadata connection (.akexi in akexi mode)
        QList<int> qids = metaConn->queryIds();
        qs << "";
        for (int i = 0; i < qids.size(); ++i) {
            KDbQuerySchema* qsc = metaConn->querySchema(qids[i]);
            if (qsc)
                qs << qsc->name();
        }
    }

    return qs;
}

KReportDataSource* KexiDBReportDataSource::create(const QString& source) const
{
    return new KexiDBReportDataSource(source, QString(), d->tempData);
}
