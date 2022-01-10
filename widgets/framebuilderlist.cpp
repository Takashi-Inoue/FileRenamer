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

#include "framebuilderlist.h"
#include "ui_framebuilderlist.h"

#include "htmltextdelegate.h"
#include "stringbuilder/stringbuildersettingsmodel.h"
#include "stringbuilder/stringbuildersmodel.h"
#include "usingstringbuilder.h"

FrameBuilderList::FrameBuilderList(QWidget *parent)
    : QFrame(parent),
      ui(new Ui::FrameBuilderList),
      m_buildersModel(new StringBuilder::BuildersModel{this}),
      m_settingsModel(new StringBuilder::SettingsModel{this})
{
    ui->setupUi(this);

    ui->tableViewBuilders->setModel(m_buildersModel);
    ui->tableViewSettings->setModel(m_settingsModel);

    connect(ui->tableViewSettings, &QAbstractItemView::activated,
            this, &FrameBuilderList::onSettingActivated);

    ui->tableViewSettings->setItemDelegate(new HtmlTextDelegate{this});
}

FrameBuilderList::~FrameBuilderList()
{
    delete ui;
}

void FrameBuilderList::on_buttonAdd_clicked()
{
    const QModelIndexList &indexes = ui->tableViewBuilders->selectionModel()->selectedRows();

    m_settingsModel->appendBuilders(m_buildersModel->createBuilders(indexes));
}

void FrameBuilderList::onSettingActivated(const QModelIndex &index)
{
}

void FrameBuilderList::onSettingsChangeStarted()
{

}

