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
#include "utilityshtml.h"

#include <QSettings>

namespace StringBuilder {
namespace OnFile {

namespace Settings {
constexpr char groupName[] = "ImageHash";
} // Settings

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
        return Html::leftAligned(QStringLiteral("&lt;&lt; <b>Image Hash</b>"));

    if (isRightMost())
        return Html::rightAligned(QStringLiteral("<b>Image Hash</b> &gt;&gt;"));

    return Html::leftAligned(QStringLiteral("__%1 <b>Image Hash</b>").arg(insertPosition()));
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

void ImageHash::loadSettings(QSettings *qSet)
{
    qSet->beginGroup(Settings::groupName);

    AbstractInsertString::loadSettings(qSet);

    qSet->endGroup();
}

void ImageHash::saveSettings(QSettings *qSet) const
{
    qSet->beginGroup(Settings::groupName);

    AbstractInsertString::saveSettings(qSet);

    qSet->endGroup();
}

} // OnFile
} // StringBuilder
