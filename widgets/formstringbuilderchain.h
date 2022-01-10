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

#include <QWidget>

#include "stringbuilder/onfile/builderchainonfile.h"

class FormStringBuilder;

class QSettings;
class QTimer;

namespace Ui {
class FormStringBuilderChain;
}

class FormStringBuilderChain : public QWidget
{
    Q_OBJECT

public:
    explicit FormStringBuilderChain(QWidget *parent = nullptr);
    ~FormStringBuilderChain() override;

    QSharedPointer<StringBuilder::OnFile::BuilderChainOnFile> builderChain() const;
    void loadBuilderSettings(QSharedPointer<QSettings>);
    void saveCurrentBuilderSettings(QSharedPointer<QSettings>) const;

signals:
    void builderCleared();
    void changeStarted();
    void settingsChanged(QSharedPointer<StringBuilder::OnFile::BuilderChainOnFile>);
    void settingsIndicesChanged();

public slots:
    FormStringBuilder *createNewSetting(int builderIndex = 0);
    void saveLatestSettings() const;

private slots:
    void startTimer();
    void onBuilderRequestedDown();
    void onBuilderRequestedUp();
    void onBuilderRequestedRemove();
    void onSettingsIndicesChanged();

private:
    void removeAllBuilderWidgets();
    int settingsCount() const;

    Ui::FormStringBuilderChain *ui;
    QTimer *m_timer;
};
