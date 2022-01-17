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

#include "logdata.h"

#include <QFile>

LogData::LogData(QStringView group, QStringView log)
    : m_time(QDateTime::currentDateTime()),
      m_group(group.toString()),
      m_log(log.toString())
{
}

QString LogData::time(QStringView format) const
{
    return m_time.toString(format);
}

QString LogData::group() const
{
    return m_group;
}

QString LogData::log() const
{
    return m_log;
}

void LogData::write(QFile &file) const
{
    QString line = QStringLiteral("[%1] <%2>:%3")
                   .arg(time(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz")), m_group, m_log);
    file.write(line.toUtf8());
    file.write("\r\n");
}
