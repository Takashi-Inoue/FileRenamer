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

#include "dialogsettingslistconfigurator.h"
#include "ui_dialogsettingslistconfigurator.h"

#include "renamesettingsmodel.h"

DialogSettingsListConfigurator::DialogSettingsListConfigurator(
        RenameSettingsModel *model, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DialogSettingsListConfigurator)
{
    ui->setupUi(this);

    ui->actionDelete->setShortcut(QKeySequence::Delete);
    ui->tableView->setModel(model);

    connect(model, SIGNAL(loaded()), this, SLOT(hideNotEditableSettings()));
    connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(removeSelectedSettings()));
    connect(ui->actionReload, SIGNAL(triggered()), model, SLOT(load()));
    connect(ui->actionRename, &QAction::triggered, this, [this]() {
        ui->tableView->edit(ui->tableView->currentIndex());
    });

    hideNotEditableSettings();
}

DialogSettingsListConfigurator::~DialogSettingsListConfigurator()
{
    delete ui;
}

void DialogSettingsListConfigurator::hideNotEditableSettings()
{
    ui->tableView->hideRow(0);
    ui->tableView->hideRow(ui->tableView->model()->rowCount() - 1);
}

void DialogSettingsListConfigurator::removeSelectedSettings()
{
    const QModelIndexList &indexes = ui->tableView->selectionModel()->selectedRows();

    for (auto rItr = indexes.crbegin(), rEnd = indexes.crend(); rItr != rEnd; ++rItr)
        ui->tableView->model()->removeRow(rItr->row());
}
