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
#include "stringbuilder/stringbuildersettingsmodel.h"
#include "stringbuilder/stringbuildersmodel.h"
#include "stringbuilder/widgets/dialogbuildersettings.h"
#include "usingstringbuilder.h"

#include <QAction>
#include <QDir>
#include <QMenu>
#include <QSettings>
#include <QTimer>

namespace {

constexpr char lastSettingsFileName[] = "Last used.ini";

void initSettingsComboBox(QComboBox *combox)
{
    QDir dir{Application::settingsDirPath()};

    if (!dir.exists())
        return;

    QStringList &&inis = dir.entryList({QStringLiteral("*.ini")}, QDir::Files, QDir::Name);
    qsizetype indexLastUsed = inis.indexOf(QString{lastSettingsFileName});

    if (indexLastUsed != -1)
        inis.removeAt(indexLastUsed);

    combox->addItem(QStringLiteral("New settings"));

    for (QStringView iniName : inis)
        combox->addItem(iniName.chopped(4).toString(), iniName.toString());

    if (indexLastUsed != -1)
        combox->addItem(QString{lastSettingsFileName}.chopped(4), QString{lastSettingsFileName});
}

} // anonymous

FrameBuilderList::FrameBuilderList(QWidget *parent)
    : QFrame(parent),
      ui(new Ui::FrameBuilderList),
      m_timer(new QTimer{this}),
      m_buildersModel(new StringBuilder::BuildersModel{this}),
      m_settingsModel(new StringBuilder::SettingsModel{this})
{
    ui->setupUi(this);

    ui->tableViewBuilders->setModel(m_buildersModel);
    ui->tableViewSettings->setModel(m_settingsModel);
    ui->tableViewSettings->setItemDelegate(new HtmlTextDelegate{this});

    auto deleteAction = GenericActions::createDeleteAction(QIcon{QStringLiteral(":/x.svg")}, this);

    ui->tableViewSettings->addAction(deleteAction);
    ui->tableViewSettings->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(deleteAction, &QAction::triggered, this, &FrameBuilderList::deleteSelectedSettings);

    connect(ui->tableViewSettings, &QWidget::customContextMenuRequested,
            this, &FrameBuilderList::showSettingsViewContextMenu);

    connect(ui->tableViewSettings, &QAbstractItemView::activated,
            this, &FrameBuilderList::onSettingActivated);

    connect(ui->buttonAdd, &QPushButton::clicked,
            this, &FrameBuilderList::appendSelectedBuildersToSettings);
    connect(ui->tableViewBuilders, &QAbstractItemView::activated,
            this, &FrameBuilderList::appendSelectedBuildersToSettings);

    connect(m_settingsModel, &StringBuilder::SettingsModel::settingsChanged,
            this, &FrameBuilderList::settingsChanged);

    m_timer->setSingleShot(true);
    m_timer->setInterval(QApplication::keyboardInputInterval());

    connect(m_timer, &QTimer::timeout, this, [this]() {
        emit settingsChanged(builderChain());
    });

    initSettingsComboBox(ui->comboxSettings);

    connect(ui->comboxSettings, &QComboBox::currentIndexChanged,
            this, &FrameBuilderList::loadSettings);

    const int lastIndex = ui->comboxSettings->count() - 1;

    ui->comboxSettings->itemData(lastIndex) == QString{lastSettingsFileName}
        ? ui->comboxSettings->setCurrentIndex(lastIndex)
        : ui->comboxSettings->setCurrentIndex(0);
}

FrameBuilderList::~FrameBuilderList()
{
    QSettings qSet{Application::settingsIniPath(QString{lastSettingsFileName}),
                QSettings::IniFormat};

    m_settingsModel->saveSettings(&qSet);

    delete ui;
}

SharedBuilderChainOnFile FrameBuilderList::builderChain() const
{
    return m_settingsModel->builderChain();
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

void FrameBuilderList::deleteSelectedSettings()
{
    m_settingsModel->removeSpecifiedRows(ui->tableViewSettings->selectionModel()->selectedRows());
}

void FrameBuilderList::loadSettings(int comboBoxIndex)
{
    QVariant &&iniFileNameData = ui->comboxSettings->itemData(comboBoxIndex);

    if (!iniFileNameData.isValid()) {
        m_settingsModel->clearSettings();
        return;
    }

    QSettings qSet{Application::settingsIniPath(iniFileNameData.toString()), QSettings::IniFormat};

    m_settingsModel->loadSettings(&qSet);
}

void FrameBuilderList::onSettingActivated(const QModelIndex &/*index*/)
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

void FrameBuilderList::showSettingsViewContextMenu(const QPoint &/*pos*/)
{
    if (!ui->tableViewSettings->selectionModel()->hasSelection())
        return;

    auto menu = new QMenu{this};

    connect(menu, &QMenu::aboutToHide, this, [this]() {
        sender()->deleteLater();
    });

    menu->addActions(ui->tableViewSettings->actions());
    menu->popup(QCursor::pos());
}

void FrameBuilderList::notifyStartChanging()
{
    if (m_settingsModel->isEmpty())
        emit builderCleared();

    m_timer->start();
    emit changeStarted();
}
