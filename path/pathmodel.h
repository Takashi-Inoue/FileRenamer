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

#pragma once

#include <QAbstractTableModel>
#include <QSharedPointer>

namespace Path {
class PathRoot;
}

namespace StringBuilder {
namespace OnFile {
class BuilderChainOnFile;
} // OnFile
} // StringBuilder

class ThreadCreateNewNames;
class ThreadRename;
class ThreadUndoRenaming;

class PathModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    using ParentChildrenPair = QPair<QString, QStringList>;

    explicit PathModel(QObject *parent = nullptr);

    enum Role {
        StateTextRole = Qt::UserRole + 1, StateIconRole
    };

    enum class HSection : int {
        OriginalName, NewName, Path
    };

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation
                      , int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    // drag & drop
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action
                       , int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action
                    , int row, int column, const QModelIndex &parent) override;

    // Add/Remove data:
    void addPaths(QList<ParentChildrenPair> dirs, QList<ParentChildrenPair> files);
    void removeSpecifiedRows(QList<int> rows);

    bool isDir(int row) const;
    QString fullPath(const QModelIndex &index) const;
    QString fullPath(int row, HSection section) const;
    QString name(const QModelIndex &index) const;
    QString name(int row, HSection section) const;
    QString originalName(int row) const;
    QString newName(int row) const;

public slots:
    void clear();
    // Start/Stop threads
    void startCreateNewNames(QSharedPointer<StringBuilder::OnFile::BuilderChainOnFile> builderChain);
    void startRename();
    void stopRename();
    void undoRename();

signals:
    void internalDataChanged();
    void itemCleared();
    void itemCountChanged(int);
    void readyToRename();
    void renameStarted();
    void renameStopped();
    void renameFinished();
    void undoStarted();
    void sortingBroken();

private slots:
    void onCreateNameCompleted();
    void onNewNameCollisionDetected(QPair<int, int> indices);
    void onNewNameCreated(int row);
    void onNewNameStateChanged(int row);

private:
    void stopThreadToCreateNames();

    QSharedPointer<Path::PathRoot> m_dataRoot;
    ThreadCreateNewNames *m_threadCreateNewNames;
    ThreadRename *m_threadRename;
    ThreadUndoRenaming *m_threadUndoRenaming;
};
