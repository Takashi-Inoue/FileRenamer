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

#include "number.h"
#include "utilityshtml.h"
#include "widgets/widgetnumbersetting.h"

namespace StringBuilder {

Number::Number()
    : Number(0, 0, 1, 0, QString{}, QString{}, nullptr)
{
}

Number::Number(int pos, int start, int step, int digit, QStringView prefix, QStringView suffix,
               QObject *parent)
    : AbstractInsertString{pos, parent},
      m_start(start),
      m_step(step),
      m_digit(digit),
      m_prefix(prefix.toString()),
      m_suffix(suffix.toString()),
      m_currentNumber(start)
{
}

void Number::build(QString &result)
{
    QString numberString = QStringLiteral("%1%2%3")
                           .arg(m_prefix)
                           .arg(m_currentNumber, m_digit, 10, QLatin1Char('0'))
                           .arg(m_suffix);

    result.insert(actualInsertPosition(result.size()), numberString);

    m_currentNumber += m_step;
}

QString Number::toHtmlString() const
{
    const QString baseText = QStringLiteral("<b>Number</b> <i>%1%2%3</i>&nbsp;&nbsp;[Incremental:%4]")
                             .arg(m_prefix)
                             .arg(m_start, m_digit, 10, QLatin1Char('0'))
                             .arg(m_suffix)
                             .arg(m_step);

    if (isLeftMost())
        return Html::leftAligned(QStringLiteral("&lt;&lt; %1").arg(baseText));

    if (isRightMost())
        return Html::rightAligned(QStringLiteral("%1 &gt;&gt;").arg(baseText));

    return Html::leftAligned(QStringLiteral("__%1 %2").arg(insertPosition()).arg(baseText));
}

void Number::reset()
{
    m_currentNumber = m_start;
}

AbstractWidget *Number::settingsWidget()
{
    auto widget = new WidgetNumberSetting(m_start, m_step, m_digit, m_prefix, m_suffix, insertPosition());

    connect(widget, &AbstractWidget::accepted, this, [this]() {
        auto settingsWidget = qobject_cast<WidgetNumberSetting *>(sender());

        m_start = settingsWidget->startNumber();
        m_step = settingsWidget->incrementalNumber();
        m_digit = settingsWidget->digits();
        m_prefix = settingsWidget->prefixString();
        m_suffix = settingsWidget->suffixString();

        setInsertPosition(settingsWidget->insertPosition());
    });

    return widget;
}

} // StringBuilder

