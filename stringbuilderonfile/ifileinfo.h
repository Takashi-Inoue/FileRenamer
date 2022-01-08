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

#include <QCryptographicHash>
#include <QString>

namespace StringBuilderOnFile {

class IFileInfo
{
public:
    IFileInfo() = default;
    virtual ~IFileInfo() = default;

    virtual bool isDir() const = 0;
    virtual QString fullPath() const = 0;
    virtual QString fileName() const = 0;
    virtual QString completeBaseName() const = 0;
    virtual QString suffix() const = 0;
    virtual QString hashHex(QCryptographicHash::Algorithm algorithm) const = 0;
    virtual QString imageHash() const = 0;

    virtual void setHashHex(QCryptographicHash::Algorithm algorithm, QString hashHex) = 0;
    virtual void setImageHash(QString) = 0;
};

} // StringBuilderOnFile
