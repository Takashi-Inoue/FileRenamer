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

#include "usingpathentity.h"
#include "stringbuilder/onfile/ifileinfo.h"

namespace Path {

class PathEntity;

class PathEntityInfo : public StringBuilder::OnFile::IFileInfo
{
public:
    PathEntityInfo(SharedEntity entity);

    bool isDir() const override;
    QString fullPath() const override;
    QString fileName() const override;
    QString completeBaseName() const override;
    QString suffix() const override;
    QString hashHex(QCryptographicHash::Algorithm algorithm) const override;
    QString imageHash() const override;

    void setHashHex(QCryptographicHash::Algorithm algorithm, QString hashHex) override;
    void setImageHash(QString) override;

private:
    SharedEntity m_entity;
};

} // Path
