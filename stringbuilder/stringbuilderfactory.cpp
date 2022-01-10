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

#include "stringbuilderfactory.h"

#include "insertstring.h"
#include "number.h"
#include "replacestring.h"
#include "onfile/cryptographichash.h"
#include "onfile/imagehash.h"
#include "onfile/originalname.h"

#include <QHash>

namespace StringBuilder {

int BuilderFactory::builderTypeCount()
{
    return 6;
}

QString BuilderFactory::builderTitle(BuilderType builderType)
{
    static const QHash<BuilderType, QString> titles{
        {BuilderType::OriginalName, QStringLiteral("Original Name")},
        {BuilderType::InsertText, QStringLiteral("Insert Text")},
        {BuilderType::ReplaceText, QStringLiteral("Replace Text")},
        {BuilderType::Number, QStringLiteral("Number")},
        {BuilderType::FileHash, QStringLiteral("File Hash")},
        {BuilderType::ImageHash, QStringLiteral("Image Hash")},
    };

    return titles[builderType];
}

SharedStringBuilder BuilderFactory::createBuilder(BuilderType builderType)
{
    if (builderType == BuilderType::OriginalName)
        return QSharedPointer<OnFile::OriginalName>::create();

    if (builderType == BuilderType::InsertText)
        return QSharedPointer<InsertString>::create();

    if (builderType == BuilderType::ReplaceText)
        return QSharedPointer<ReplaceString>::create();

    if (builderType == BuilderType::Number)
        return QSharedPointer<Number>::create();

    if (builderType == BuilderType::FileHash)
        return QSharedPointer<OnFile::CryptographicHash>::create();

    if (builderType == BuilderType::ImageHash)
        return QSharedPointer<OnFile::ImageHash>::create();

    Q_ASSERT(false);

    return nullptr;
}

} // namespace StringBuilder
