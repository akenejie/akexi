/*
* Kexi Report Plugin
* Copyright (C) 2007-2016 by Adam Pigg <adam@piggz.co.uk>
* Copyright (C) 2017 Jarosław Staniek <staniek@kde.org>
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

#ifndef KEXISOURCESELECTOR_H
#define KEXISOURCESELECTOR_H

#include <QWidget>

#include <KReportDataSource>

#include "KexiDBReportDataSource.h"
#ifdef HAVE_KEXI_MIGRATE
#include "keximigratereportdata.h"
#endif

class QDomElement;
class KexiProject;

//! @todo rename to KexiReportDataSourcePage
//! @todo use KexiPropertyPaneViewBase
class KexiSourceSelector : public QWidget
{
    Q_OBJECT
public:
    explicit KexiSourceSelector(KexiProject* project, QWidget* parent = 0);
    ~KexiSourceSelector();

    KReportDataSource* createDataSource() const;

    QDomElement connectionData();

    //! @return name plugin ID of selected item (a table or a query). Can return an empty string.
    QString selectedPluginId() const;

    //! @return name of selected table or query.
    QString selectedName() const;

    //! \return true if the current selection is valid
    bool isSelectionValid() const;

public Q_SLOTS:
    /*! Sets item for data source described by \a pluginId and \a name.
     If \a pluginId is empty, either "akenejie.akexi-project.table" and "akenejie.akexi-project.query" are tried. */
    void setDataSource(const QString& pluginId, const QString& name);

Q_SIGNALS:
    void dataSourceChanged();

private:
    class Private;
    Private * const d;
};

#endif // KEXISOURCESELECTOR_H
