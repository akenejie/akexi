/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/*
 * Copyright (C) 2026 アケネＪ / Akenejie
 *
 * Modified by akenejie and released under the GNU Affero General Public License v3.0.
 * See the LICENSE.AGPLv3 file in the project root for more details.
 */

#include "keximigrationpart.h"

#include <migration/importwizard.h>
#include <migration/importtablewizard.h>
#include <migration/migratemanager.h>
#include <core/KexiMainWindowIface.h>
#include <core/akexiproject.h>
#include <core/kexipart.h>

KEXI_PLUGIN_FACTORY(KexiMigrationPart, "akexi_migrationplugin.json")

KexiMigrationPart::KexiMigrationPart(QObject *parent, const QVariantList &args)
        : KexiInternalPart(parent, args)
{
}

KexiMigrationPart::~KexiMigrationPart()
{
}

QWidget *KexiMigrationPart::createWidget(const char* widgetClass,
        QWidget *parent, const char *objName, QMap<QString, QString>* args)
{
    QWidget *w;

    if (QString(widgetClass) == "migration") {
        w = new KexiMigration::ImportWizard(parent, args);
    }
    else if (QString(widgetClass) == "importtable") {
        w = new KexiMigration::ImportTableWizard(KexiMainWindowIface::global()->project()->dbConnection(), parent, args);
    }
    else
        return 0;

    w->setObjectName(objName);
    return w;
}

QObject *KexiMigrationPart::createObject(const char* objectClass,
                                         QObject * parent, const char * objName,
                                         QMap<QString, QString>* args)
{
    Q_UNUSED(args)
    if (0 == qstrcmp(objectClass, "manager")) {
        KexiMigration::MigrateManager *manager = new KexiMigration::MigrateManager;
        manager->setParent(parent);
        manager->setObjectName(objName);
        return manager;
    }
    return 0;
}

#include "keximigrationpart.moc"
