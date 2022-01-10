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

#include "widgetoriginalnamesetting.h"

#include "stringbuilder/onfile/originalname.h"

#include <QSettings>

namespace StringBuilder {

namespace {
constexpr char settingsGroupName[] = "OriginalName";
}

WidgetOriginalNameSetting::WidgetOriginalNameSetting(QWidget *parent)
    : WidgetOnlyPositionFixer{parent}
{
    setTitle(QStringLiteral("Original name"));
}

QSharedPointer<AbstractWidget> WidgetOriginalNameSetting::clone() const
{
    auto widget = QSharedPointer<WidgetOriginalNameSetting>::create();

    widget->setPositionToInsert(positionToInsert());

    return widget;
}

QString WidgetOriginalNameSetting::builderName() const
{
    return QStringLiteral("Original Name");
}

QString WidgetOriginalNameSetting::toString() const
{
    if (isLeftMost())
        return QStringLiteral("<<Original Name");

    if (isRightMost())
        return QStringLiteral("Original Name>>");

    return QStringLiteral("__%1 Original Name").arg(positionToInsert());
}

QFont WidgetOriginalNameSetting::fontForDisplay() const
{
    return QFont{};
}

Qt::Alignment WidgetOriginalNameSetting::alignForDisplay() const
{
    if (isRightMost())
        return Qt::AlignRight | Qt::AlignVCenter;

    return Qt::AlignLeft | Qt::AlignVCenter;
}

QSharedPointer<AbstractStringBuilder> WidgetOriginalNameSetting::stringBuilder() const
{
    return QSharedPointer<OnFile::OriginalName>::create(positionToInsert());
}

void WidgetOriginalNameSetting::loadSettings(QSharedPointer<QSettings> qSettings)
{
    qSettings->beginGroup(settingsGroupName);
    WidgetOnlyPositionFixer::loadSettings(qSettings);
    qSettings->endGroup();
}

void WidgetOriginalNameSetting::saveSettings(QSharedPointer<QSettings> qSettings) const
{
    qSettings->beginGroup(settingsGroupName);
    WidgetOnlyPositionFixer::saveSettings(qSettings);
    qSettings->endGroup();
}

} // StringBuilder
