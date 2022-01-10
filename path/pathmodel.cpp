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

#include "pathmodel.h"
#include "pathroot.h"
#include "pathentity.h"
#include "threadcreatenewnames.h"
#include "threadrename.h"
#include "threadundorenaming.h"

#include <QIODevice>
#include <QIcon>
#include <QMimeData>

namespace {
constexpr char mimeTypeModelDataList[] = "application/x-qabstractitemmodeldatalist";
} // anonymous

PathModel::PathModel(QObject *parent)
    : QAbstractTableModel(parent),
      m_dataRoot(QSharedPointer<Path::PathRoot>::create()),
      m_threadCreateNewNames(new ThreadCreateNewNames{m_dataRoot, this}),
      m_threadRename(new ThreadRename{m_dataRoot, this}),
      m_threadUndoRenaming(new ThreadUndoRenaming{m_dataRoot, this})
{
    connect(m_threadCreateNewNames, &ThreadCreateNewNames::newNameCreated
          , this, &PathModel::onNewNameCreated);

    connect(m_threadCreateNewNames, &ThreadCreateNewNames::newNameCollisionDetected
          , this, &PathModel::onNewNameCollisionDetected);

    connect(m_threadCreateNewNames, &ThreadCreateNewNames::completed
          , this, &PathModel::onCreateNameCompleted);

    connect(m_threadRename, &ThreadRename::renamed, this, &PathModel::onNewNameStateChanged);
    connect(m_threadRename, &ThreadRename::stopped, this, &PathModel::renameStopped);
    connect(m_threadRename, &ThreadRename::completed, this, &PathModel::renameFinished);

    connect(m_threadUndoRenaming, &ThreadUndoRenaming::renamed, this, &PathModel::onNewNameStateChanged);
    connect(m_threadUndoRenaming, &ThreadUndoRenaming::stopped, this, &PathModel::renameStopped);
    connect(m_threadUndoRenaming, &ThreadUndoRenaming::completed, this, &PathModel::readyToRename);
}

QVariant PathModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return QVariant();

    if (role == Qt::DecorationRole) {
        static const QMap<HSection, QVariant> m_iconMap = {
            {HSection::OriginalName, QIcon(QStringLiteral(":/res/images/rename_old.svg"))},
            {HSection::NewName,      QIcon(QStringLiteral(":/res/images/rename_new.svg"))},
            {HSection::Path,         QIcon(QStringLiteral(":/res/icons/folder-3.ico"))},
        };

        return m_iconMap[HSection(section)];
    }

    if (role == Qt::DisplayRole) {
        static const QMap<HSection, QString> m_textMap = {
            {HSection::OriginalName, QStringLiteral("Original name")},
            {HSection::NewName,      QStringLiteral("New name")},
            {HSection::Path,         QStringLiteral("Path")},
        };

        return m_textMap[HSection(section)];
    }

    return QVariant();
}

int PathModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return int(m_dataRoot->entityCount());
}

int PathModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant PathModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QSharedPointer<Path::PathEntity> entity = m_dataRoot->entity(index.row());

    HSection hSection = HSection(index.column());

    if (role == Qt::DisplayRole) {
        if (hSection == HSection::OriginalName)
            return entity->name();

        if (hSection == HSection::NewName)
            return entity->newName();

        if (hSection == HSection::Path)
            return entity->parentPath();
    }

    if (role == Qt::DecorationRole) {
        if (hSection == HSection::OriginalName)
            return entity->typeIcon();

        if (hSection == HSection::NewName)
            return entity->stateIcon();
    }

    if (role == Qt::StatusTipRole)
        return entity->statusText();

    if (role == StateTextRole)
        return entity->stateText();

    if (role == StateIconRole)
        return entity->stateIcon();

    return QVariant();
}

void PathModel::sort(int column, Qt::SortOrder order)
{
    if (m_dataRoot->isEmpty())
        return;

    stopThreadToCreateNames();

    beginResetModel();

    column == int(HSection::OriginalName) ? m_dataRoot->sortByEntityName(order)
                                          : m_dataRoot->sortByParentDir(order);

    endResetModel();

    emit internalDataChanged();
}

// drag & drop
Qt::ItemFlags PathModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;

    return Qt::ItemIsDropEnabled | defaultFlags;
}

Qt::DropActions PathModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool PathModel::canDropMimeData(const QMimeData *data, Qt::DropAction /*action*/
                              , int /*row*/, int /*column*/, const QModelIndex &parent) const
{
    const int targetRow = parent.row();

    if ((targetRow == -1) || !data->hasFormat(mimeTypeModelDataList))
        return false;

    const QWeakPointer<Path::ParentDir> targetParent = m_dataRoot->entity(targetRow)->parent();

    for (int sourceRow : rowsFromMimeData(data)) {
        if (m_dataRoot->entity(sourceRow)->parent() != targetParent)
            return false;
    }

    return true;
}

bool PathModel::dropMimeData(const QMimeData *data, Qt::DropAction /*action*/
                           , int /*row*/, int /*column*/, const QModelIndex &parent)
{
    stopThreadToCreateNames();

    beginResetModel();

    m_dataRoot->move(rowsFromMimeData(data), parent.row());

    endResetModel();

    emit internalDataChanged();
    emit sortingBroken();

    return true;
}

