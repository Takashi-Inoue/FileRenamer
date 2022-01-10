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

namespace StringBuilder {
namespace OnFile {

void OriginalName::build(QString &result)
{
    emit needFileInfo(this);

    QString name = m_fileInfo->isDir() ? m_fileInfo->fileName()
                                       : m_fileInfo->completeBaseName();

    result.insert(posToInsert(result.size()), name);
}

QString OriginalName::toString() const
{
    if (isLeftMost())
        return QStringLiteral("&lt;&lt; Original Name");

    if (isRightMost())
        return QStringLiteral("Original Name &gt;&gt;");

    return QStringLiteral("__%1 Original Name").arg(insertPosition());

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

} // OnFile
} // StringBuilder
