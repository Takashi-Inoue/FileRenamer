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

#include "insertstring.h"
#include "widgets/widgetinserttextsetting.h"

namespace StringBuilder {

InsertString::InsertString()
    : InsertString(0, QString{}, nullptr)
{
}

InsertString::InsertString(int pos, QStringView string, QObject *parent)
    : AbstractInsertString{pos, parent},
      m_string{string.toString()}
{
}

void InsertString::build(QString &result)
{
    result.insert(actualInsertPosition(result.size()), m_string);
}

QString InsertString::toHtmlString() const
{
    const QString text = m_string.isEmpty() ? QStringLiteral("<b>Insert Text</b>")
                                            : QStringLiteral("<i>%1</i>").arg(m_string);

    if (isLeftMost())
        return QStringLiteral("<< %1").arg(text);

    if (isRightMost())
        return QStringLiteral("%1 >>").arg(text);

    return QStringLiteral("__%1 %2").arg(insertPosition()).arg(text);
}

AbstractWidget *InsertString::settingsWidget()
{
    auto widget = new WidgetInsertTextSetting(m_string, insertPosition());

    connect(widget, &AbstractWidget::accepted, this, [&, this]() {
        auto settingsWidget = qobject_cast<WidgetInsertTextSetting *>(sender());

        m_string = settingsWidget->insertText();
        setInsertPosition(settingsWidget->insertPosition());
    });

    return widget;
}

} // StringBuilder

