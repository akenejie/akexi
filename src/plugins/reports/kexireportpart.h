/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2011-2017 Jarosław Staniek <staniek@kde.org>
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

#ifndef _KEXIREPORTPART_H_
#define _KEXIREPORTPART_H_

#include <core/kexipart.h>
#include <core/KexiWindowData.h>

#include <KReportDataSource>
#include <KReportScriptSource>

#include <KDbTableSchemaChangeListener>

#include <QDomElement>

#include <core/akexiproject.h>

class KexiReportPartTempData : public KexiWindowData, public KDbTableSchemaChangeListener
{
    Q_OBJECT
public:
    KexiReportPartTempData(KexiWindow* parent, KexiProject *project);
    ~KexiReportPartTempData();
    QDomElement reportDefinition;
    QDomElement connectionDefinition;

    /*! true, if \a document member has changed in previous view. Used on view switching.
    Check this flag to see if we should refresh data for DataViewMode. */
    bool reportSchemaChangedInPreviousView;

    KDbConnection *connection();
    KDbConnection *dataConnection();
    KexiProject *project() const;

protected:
    //! This temp-data acts as a listener for tracking changes in table schema
    //! used by the report. This method closes the report on request.
    tristate closeListener() override;

private:
    Q_DISABLE_COPY(KexiReportPartTempData)
    class Private;
    Private * const d;
};

/**
 * @short Application Main Window
 */
class KexiReportPart : public KexiPart::Part, public KReportScriptSource
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    KexiReportPart(QObject *parent, const QVariantList &l);

    /**
     * Default Destructor
     */
    virtual ~KexiReportPart();

    virtual void setupCustomPropertyPanelTabs(QTabWidget *tab);

    virtual KLocalizedString i18nMessage(const QString& englishMessage,
                                         KexiWindow* window) const;

    QStringList scriptList() const override;
    QString scriptCode(const QString& script) const override;

protected:
    KexiView *createView(QWidget *parent, KexiWindow *win, KexiPart::Item *item,
                         Kexi::ViewMode = Kexi::DataViewMode,
                         QMap<QString, QVariant> *staticObjectArgs = nullptr) override;

    KexiWindowData* createWindowData(KexiWindow* window) override;

    virtual void initPartActions();

    virtual KDbObject* loadSchemaObject(KexiWindow *window,
            const KDbObject& object, Kexi::ViewMode viewMode, bool *ownedByWindow);

private Q_SLOTS:
    void slotToolboxActionTriggered(bool checked);
    //! Unchecks toolbox action for @a entity after it is used.
    void slotItemInserted(const QString& entity);

private:
    class Private;
    Private* d;
};

#endif // _KEXIREPORTPART_H_
