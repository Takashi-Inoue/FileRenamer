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

#include "widgetapplicationlogs.h"
#include "ui_widgetapplicationlogs.h"

#include "applicationlog/applicationlog.h"

WidgetApplicationLogs::WidgetApplicationLogs(QWidget *parent) :
    QWidget{parent},
    ui{new Ui::WidgetApplicationLogs}
{
    ui->setupUi(this);

    ui->tableView->setModel(&ApplicationLog::instance());

    QHeaderView *header = ui->tableView->horizontalHeader();

    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
}

WidgetApplicationLogs::~WidgetApplicationLogs()
{
    delete ui;
}
