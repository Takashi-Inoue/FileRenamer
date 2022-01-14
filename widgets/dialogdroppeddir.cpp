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

#include "dialogdroppeddir.h"
#include "ui_dialogdroppeddir.h"

#include "searchindirs.h"

#include <QApplication>
#include <QFileIconProvider>
#include <QSettings>

namespace {
constexpr char settingsGroupName[] = "Search";
constexpr char settingsKeyFilter[] = "Filter";
constexpr char settingsKeyFilterHistory[] = "FilterHistory";
constexpr char settingsKeyHierarchy[] = "Hierarchy";
constexpr char settingsKeySearchDirs[] = "SearchDirs";
constexpr char settingsKeySearchFiles[] = "SearchFiles";
}

DialogDroppedDir::DialogDroppedDir(const QList<ParentChildrenPair> &dirs, QWidget *parent)
    : QDialog{parent},
      ui{new Ui::DialogDroppedDir},
      m_dirsToSearch{dirs}
{
    ui->setupUi(this);

    const QString headerText = (dirs.size() == 1 && dirs[0].second.size() == 1)
            ? tr("Directory") : tr("Directories");
    ui->treeWidget->headerItem()->setText(0, headerText);

    QIcon dirIcon = QFileIconProvider().icon(QAbstractFileIconProvider::Folder);

    for (const ParentChildrenPair &parentChildren : dirs) {
        auto pathItem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), {parentChildren.first});
        ui->treeWidget->addTopLevelItem(pathItem);

        for (QStringView childName : parentChildren.second) {
            auto item = new QTreeWidgetItem(pathItem, {childName.toString()});
            item->setIcon(0, dirIcon);
        }
    }

    ui->treeWidget->expandAll();

    loadSettings();
}

DialogDroppedDir::~DialogDroppedDir()
{
    delete ui;
}

QList<DialogDroppedDir::ParentChildrenPair> DialogDroppedDir::dirsToRename() const
{
    return m_dirs;
}

QList<DialogDroppedDir::ParentChildrenPair> DialogDroppedDir::filesToRename() const
{
    return m_files;
}

void DialogDroppedDir::onPushButtonOkClicked()
{
    saveSettings();

    SearchInDirs::Settings searchSettings = {
        ui->checkBoxDirs->isChecked(),
        ui->checkBoxFiles->isChecked(),
        ui->spinBoxHierarchy->value(),
        fixFiltersString(ui->comboBoxFilter->currentText()).split(';', Qt::SkipEmptyParts)
    };

    SearchInDirs searchInDirs(searchSettings);

    searchInDirs.exec(m_dirsToSearch);
    m_dirs = searchInDirs.dirs();
    m_files = searchInDirs.files();

    accept();
}

QString DialogDroppedDir::fixFiltersString(QStringView filtersString) const
{
    QStringList filters = filtersString.toString().split(';', Qt::SkipEmptyParts);

    for (QString &filter : filters) {
        if (!filter.contains('*') && !filter.contains('?'))
            filter = QStringLiteral("*%1*").arg(filter);
    }

    return filters.join(';');
}

QString DialogDroppedDir::iniFilePath() const
{
    return QApplication::applicationDirPath() + "/search.ini";
}

void DialogDroppedDir::loadSettings()
{
    QSettings qSettings(iniFilePath(), QSettings::IniFormat);

    qSettings.beginGroup(settingsGroupName);

    QStringList filtersHistory = qSettings.value(settingsKeyFilterHistory).toStringList();

    for (QStringView filtersString : filtersHistory)
        ui->comboBoxFilter->addItem(filtersString.toString());

    ui->checkBoxDirs->setChecked(qSettings.value(settingsKeySearchDirs, false).toBool());
    ui->checkBoxFiles->setChecked(qSettings.value(settingsKeySearchFiles, true).toBool());
    ui->comboBoxFilter->setCurrentText(qSettings.value(settingsKeyFilter).toString());
    ui->spinBoxHierarchy->setValue(qSettings.value(settingsKeyHierarchy, 0).toInt());

    qSettings.endGroup();
}

void DialogDroppedDir::saveSettings() const
{
    QString filtersString = ui->comboBoxFilter->currentText();

    filtersString = fixFiltersString(filtersString);

    QSettings qSettings(iniFilePath(), QSettings::IniFormat);

    qSettings.beginGroup(settingsGroupName);

    qSettings.setValue(settingsKeySearchDirs, ui->checkBoxDirs->isChecked());
    qSettings.setValue(settingsKeySearchFiles, ui->checkBoxFiles->isChecked());
    qSettings.setValue(settingsKeyFilter, filtersString);
    qSettings.setValue(settingsKeyHierarchy, ui->spinBoxHierarchy->value());

    QStringList filtersList;

    for (int i = 0, count = ui->comboBoxFilter->count(); i < count; ++i)
        filtersList << fixFiltersString(ui->comboBoxFilter->itemText(i));

    filtersList.prepend(filtersString);
    filtersList.removeDuplicates();

    qSettings.setValue(settingsKeyFilterHistory, filtersList);

    qSettings.endGroup();
}
