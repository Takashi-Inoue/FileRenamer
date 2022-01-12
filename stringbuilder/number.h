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

#include "abstractinsertstring.h"

namespace StringBuilder {

class Number : public AbstractInsertString
{
    Q_OBJECT
public:
    Number();
    Number(int pos, int start, int step, int digit, QStringView prefix, QStringView suffix,
           QObject *parent = nullptr);

    void build(QString &result) override;
    QString toHtmlString() const override;
    void reset() override;
    AbstractWidget *settingsWidget() override;

    void loadSettings(QSettings *qSet) override;
    void saveSettings(QSettings *qSet) const override;

private:
    int m_start;
    int m_step;
    int m_digit;
    QString m_prefix;
    QString m_suffix;

    int m_currentNumber;
};

} // StringBuilder
