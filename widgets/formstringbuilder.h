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
#include <QSharedPointer>

namespace StringBuilder {
class AbstractStringBuilder;
class AbstractWidget;
}

class QPushButton;
class QSettings;

namespace Ui {
class FormStringBuilder;
}

class FormStringBuilder : public QFrame
{
    Q_OBJECT

public:
    explicit FormStringBuilder(QWidget *parent = nullptr);
    ~FormStringBuilder() override;

    QSharedPointer<StringBuilder::AbstractStringBuilder> stringBuilder() const;

    int currentBuilderIndex() const;
    void setCurrentBuilderIndex(int index);

    void loadBuilderSettings(QSharedPointer<QSettings>);
    void saveCurrentBuilderSettings(QSharedPointer<QSettings>) const;

    void notifySettingIndexChanged(int index, int settingsCount);

signals:
    void requestUp();
    void requestDown();
    void requestRemove();
    void changeStarted();

private slots:
    void onComboBoxBuildersCurrentIndexChanged(int);

private:
    void addBuilderWidget(int builderIndex);
    StringBuilder::AbstractWidget *currentBuilderWidget() const;

    Ui::FormStringBuilder *ui;
};
