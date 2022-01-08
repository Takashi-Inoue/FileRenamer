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

#include "pathentityinfo.h"
#include "pathentity.h"

namespace Path {

PathEntityInfo::PathEntityInfo(SharedEntity entity)
    : m_entity(entity)
{
    Q_ASSERT(entity != nullptr);
}

bool PathEntityInfo::isDir() const
{
    return m_entity->isDir();
}

QString PathEntityInfo::fullPath() const
{
    return m_entity->fullPath();
}

QString PathEntityInfo::fileName() const
{
    return m_entity->name();
}

QString PathEntityInfo::completeBaseName() const
{
    QString name = m_entity->name();

    return name.left(name.lastIndexOf('.'));
}

QString PathEntityInfo::suffix() const
{
    QString name = m_entity->name();

    return name.mid(name.lastIndexOf('.') + 1);
}

QString PathEntityInfo::hashHex(QCryptographicHash::Algorithm algorithm) const
{
    return m_entity->hashHex(algorithm);
}

QString PathEntityInfo::imageHash() const
{
    return m_entity->imageHash();
}

void PathEntityInfo::setHashHex(QCryptographicHash::Algorithm algorithm, QString hashHex)
{
    m_entity->setHashHex(algorithm, hashHex);
}

void PathEntityInfo::setImageHash(QString imageHash)
{
    m_entity->setImageHash(imageHash);
}

} // Path
