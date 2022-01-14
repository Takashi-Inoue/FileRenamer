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
#include "utilityshtml.h"
#include "widgets/widgetinserttextsetting.h"

#include <QSettings>

namespace StringBuilder {

namespace Settings {
constexpr char groupName[] = "InsertText";
constexpr char keyText[] = "Text";
} // Settings

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
    const QString text = m_string.isEmpty()
                         ? QStringLiteral("<b>Insert Text</b>")
                         : QStringLiteral("<b>Insert Text</b> <i>%1</i>").arg(m_string);

    if (isLeftMost())
        return Html::leftAligned(QStringLiteral("&lt;&lt; %1").arg(text));

    if (isRightMost())
        return Html::rightAligned(QStringLiteral("%1 &gt;&gt;").arg(text));

    return Html::leftAligned(QStringLiteral("__%1 %2").arg(insertPosition()).arg(text));
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

void InsertString::loadSettings(QSettings *qSet)
{
    qSet->beginGroup(Settings::groupName);

    m_string = qSet->value(Settings::keyText, QString{}).toString();
    AbstractInsertString::loadSettings(qSet);

    qSet->endGroup();
}

void InsertString::saveSettings(QSettings *qSet) const
{
    qSet->beginGroup(Settings::groupName);

    qSet->setValue(Settings::keyText, m_string);
    AbstractInsertString::saveSettings(qSet);

    qSet->endGroup();
}

} // StringBuilder

