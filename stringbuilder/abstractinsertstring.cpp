/*
 * Copyright YEAR Takashi Inoue
 *
 * This file is part of APPNAME.
 *
 * APPNAME is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * APPNAME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with APPNAME.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "abstractinsertstring.h"

#include <QSettings>

namespace Settings {
constexpr char keyPosition[] = "Position";
} // Settings

StringBuilder::AbstractInsertString::AbstractInsertString()
    : AbstractInsertString(0) {}

StringBuilder::AbstractInsertString::AbstractInsertString(int pos, QObject *parent)
    : AbstractStringBuilder(parent),
      m_pos(pos)
{
}

qsizetype StringBuilder::AbstractInsertString::actualInsertPosition(qsizetype targetLength)
{
    if (m_pos == std::numeric_limits<int>::min())
        return 0;

    if (m_pos == std::numeric_limits<int>::max())
        return targetLength;

    return (m_pos < 0) ? qMax<qsizetype>(0, targetLength + m_pos)
                       : qMin<qsizetype>(m_pos, targetLength);
}

void StringBuilder::AbstractInsertString::loadSettings(QSettings *qSet)
{
    m_pos = qSet->value(Settings::keyPosition, 0).toInt();
}

void StringBuilder::AbstractInsertString::saveSettings(QSettings *qSet) const
{
    qSet->setValue(Settings::keyPosition, m_pos);
}
