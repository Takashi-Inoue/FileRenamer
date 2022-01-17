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

#include "application.h"
#include "genericactions.h"
#include "htmltextdelegate.h"
#include "stringbuilder/stringbuilderchainmodel.h"
#include "stringbuilder/stringbuildersmodel.h"
#include "stringbuilder/widgets/dialogbuildersettings.h"
#include "usingstringbuilder.h"
#include "widgetloadsavebuildersettings.h"

#include <QAction>
#include <QMenu>
#include <QSettings>
#include <QTimer>

namespace {

void initSettingsTableView(FrameBuilderList *builderList, Ui::FrameBuilderList *ui,
                           StringBuilder::BuilderChainModel *model)
{
    ui->tableViewSettings->setModel(model);
    ui->tableViewSettings->setItemDelegate(new HtmlTextDelegate{ui->tableViewSettings});

    ui->tableViewSettings->addActions({ui->actionEdit, ui->actionDelete});
    ui->tableViewSettings->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->actionEdit->connect(ui->tableViewSettings, SIGNAL(activated(QModelIndex)), SLOT(trigger()));

    builderList->connect(model, SIGNAL(settingsChanged(SharedBuilderChainOnFile)),
                         SIGNAL(settingsChanged(SharedBuilderChainOnFile)));
}

} // anonymous

//--------------------------------------------------------------------------------------------------
FrameBuilderList::FrameBuilderList(QWidget *parent)
    : QFrame(parent),
      ui(new Ui::FrameBuilderList),
      m_timer(new QTimer{this}),
      m_buildersModel(new StringBuilder::BuildersModel{this}),
      m_settingsModel(new StringBuilder::BuilderChainModel{this})
{
    ui->setupUi(this);

    ui->tableViewBuilders->setModel(m_buildersModel);

    initSettingsTableView(this, ui, m_settingsModel);

    connect(ui->tableViewSettings->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this]() {
        bool hasSelection = ui->tableViewSettings->selectionModel()->hasSelection();
        ui->actionDelete->setEnabled(hasSelection);
        ui->actionEdit->setEnabled(hasSelection);
    });

    connect(ui->actionEdit, SIGNAL(triggered(bool)), this, SLOT(showSettingDialog()));
    connect(ui->actionDelete, &QAction::triggered, this, [this]() {
        m_settingsModel->removeSpecifiedRows(ui->tableViewSettings->selectionModel()->selectedRows());
    });

    connect(ui->actionAddBuilders, &QAction::triggered,
            this, &FrameBuilderList::appendSelectedBuildersToSettings);

    connect(ui->buttonAdd, &QPushButton::clicked, ui->actionAddBuilders, &QAction::trigger);
    connect(ui->tableViewBuilders, &QTableView::activated, ui->actionAddBuilders, &QAction::trigger);

    m_timer->setSingleShot(true);
    m_timer->setInterval(QApplication::keyboardInputInterval());

    connect(m_timer, &QTimer::timeout, this, [this]() {
        emit settingsChanged(builderChain());
    });

    // Init m_widgetLoadSaver after finished connecting.
    m_widgetLoadSaver = new WidgetLoadSaveBuilderSettings{m_settingsModel, this};
    ui->hLayoutLoadSaver->addWidget(m_widgetLoadSaver);

    setTabOrder(m_widgetLoadSaver->lastTabOrderWidget(), ui->tableViewSettings);
    setTabOrder(ui->tableViewSettings, ui->buttonAdd);
    setTabOrder(ui->buttonAdd, ui->tableViewBuilders);
}

FrameBuilderList::~FrameBuilderList()
{
    m_widgetLoadSaver->saveLastTimeSettings();

    delete ui;
}

SharedBuilderChainOnFile FrameBuilderList::builderChain() const
{
    return m_settingsModel->builderChain();
}

void FrameBuilderList::saveLastUsedSettings() const
{
    m_widgetLoadSaver->saveLastUsedSettings();
}

void FrameBuilderList::appendSelectedBuildersToSettings()
{
    QModelIndexList &&indexes = ui->tableViewBuilders->selectionModel()->selectedRows();

    std::sort(indexes.begin(), indexes.end(), [](const QModelIndex &lhs, const QModelIndex &rhs) {
        return lhs.row() < rhs.row();
    });

    m_settingsModel->appendBuilders(m_buildersModel->createBuilders(indexes));

    notifyStartChanging();
}

void FrameBuilderList::showSettingDialog()
{
    QList<int> rows;

    for (const QModelIndex &index : ui->tableViewSettings->selectionModel()->selectedRows())
        rows.append(index.row());

    using DialogSettings = StringBuilder::DialogBuilderSettings;
    DialogSettings *dlg = m_settingsModel->settingsDialog(rows, this);

    connect(dlg, &DialogSettings::changeStarted, this, &FrameBuilderList::changeStarted);
    connect(dlg, &DialogSettings::settingsChanged, this, &FrameBuilderList::settingsChanged);

    dlg->exec();

    for (const QModelIndex &index : ui->tableViewSettings->selectionModel()->selectedRows())
        ui->tableViewSettings->update(index);

    notifyStartChanging();
}

void FrameBuilderList::notifyStartChanging()
{
    if (m_settingsModel->isEmpty())
        emit builderCleared();

    m_timer->start();
    emit changeStarted();
}
