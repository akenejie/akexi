/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2005 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/*
 * Copyright (C) 2026 アケネＪ / Akenejie
 *
 * Modified by akenejie and released under the GNU Affero General Public License v3.0.
 * See the LICENSE.AGPLv3 file in the project root for more details.
 */

#include "kexiscripteditor.h"

/// \internal d-pointer class
class Q_DECL_HIDDEN KexiScriptEditor::Private
{
public:
    QString scriptaction;
    Private() {}
};

KexiScriptEditor::KexiScriptEditor(QWidget *parent)
        : KexiEditor(parent)
        , d(new Private())
{
}

KexiScriptEditor::~KexiScriptEditor()
{
    delete d;
}

bool KexiScriptEditor::isInitialized() const
{
    return !d->scriptaction.isEmpty();
}

void KexiScriptEditor::initialize(const QString &scriptaction)
{
    d->scriptaction = scriptaction;

    disconnect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));

    if (d->scriptaction.isEmpty()) {
    }
    KexiEditor::setText(d->scriptaction);
    // We assume Kross and the HighlightingInterface are using same
    // names for the support languages...
    setHighlightMode("javascript");

    clearUndoRedo();
    KexiEditor::setDirty(false);
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

void KexiScriptEditor::slotTextChanged()
{
    KexiEditor::setDirty(true);
    d->scriptaction = KexiEditor::text();
}

void KexiScriptEditor::setLineNo(long lineno)
{
    setCursorPosition(lineno, 0);
}


