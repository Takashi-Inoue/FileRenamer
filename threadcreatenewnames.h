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

namespace StringBuilderOnFile {
class BuilderChainOnFile;
}

class ThreadCreateNewNames : public QThread
{
    Q_OBJECT
public:
    ThreadCreateNewNames(QWeakPointer<Path::PathRoot> pathRoot, QObject *parent = nullptr);

    void setStringBuilderOnFile(QSharedPointer<StringBuilderOnFile::BuilderChainOnFile> builderChain);
    void stop();

signals:
    void completed();
    void newNameCreated(int index);
    void newNameCollisionDetected(QPair<int, int> indices);
    void newNameCollisionNotDetected();

protected:
    void run() override;

    bool isStopRequested() const;

private:
    using EntityToIndex = QPair<QSharedPointer<Path::PathEntity>, int>;
    using HashToCheckEntities = QHash<quintptr, QList<EntityToIndex>>;

    bool checkNewNames(HashToCheckEntities &hashToCheckNames);
    bool createNewNames(HashToCheckEntities &hashToCheckNames);
    void createOneNewName(EntityToIndex entityToIndex, HashToCheckEntities &hashToCheckNames);

    mutable QReadWriteLock m_lock;

    bool m_isStopRequested = false;
    QWeakPointer<Path::PathRoot> m_pathRoot;
    QSharedPointer<StringBuilderOnFile::BuilderChainOnFile> m_builderChain;
};
