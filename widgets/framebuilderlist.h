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

#pragma once

#include <QFrame>

namespace StringBuilder {
class BuildersModel;
class SettingsModel;
} // StringBuilder

namespace Ui {
class FrameBuilderList;
}

class FrameBuilderList : public QFrame
{
    Q_OBJECT
public:
    explicit FrameBuilderList(QWidget *parent = nullptr);
    ~FrameBuilderList() override;

signals:
    void changeStarted();
    void settingsChanged();

private slots:
    void on_buttonAdd_clicked();

    void onSettingActivated(const QModelIndex &index);
    void onSettingsChangeStarted();

private:
    Ui::FrameBuilderList *ui;

    StringBuilder::BuildersModel *m_buildersModel;
    StringBuilder::SettingsModel *m_settingsModel;
};

