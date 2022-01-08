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

#include <QFrame>
#include <QSettings>

namespace Ui {
class WidgetPositionFixer;
}

class WidgetPositionFixer : public QFrame
{
    Q_OBJECT
public:
    explicit WidgetPositionFixer(QWidget *parent = nullptr);
    ~WidgetPositionFixer() override;

    void loadSettings(QSharedPointer<QSettings>);
    void saveSettings(QSharedPointer<QSettings>) const;

    void setValue(int value);
    int value() const;

signals:
    void changeStarted();

private slots:
    void onPushButtonClicked(bool);

private:
    enum PositionAlignment : int {
        Leftmost, Rightmost, Specified
    };

    void setAlignmentUI(PositionAlignment alignment);

    Ui::WidgetPositionFixer *ui;
};
