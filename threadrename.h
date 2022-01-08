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

#include <QThread>

#include <QReadWriteLock>
#include <QWeakPointer>

namespace Path {
class PathRoot;
class PathEntity;
}

class ThreadRename : public QThread
{
    Q_OBJECT
public:
    ThreadRename(QWeakPointer<Path::PathRoot> pathRoot, QObject *parent = nullptr);

    void stop();

signals:
    void renamed(int index);
    void completed();
    void stopped();

protected:
    void run() override;

    bool isStopRequested() const;

private:
    using EntityPtr = QSharedPointer<Path::PathEntity>;
    using EntityToIndex = QPair<EntityPtr, int>;

    void renameEntities(const QList<EntityToIndex> &entityToIndexList);

    mutable QReadWriteLock m_lock;

    bool m_isStopRequested = false;
    QWeakPointer<Path::PathRoot> m_pathRoot;
};