void PathModel::addPaths(QList<PathModel::ParentChildrenPair> dirs
                       , QList<PathModel::ParentChildrenPair> files)
{
    if (dirs.isEmpty() && files.isEmpty())
        return;

    stopThreadToCreateNames();

    beginResetModel();

    m_dataRoot->addDirectories(dirs);
    m_dataRoot->addFiles(files);

    endResetModel();

    emit itemCountChanged(rowCount());
    emit internalDataChanged();
}

void PathModel::removeSpecifiedRows(QList<int> rows)
{
    stopThreadToCreateNames();

    beginResetModel();
    m_dataRoot->removeSpecifiedRows(rows);
    endResetModel();

    emit itemCountChanged(rowCount());

    m_dataRoot->entityCount() != 0 ? emit internalDataChanged()
                                   : emit itemCleared();
}

bool PathModel::isDir(int row) const
{
    return m_dataRoot->entity(row)->isDir();
}

QString PathModel::fullPath(const QModelIndex &index) const
{
    return fullPath(index.row(), HSection(index.column()));
}

QString PathModel::fullPath(int row, HSection section) const
{
    QSharedPointer<Path::PathEntity> entity = m_dataRoot->entity(row);

    if (section == HSection::OriginalName)
        return entity->fullPath();

    if (section == HSection::NewName)
        return QStringLiteral("%1%2").arg(entity->parentPath(), entity->newName());

    return entity->parentPath();
}

QString PathModel::name(const QModelIndex &index) const
{
    return name(index.row(), HSection(index.column()));
}

QString PathModel::name(int row, HSection section) const
{
    QSharedPointer<Path::PathEntity> entity = m_dataRoot->entity(row);

    if (section == HSection::OriginalName)
        return entity->name();

    if (section == HSection::NewName)
        return entity->newName();

    if (section == HSection::Path)
        return entity->parentPath();

    return QString();
}

QString PathModel::originalName(int row) const
{
    QSharedPointer<Path::PathEntity> entity = m_dataRoot->entity(row);

    return entity->name();
}

QString PathModel::newName(int row) const
{
    QSharedPointer<Path::PathEntity> entity = m_dataRoot->entity(row);

    return entity->newName();
}

void PathModel::clear()
{
    stopThreadToCreateNames();

    beginResetModel();
    m_dataRoot->clear();
    endResetModel();

    emit itemCleared();
}

// Start/Stop threads
void PathModel::startCreateNewNames(QSharedPointer<StringBuilder::OnFile::BuilderChainOnFile> builderChain)
{
    if (m_dataRoot->isEmpty())
        return;

    m_threadCreateNewNames->stop();
    m_threadCreateNewNames->wait();
    m_threadCreateNewNames->setStringBuilderOnFile(builderChain);
    m_threadCreateNewNames->start();
}

void PathModel::startRename()
{
    emit renameStarted();
    m_threadRename->start();
}

void PathModel::stopRename()
{
    if (m_threadRename->isRunning()) {
        m_threadRename->stop();
        m_threadRename->wait();
    }

    if (m_threadUndoRenaming->isRunning()) {
        m_threadUndoRenaming->stop();
        m_threadUndoRenaming->wait();
    }
}

void PathModel::undoRename()
{
    emit undoStarted();
    m_threadUndoRenaming->start();
}

// private slots //
void PathModel::onCreateNameCompleted()
{
    if (m_dataRoot->isEmpty())
        return;

    QModelIndex tl = index(0, int(HSection::NewName));
    QModelIndex br = index(int(m_dataRoot->entityCount() - 1), int(HSection::NewName));

    emit dataChanged(tl, br, {Qt::DecorationRole});
    emit readyToRename();
}

void PathModel::onNewNameCollisionDetected(QPair<int, int> indices)
{
    QModelIndex lIndex = index(indices.first,  int(HSection::NewName));
    QModelIndex rIndex = index(indices.second, int(HSection::NewName));

    emit dataChanged(lIndex, lIndex, {Qt::DecorationRole});
    emit dataChanged(rIndex, rIndex, {Qt::DecorationRole});
}

void PathModel::onNewNameCreated(int row)
{
    QModelIndex modelIndex = index(row, int(HSection::NewName));

    emit dataChanged(modelIndex, modelIndex, {Qt::DisplayRole});
}

void PathModel::onNewNameStateChanged(int row)
{
    QModelIndex modelIndex = index(row, int(HSection::NewName));

    emit dataChanged(modelIndex, modelIndex, {Qt::DecorationRole});
}

// drag & drop
QList<int> PathModel::rowsFromMimeData(const QMimeData *data) const
{
    if (!data->hasFormat(mimeTypeModelDataList))
        return QList<int>();

    QList<int> rows;
    QByteArray encodedData = data->data(mimeTypeModelDataList);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        int row;
        int col;
        QMap<int, QVariant> roleDataMap;

        stream >> row >> col >> roleDataMap;

        if (col == 0)
            rows << row;
    }

    return rows;
}

// private //
void PathModel::stopThreadToCreateNames()
{
    m_threadCreateNewNames->stop();
    m_threadCreateNewNames->wait();
}
