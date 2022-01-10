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

#include "imagehash.h"
#include "ifileinfo.h"
#include "imagehash/imagehashcalculator.h"

namespace StringBuilder {
namespace OnFile {

void ImageHash::build(QString &result)
{
    emit needFileInfo(this);

    QString imageHashString = m_fileInfo->imageHash();

    if (imageHashString.isEmpty()) {
        ImageHashCalculator imageHash(m_fileInfo->fullPath());

        imageHashString = imageHash.resultString();

        m_fileInfo->setImageHash(imageHashString);
    }

    result.insert(posToInsert(result.size()), imageHashString);
}

QString ImageHash::toString() const
{
    if (isLeftMost())
        return QStringLiteral("<< Image Hash");

    if (isRightMost())
        return QStringLiteral("Image Hash >>");

    return QStringLiteral("__%1 Image Hash").arg(insertPosition());
}

} // OnFile
} // StringBuilder
