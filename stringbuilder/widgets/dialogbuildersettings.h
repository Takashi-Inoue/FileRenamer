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

#include "usingstringbuilder.h"
#include "stringbuilder/onfile/builderchainonfile.h"

#include <QDialog>

class QTimer;

namespace StringBuilder {

class AbstractWidget;

namespace Ui {
class DialogBuilderSettings;
}

class DialogBuilderSettings : public QDialog
{
    Q_OBJECT

public:
    DialogBuilderSettings(QList<AbstractWidget *> widgets, QList<int> showIndexes,
                          QWidget *parent = nullptr);
    ~DialogBuilderSettings();

signals:
    void changeStarted();
    void settingsChanged(SharedBuilderChainOnFile);

private slots:
    void acceptSettingsWidgets();
    void notifyStartChanging();

private:
    Ui::DialogBuilderSettings *ui;

    QTimer *m_timer;
    QList<AbstractWidget *> m_widgets;
};


} // namespace StringBuilder
