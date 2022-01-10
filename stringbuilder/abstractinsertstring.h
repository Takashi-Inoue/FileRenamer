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

#pragma once

#include "abstractstringbuilder.h"

namespace StringBuilder {

class AbstractInsertString : public AbstractStringBuilder
{
    Q_OBJECT
public:
    AbstractInsertString()
        : AbstractInsertString(0, nullptr)
    {
    }

    AbstractInsertString(int pos, QObject *parent = nullptr)
        : AbstractStringBuilder(parent),
          m_pos(pos)
    {
    }

    inline bool isLeftMost() const
    {
        return m_pos == std::numeric_limits<int>::min();
    }

    inline bool isRightMost() const
    {
        return m_pos == std::numeric_limits<int>::max();
    }

    int insertPosition() const
    {
        return m_pos;
    }

    void setInsertPosition(int pos)
    {
        m_pos = pos;
    }

protected:
    qsizetype posToInsert(qsizetype targetLength)
    {
        if (m_pos == std::numeric_limits<int>::min())
            return 0;

        if (m_pos == std::numeric_limits<int>::max())
            return targetLength;

        return (m_pos < 0) ? qMax<qsizetype>(0, targetLength + m_pos)
                           : qMin<qsizetype>(m_pos, targetLength);
    }

    int m_pos;
};

} // StringBuilder
