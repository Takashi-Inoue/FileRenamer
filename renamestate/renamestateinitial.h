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

#pragma once

#include "renamestateistate.h"

namespace RenameState {

class Initial : public RenameState::IState
{
public:
    using IState::IState;

    QIcon stateIcon() const override;
    QString stateText() const override;

    bool isAfterRenaming() const override;
    bool readyToRename() const override;
    bool isRenameSucceeded() const override;
    bool isRenameFailed() const override;
};

} // RenameState
