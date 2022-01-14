/*
 * Copyright 2021 Takashi Inoue
 *
 * This file is part of FileRenamer.
 *
 * FileRenamer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FileRenamer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FileRenamer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "renamestateinitial.h"

namespace RenameState {

QIcon Initial::stateIcon() const
{
    static QIcon icon(QStringLiteral(":/res/images/circlegray.svg"));

    return icon;
}

QString Initial::stateText() const
{
    return QObject::tr("Waiting");
}

bool Initial::isAfterRenaming() const
{
    return false;
}

bool Initial::readyToRename() const
{
    return false;
}

bool Initial::isRenameSucceeded() const
{
    return false;
}

bool Initial::isRenameFailed() const
{
    return false;
}

} // RenameState
