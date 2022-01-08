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

#include "savedsettingslistwidget.h"

#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QMessageBox>

#include <QDebug>

SavedSettingsListWidget::SavedSettingsListWidget(QWidget *parent)
    : QListWidget(parent),
      m_deleteAction(new QAction(QIcon(":/res/images/clear.svg"), QStringLiteral("Delete"), this))
{
    m_deleteAction->setShortcutContext(Qt::WidgetShortcut);
    m_deleteAction->setShortcut(QKeySequence(QKeySequence::Delete));
    m_deleteAction->setEnabled(false);

    connect(m_deleteAction, &QAction::triggered,
            this, &SavedSettingsListWidget::onActionDeleteTriggered);

    connect(this, &QListWidget::itemSelectionChanged,
            this, &SavedSettingsListWidget::onItemSelectionChanged);

    addAction(m_deleteAction);
}

QString SavedSettingsListWidget::settingsDir() const
{
    return QApplication::applicationDirPath() + QStringLiteral("/settings");
}

void SavedSettingsListWidget::load()
{
    clear();

    QDir dir(settingsDir());

    if (!dir.exists())
        return;

    QStringList settingNames = dir.entryList({QStringLiteral("*.ini")}, QDir::Files, QDir::Name);

    for (QString &name : settingNames)
        name.chop(4);

    addItems(settingNames);
}

void SavedSettingsListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    m_deleteAction->setEnabled(itemAt(event->pos()) != nullptr);

    QMenu menu(this);

    menu.setAttribute(Qt::WA_DeleteOnClose);
    menu.addAction(m_deleteAction);
    menu.exec(mapToGlobal(event->pos()));
}

void SavedSettingsListWidget::showEvent(QShowEvent *event)
{
    QListWidget::showEvent(event);

    load();
}

void SavedSettingsListWidget::onActionDeleteTriggered()
{
    QListWidgetItem *currentItem = this->currentItem();

    if (currentItem == nullptr)
        return;

    const QString currentText = currentItem->text();
    const QString currentTextBold = QStringLiteral("<b>%1</b>").arg(currentText);

    QString msg = QStringLiteral("%1<br>Would you like to delete this setting?").arg(currentTextBold);

    if (QMessageBox::question(this, QStringLiteral("Confirm Delete"), msg) != QMessageBox::Yes)
        return;

    QString settingFilePath = QString("%1/%2.ini").arg(settingsDir(), currentText);

    msg = QStringLiteral("%1<br>Failed to delete this setting.").arg(currentTextBold);


    if (QFile::moveToTrash(settingFilePath))
        delete takeItem(currentRow());
    else
        QMessageBox::warning(this, QStringLiteral("Failed to Delete"), msg);
}

void SavedSettingsListWidget::onItemSelectionChanged()
{
    m_deleteAction->setEnabled(selectionModel()->hasSelection());
}
