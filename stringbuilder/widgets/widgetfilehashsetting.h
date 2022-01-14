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

#include "abstractstringbuilderwidget.h"

#include <QCryptographicHash>

namespace StringBuilder {

namespace Ui {
class WidgetFileHashSetting;
}

class WidgetFileHashSetting : public AbstractWidget
{
    Q_OBJECT
public:
    explicit WidgetFileHashSetting(QWidget *parent = nullptr);
    WidgetFileHashSetting(QCryptographicHash::Algorithm algorithm, int insertPos,
                          QWidget *parent = nullptr);
    ~WidgetFileHashSetting() override;

    // StringBuilder::AbstractWidget interface
    QSharedPointer<AbstractStringBuilder> stringBuilder() const override;

    QCryptographicHash::Algorithm algorithm() const;
    int insertPosition() const;

private:
    Ui::WidgetFileHashSetting *ui;
};

} // StringBuilder
