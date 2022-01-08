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

#include "widgetonlypositionfixer.h"
#include "ui_widgetonlypositionfixer.h"

WidgetOnlyPositionFixer::WidgetOnlyPositionFixer(QWidget *parent)
    : AbstractStringBuilderWidget{parent},
      ui{new Ui::WidgetOnlyPositionFixer}
{
    ui->setupUi(this);

    ui->labelTitle->setText(windowTitle());

    connect(ui->widgetPositionFixer, &WidgetPositionFixer::changeStarted,
            this, &AbstractStringBuilderWidget::changeStarted);
}

WidgetOnlyPositionFixer::~WidgetOnlyPositionFixer()
{
    delete ui;
}

void WidgetOnlyPositionFixer::loadSettings(QSharedPointer<QSettings> qSettings)
{
    ui->widgetPositionFixer->loadSettings(qSettings);
}

void WidgetOnlyPositionFixer::saveSettings(QSharedPointer<QSettings> qSettings) const
{
    ui->widgetPositionFixer->saveSettings(qSettings);
}

int WidgetOnlyPositionFixer::positionToInsert() const
{
    return ui->widgetPositionFixer->value();
}

void WidgetOnlyPositionFixer::setPositionToInsert(int pos)
{
    ui->widgetPositionFixer->setValue(pos);
}

void WidgetOnlyPositionFixer::setTitle(QStringView title)
{
    ui->labelTitle->setText(title.toString());
}
