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
#include "stringbuilder/widgets/widgetimagehashsetting.h"

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

    result.insert(actualInsertPosition(result.size()), imageHashString);
}

QString ImageHash::toHtmlString() const
{
    if (isLeftMost())
        return QStringLiteral("<p align=\"left\">&lt;&lt; <b>Image Hash</b></p>");

    if (isRightMost())
        return QStringLiteral("<p align=\"right\"><b>Image Hash</b> &gt;&gt;</p>");

    return QStringLiteral("<p align=\"left\">__%1 <b>Image Hash</b></p>").arg(insertPosition());
}

AbstractWidget *ImageHash::settingsWidget()
{
    auto widget = new WidgetImageHashSetting(insertPosition());

    connect(widget, &AbstractWidget::accepted, this, [&, this]() {
        auto settingsWidget = qobject_cast<WidgetImageHashSetting *>(sender());

        setInsertPosition(settingsWidget->insertPosition());
    });

    return widget;
}

} // OnFile
} // StringBuilder
