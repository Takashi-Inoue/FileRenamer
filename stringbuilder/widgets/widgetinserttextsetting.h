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

namespace StringBuilder {

namespace Ui {
class WidgetInsertTextSetting;
}

class WidgetInsertTextSetting : public AbstractWidget
{
    Q_OBJECT

public:
    explicit WidgetInsertTextSetting(QWidget *parent = nullptr);
    ~WidgetInsertTextSetting() override;

    // StringBuilder::AbstractWidget interface
    QSharedPointer<AbstractWidget> clone() const override;
    QString builderName() const override;
    QString toString() const override;
    QFont fontForDisplay() const override;
    Qt::Alignment alignForDisplay() const override;

    QSharedPointer<AbstractStringBuilder> stringBuilder() const override;
    void loadSettings(QSharedPointer<QSettings>) override;
    void saveSettings(QSharedPointer<QSettings>) const override;

protected:
    void changeEvent(QEvent *event) override;

private:
    Ui::WidgetInsertTextSetting *ui;
};

} // StringBuilder
