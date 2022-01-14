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

#include "threadundorenaming.h"

#include "path/pathroot.h"
#include "path/pathentity.h"

#include <QDebug>

ThreadUndoRenaming::ThreadUndoRenaming(QWeakPointer<Path::PathRoot> pathRoot, QObject *parent)
    : QThread{parent},
      m_pathRoot{pathRoot}
{
    Q_ASSERT(m_pathRoot != nullptr);
}

void ThreadUndoRenaming::stop()
{
    QWriteLocker locker(&m_lock);

    qInfo() << tr("Thread for undoing rename got request to stop.");

    m_isStopRequested = true;
}

void ThreadUndoRenaming::run()
{
    qInfo() << tr("Start undoing rename.");

    m_lock.lockForWrite();
    m_isStopRequested = false;
    m_lock.unlock();

    QList<EntityToIndex> dirs;
    QList<EntityToIndex> files;

    QSharedPointer<Path::PathRoot> root = m_pathRoot.lock();

    for (qsizetype i = 0, count = root->entityCount(); i < count; ++i) {
        QSharedPointer<Path::PathEntity> entity = root->entity(i);

        entity->isDir() ? dirs  << EntityToIndex(entity, i)
                        : files << EntityToIndex(entity, i);
    }

    std::sort(dirs.begin(), dirs.end(), [](const EntityToIndex &lhs, const EntityToIndex &rhs) {
        return lhs.first->fullPath().count('/') < rhs.first->fullPath().count('/');
    });

    renameEntities(dirs);

    if (isStopRequested()) {
        emit stopped();
        return;
    }

    renameEntities(files);

    if (isStopRequested()) {
        emit stopped();
        return;
    }

    qInfo() << tr("Finished undoing rename.");

    emit completed();
}

bool ThreadUndoRenaming::isStopRequested() const
{
    QReadLocker locker(&m_lock);

    return m_isStopRequested;
}

void ThreadUndoRenaming::renameEntities(const QList<EntityToIndex> &entityToIndexList)
{
    for (const EntityToIndex &entityToIndex : entityToIndexList) {
        entityToIndex.first->undoRename();

        emit renamed(entityToIndex.second);

        if (isStopRequested())
            return;
    }
}
