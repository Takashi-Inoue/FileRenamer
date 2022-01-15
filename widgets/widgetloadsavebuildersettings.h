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

#include <QWidget>

namespace StringBuilder { class SettingsModel; }

class RenameSettingsModel;

class QMenu;

namespace Ui { class WidgetLoadSaveBuilderSettings; }

class WidgetLoadSaveBuilderSettings : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetLoadSaveBuilderSettings(StringBuilder::SettingsModel *settingsModel,
                                           QWidget *parent = nullptr);
    ~WidgetLoadSaveBuilderSettings();

    QWidget *lastTabOrderWidget() const;
    void saveLatestSettings() const;

private slots:
    void loadSettings(int comboBoxIndex);
    void saveOverwrite() const;
    void saveNewSettings() const;

private:
    Ui::WidgetLoadSaveBuilderSettings *ui;

    StringBuilder::SettingsModel *m_settingsModel;
    RenameSettingsModel *m_settingsListModel;
    QAction *m_actionSave = nullptr;
};

