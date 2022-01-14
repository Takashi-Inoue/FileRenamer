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

#include "originalname.h"
#include "ifileinfo.h"
#include "stringbuilder/widgets/widgetoriginalnamesetting.h"
#include "utilityshtml.h"

#include <QSettings>

namespace StringBuilder {
namespace OnFile {

namespace Settings {
constexpr char groupName[] = "OriginalName";
} // Settings

void OriginalName::build(QString &result)
{
    emit needFileInfo(this);

    QString name = m_fileInfo->isDir() ? m_fileInfo->fileName()
                                       : m_fileInfo->completeBaseName();

    result.insert(actualInsertPosition(result.size()), name);
}

QString OriginalName::toHtmlString() const
{
    if (isLeftMost())
        return Html::leftAligned(tr("&lt;&lt; <b>Original Name</b>"));

    if (isRightMost())
        return Html::rightAligned(tr("<b>Original Name</b> &gt;&gt;"));

    return Html::leftAligned(tr("__%1__ <b>Original Name</b>").arg(insertPosition()));
}

AbstractWidget *OriginalName::settingsWidget()
{
    auto widget = new WidgetOriginalNameSetting(insertPosition());

    connect(widget, &AbstractWidget::accepted, this, [&, this]() {
        auto settingsWidget = qobject_cast<WidgetOriginalNameSetting *>(sender());

        setInsertPosition(settingsWidget->insertPosition());
    });

    return widget;
}

void OriginalName::loadSettings(QSettings *qSet)
{
    qSet->beginGroup(Settings::groupName);

    AbstractInsertString::loadSettings(qSet);

    qSet->endGroup();
}

void OriginalName::saveSettings(QSettings *qSet) const
{
    qSet->beginGroup(Settings::groupName);

    AbstractInsertString::saveSettings(qSet);

    qSet->endGroup();
}

} // OnFile
} // StringBuilder
