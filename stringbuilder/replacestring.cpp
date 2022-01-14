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
#include "utilityshtml.h"
#include "widgets/widgetreplacesetting.h"

#include <QRegularExpression>
#include <QSettings>

namespace StringBuilder {

namespace Settings {
constexpr char groupName[] = "ReplaceText";
constexpr char keyFind[] = "Find";
constexpr char keyReplace[] = "Replace";
constexpr char keyIsUseRegExp[] = "UseRegExp";
constexpr char keyIsCaseSensitive[] = "CaseSensitive";
} // Settings

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

QString ReplaceString::toHtmlString() const
{
    const QString regExpOnOff(m_isUseRegExp ? tr("On") : tr("Off"));
    const QString caseSensitive(m_isCaseSensitive ? tr("CaseSensitive") : tr("CaseInsensitive"));

    const QString text = tr("<b>Replace</b> <i>%1</i> &gt; <i>%2</i> [RegExp:%3][%4]")
                         .arg(m_before, m_after, regExpOnOff, caseSensitive);
    return Html::centerAligned(text);
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

void ReplaceString::loadSettings(QSettings *qSet)
{
    qSet->beginGroup(Settings::groupName);

    m_before = qSet->value(Settings::keyFind, QString{}).toString();
    m_after = qSet->value(Settings::keyReplace, QString{}).toString();
    m_isUseRegExp = qSet->value(Settings::keyIsUseRegExp, false).toBool();
    m_isCaseSensitive = qSet->value(Settings::keyIsCaseSensitive, true).toBool();

    qSet->endGroup();
}

void ReplaceString::saveSettings(QSettings *qSet) const
{
    qSet->beginGroup(Settings::groupName);

    qSet->setValue(Settings::keyFind, m_before);
    qSet->setValue(Settings::keyReplace, m_after);
    qSet->setValue(Settings::keyIsUseRegExp, m_isUseRegExp);
    qSet->setValue(Settings::keyIsCaseSensitive, m_isCaseSensitive);

    qSet->endGroup();
}

} // StringBuilder

