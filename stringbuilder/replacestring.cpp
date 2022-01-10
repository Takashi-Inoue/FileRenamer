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

#include "replacestring.h"
#include "widgets/widgetreplacesetting.h"

#include <QRegularExpression>

namespace StringBuilder {

ReplaceString::ReplaceString()
    : ReplaceString(QString{}, QString{}, false, true, nullptr)
{
}

ReplaceString::ReplaceString(QStringView before, QStringView after,
                             bool isUseRegExp, bool isCaseSensitive, QObject *parent)
    : AbstractStringBuilder{parent},
      m_before{before.toString()},
      m_after{after.toString()},
      m_isUseRegExp{isUseRegExp},
      m_isCaseSensitive{isCaseSensitive}
{
}

void ReplaceString::build(QString &result)
{
    if (!m_isUseRegExp) {
        Qt::CaseSensitivity caseSensitivity = m_isCaseSensitive ? Qt::CaseSensitive
                                                                : Qt::CaseInsensitive;
        result.replace(m_before, m_after, caseSensitivity);
        return;
    }

    QRegularExpression::PatternOptions options
            = m_isCaseSensitive ? QRegularExpression::NoPatternOption
                                : QRegularExpression::CaseInsensitiveOption;

    result.replace(QRegularExpression{m_before, options}, m_after);
}

QString ReplaceString::toString() const
{
    const QString regExpOnOff(m_isUseRegExp ? QStringLiteral("On")
                                            : QStringLiteral("Off"));
    const QString caseSensitive(m_isCaseSensitive ? QStringLiteral("CaseSensitive")
                                                  : QStringLiteral("CaseInsensitive"));
    return QStringLiteral("%1 > %2 [RegExp:%3][%4]")
            .arg(m_before, m_after, regExpOnOff, caseSensitive);
}

AbstractWidget *ReplaceString::settingsWidget()
{
    auto widget = new WidgetReplaceSetting(m_before, m_after, m_isUseRegExp, m_isCaseSensitive);

    connect(widget, &AbstractWidget::accepted, this, [this]() {
        auto settingsWidget = qobject_cast<WidgetReplaceSetting *>(sender());

        m_before = settingsWidget->findString();
        m_after = settingsWidget->replaceString();
        m_isUseRegExp = settingsWidget->isUseRegExp();
        m_isCaseSensitive = settingsWidget->isCaseSensitive();
    });

    return widget;
}

} // StringBuilder

