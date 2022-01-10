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

#include "widgetimagehashsetting.h"

#include "stringbuilder/onfile/imagehash.h"

#include <QSettings>

namespace StringBuilder {

namespace {
constexpr char settingsGroupName[] = "ImageHash";
}

WidgetImageHashSetting::WidgetImageHashSetting(QWidget *parent)
    : WidgetOnlyPositionFixer(parent)
{
    setTitle(QStringLiteral("Image Hash"));
}

QSharedPointer<AbstractWidget> WidgetImageHashSetting::clone() const
{
    auto widget = QSharedPointer<WidgetImageHashSetting>::create();

    widget->setPositionToInsert(positionToInsert());

    return widget;
}

QString WidgetImageHashSetting::builderName() const
{
    return QStringLiteral("Image Hash");
}

QString WidgetImageHashSetting::toString() const
{
    return QStringLiteral("Image Hash > pos:%1").arg(positionToInsert());
}

QFont WidgetImageHashSetting::fontForDisplay() const
{
    return QFont{};
}

Qt::Alignment WidgetImageHashSetting::alignForDisplay() const
{
    if (isRightMost())
        return Qt::AlignRight | Qt::AlignVCenter;

    return Qt::AlignLeft | Qt::AlignVCenter;
}

QSharedPointer<AbstractStringBuilder> WidgetImageHashSetting::stringBuilder() const
{
    return QSharedPointer<OnFile::ImageHash>::create(positionToInsert());
}

void WidgetImageHashSetting::loadSettings(QSharedPointer<QSettings> qSettings)
{
    qSettings->beginGroup(settingsGroupName);
    WidgetOnlyPositionFixer::loadSettings(qSettings);
    qSettings->endGroup();
}

void WidgetImageHashSetting::saveSettings(QSharedPointer<QSettings> qSettings) const
{
    qSettings->beginGroup(settingsGroupName);
    WidgetOnlyPositionFixer::saveSettings(qSettings);
    qSettings->endGroup();
}

} // StringBuilder
