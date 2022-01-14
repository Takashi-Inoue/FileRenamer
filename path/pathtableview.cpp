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

#include "pathtableview.h"
#include "pathtableviewmenu.h"
#include "pathmodel.h"

#include <QClipboard>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMenu>
#include <QMetaEnum>
#include <QDebug>

PathTableView::PathTableView(QWidget *parent)
    : QTableView(parent)
{
    auto menu = new PathTableViewMenu(this);

    menu->addActionsToWidget(this);

    connect(menu, &PathTableViewMenu::requestRemoveItem, this, &PathTableView::removeSelectedRows);
    connect(menu, &PathTableViewMenu::requestCopyName, this, &PathTableView::copyName);
    connect(menu, &PathTableViewMenu::requestOpenPath, this, &PathTableView::openFile);
    connect(menu, &PathTableViewMenu::requestDeletePath, this, &PathTableView::deleteFile);
    connect(menu, &PathTableViewMenu::requestOpenMulti, this, &PathTableView::openBothFiles);
}

void PathTableView::setEnableToChangeItems(bool isEnable)
{
    setDragEnabled(isEnable);

    auto menu = findChild<PathTableViewMenu*>();

    Q_CHECK_PTR(menu);

    menu->setEnabledActions(PathTableViewMenu::Action::RemoveItem, isEnable);
}

void PathTableView::contextMenuEvent(QContextMenuEvent *event)
{
    if (!selectionModel()->hasSelection())
        return;

    auto menu = findChild<PathTableViewMenu*>();

    Q_CHECK_PTR(menu);

    menu->updateActions();
    menu->popup(viewport()->mapToGlobal(event->pos()));
}

void PathTableView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QTableView::selectionChanged(selected, deselected);

    QModelIndexList selectedRows = selectionModel()->selectedRows();
    qsizetype selectedCount = selectedRows.count();

    emit selectedCountChanged(selectedCount);

    QString statusText;
    QIcon statusIcon;
    QString stateText;
    QIcon stateIcon;

    if (selectedCount == 1) {
        QModelIndex &index = selectedRows.first();

        statusText = index.data(Qt::StatusTipRole).toString();
        statusIcon = index.data(Qt::DecorationRole).value<QIcon>();
        stateText = index.data(PathModel::StateTextRole).toString();
        stateIcon = index.data(PathModel::StateIconRole).value<QIcon>();
    }

    emit statusTextChanged(statusIcon, statusText);
    emit stateTextChanged(stateIcon, stateText);
}

void PathTableView::copyName()
{
    QString text = currentIndex().data().toString();

    QGuiApplication::clipboard()->setText(text);

    QString statusText = tr("Copied to clipboard : <b>%1</b>").arg(text);

    emit statusTextChanged(QIcon(), statusText);
}

void PathTableView::openFile()
{
    auto pathModel = qobject_cast<PathModel*>(model());

    Q_CHECK_PTR(pathModel);

    QString fullPath = pathModel->fullPath(currentIndex());

    if (!QFileInfo::exists(fullPath))
        return;

    QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
}

void PathTableView::deleteFile()
{
    const QModelIndex index = currentIndex();

    if (index.column() == int(PathModel::HSection::Path))
        return;

    auto pathModel = qobject_cast<PathModel*>(model());

    Q_CHECK_PTR(pathModel);

    QString fullPath = pathModel->fullPath(index);

    if (!QFileInfo::exists(fullPath))
        return;

    if (QFile::moveToTrash(fullPath) && index.column() == int(PathModel::HSection::OriginalName))
        pathModel->removeSpecifiedRows({index.row()});
}

void PathTableView::openBothFiles()
{
    auto pathModel = qobject_cast<PathModel*>(model());

    Q_CHECK_PTR(pathModel);

    QString fullPathOriginal = pathModel->fullPath(currentIndex().row(), PathModel::HSection::OriginalName);
    QString fullPathNew = pathModel->fullPath(currentIndex().row(), PathModel::HSection::NewName);

    if (!QFileInfo::exists(fullPathOriginal) || !QFileInfo::exists(fullPathNew))
        return;

    QDesktopServices::openUrl(QUrl::fromLocalFile(fullPathOriginal));
    QDesktopServices::openUrl(QUrl::fromLocalFile(fullPathNew));
}

void PathTableView::removeSelectedRows()
{
    auto pathModel = qobject_cast<PathModel *>(model());

    if (pathModel == nullptr)
        return;

    QList<int> rows;

    for (const QModelIndex &modelIndex: selectionModel()->selectedRows())
        rows << modelIndex.row();

    pathModel->removeSpecifiedRows(rows);
}
