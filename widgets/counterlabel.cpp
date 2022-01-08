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

#include "counterlabel.h"

CounterLabel::CounterLabel(QStringView singleNumberSuffix, QStringView multiNumberSuffix,
                           QWidget *parent)
    : QLabel{QStringLiteral("0 %1").arg(singleNumberSuffix), parent},
      m_singleSuffix{singleNumberSuffix.toString()},
      m_multiSuffix{multiNumberSuffix.toString()}
{
}

CounterLabel::CounterLabel(QStringView suffix, QWidget *parent)
    : CounterLabel(suffix, suffix, parent)
{
}

void CounterLabel::setCount(int m_count)
{
    QString countText = (m_count > 1) ? QStringLiteral("%1 %2").arg(m_count).arg(m_multiSuffix)
                                      : QStringLiteral("%1 %2").arg(m_count).arg(m_singleSuffix);

    setText(countText);
}
