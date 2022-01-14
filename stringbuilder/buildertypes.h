/*
 * Copyright YEAR Takashi Inoue
 *
 * This file is part of APPNAME.
 *
 * APPNAME is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * APPNAME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with APPNAME.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QHash>
#include <QObject>
#include <QString>

namespace StringBuilder {

enum class BuilderType : int {
    OriginalName, InsertText, ReplaceText, Number, FileHash, ImageHash
};

constexpr int builderTypeCount()
{
    return 6;
}

inline QString builderName(BuilderType builderType)
{
    static const QHash<BuilderType, QString> names{
        {BuilderType::OriginalName, QObject::tr("Original Name")},
        {BuilderType::InsertText, QObject::tr("Insert Text")},
        {BuilderType::ReplaceText, QObject::tr("Replace Text")},
        {BuilderType::Number, QObject::tr("Number")},
        {BuilderType::FileHash, QObject::tr("File Hash")},
        {BuilderType::ImageHash, QObject::tr("Image Hash")},
    };

    return names[builderType];
}

} // StringBuilder
