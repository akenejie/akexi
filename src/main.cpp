/* This file is part of the KDE project

   begin : Sun Jun  9 12:15:11 CEST 2002

   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2017 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
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

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <main/AkexiMainWindow.h>

int main(int argc, char *argv[])
{
    // If QT_QPA_PLATFORM_PLUGIN_PATH is not set, look for a plugins/ directory
    // next to the executable (common layout on Windows)
    const QFileInfo exeInfo(QFile::decodeName(argv[0]));
    const QString exeDir = exeInfo.absolutePath();
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM_PLUGIN_PATH")) {
        const QString pluginDir = exeDir + QStringLiteral("/plugins");
        if (QDir(pluginDir).exists()) {
            qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", QDir::toNativeSeparators(pluginDir).toLocal8Bit());
        }
    }
    // Add exe directory to library paths so KDb, KReport etc. can find
    // their driver/plugin DLLs in <exeDir>/kdb3/, <exeDir>/kreport3/ etc.
    QCoreApplication::addLibraryPath(exeDir);

    // Initialize before constructing QGuiApplication to avoid issue
    // "Qt WebEngine seems to be initialized from a plugin. Please set
    // Qt::AA_ShareOpenGLContexts using QCoreApplication::setAttribute before constructing
    // QGuiApplication."
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    //! @todo use non-GUI app when needed
    QApplication app(argc, argv);
    const int result = KexiMainWindow::create(QCoreApplication::arguments());
    if (result != 0) {
        return result;
    }
    return app.exec();
}
