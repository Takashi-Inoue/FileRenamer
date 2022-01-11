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

class ReplaceString : public AbstractStringBuilder
{
    Q_OBJECT
public:
    ReplaceString();
    ReplaceString(QStringView before, QStringView after, bool isUseRegExp, bool isCaseSensitive,
                  QObject *parent = nullptr);

    void build(QString &result) override;
    QString toHtmlString() const override;
    AbstractWidget *settingsWidget() override;

private:
    QString m_before;
    QString m_after;
    bool m_isUseRegExp;
    bool m_isCaseSensitive;
};

} // StringBuilder
