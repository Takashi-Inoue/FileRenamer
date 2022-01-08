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

#include <QReadWriteLock>
#include <QSharedPointer>

namespace Path {

class PathEntity;
class ParentDir;

class PathRoot final
{
    Q_DISABLE_COPY_MOVE(PathRoot)
public:
    using ParentChildrenPair = QPair<QString, QStringList>;

    PathRoot() = default;
    ~PathRoot() = default;

    // Add / Remove / Move Data
    void addDirectories(QList<ParentChildrenPair> dirs);
    void addFiles(QList<ParentChildrenPair> files);
    void clear();
    void move(QList<int> sourceRows, int targetRow);
    void remove(int index, int count = 1);
    void removeSpecifiedRows(QList<int> rows);

    QSharedPointer<ParentDir> dir(QStringView path) const;
    QSharedPointer<PathEntity> entity(qsizetype index) const;
    qsizetype entityCount() const;
    bool isEmpty() const;
    void sortByEntityName(Qt::SortOrder order);
    void sortByParentDir(Qt::SortOrder order);

private:
    enum class EntityType {Dirs, Files};

    void addPaths(const QList<ParentChildrenPair> &paths, EntityType entityType);

    mutable QReadWriteLock m_lock;

    QList<QSharedPointer<PathEntity>> m_entities;
    QList<QSharedPointer<ParentDir>> m_dirs;
};

} // Path
