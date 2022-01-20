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

namespace StringBuilder { class BuilderChainModel; }

class SavedSettingsModel;

class QMenu;

namespace Ui { class WidgetLoadSaveBuilderSettings; }

class WidgetLoadSaveBuilderSettings : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetLoadSaveBuilderSettings(StringBuilder::BuilderChainModel *builderChainModel,
                                           QWidget *parent = nullptr);
    ~WidgetLoadSaveBuilderSettings();

    QWidget *lastTabOrderWidget() const;
    void saveLastUsedSettings() const;
    void saveLastTimeSettings() const;

private slots:
    void saveOverwrite();
    void saveNewSettings();

private:
    Ui::WidgetLoadSaveBuilderSettings *ui;

    StringBuilder::BuilderChainModel *m_builderChainModel;
    SavedSettingsModel *m_savedSettingsModel;
    QAction *m_actionSave = nullptr;

    QString m_iniBaseToLoadAtNextTime;
};

