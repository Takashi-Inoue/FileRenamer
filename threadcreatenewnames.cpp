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

#include "threadcreatenewnames.h"

#include "path/pathroot.h"
#include "path/pathentity.h"
#include "path/pathentityinfo.h"
#include "stringbuilderonfile/builderchainonfile.h"

ThreadCreateNewNames::ThreadCreateNewNames(QWeakPointer<Path::PathRoot> pathRoot, QObject *parent)
    : QThread{parent},
      m_pathRoot{pathRoot}
{
    Q_ASSERT(m_pathRoot != nullptr);
}

void ThreadCreateNewNames::setStringBuilderOnFile(
        QSharedPointer<StringBuilderOnFile::BuilderChainOnFile> builderChain)
{
    QWriteLocker locker(&m_lock);

    if (m_builderChain != builderChain)
        m_builderChain = builderChain;
}

void ThreadCreateNewNames::stop()
{
    QWriteLocker locker(&m_lock);

//    qInfo() << QStringLiteral("Thread for creating new name got request to stop.");

    m_isStopRequested = true;
}

void ThreadCreateNewNames::run()
{
//    qInfo() << QStringLiteral("Start creating new name(s).");

    QReadLocker locker(&m_lock);

    if(m_builderChain == nullptr)
        return;

    locker.unlock();

    m_lock.lockForWrite();
    m_isStopRequested = false;
    m_lock.unlock();

    HashToCheckEntities hashToCheckNames;

    if (!createNewNames(hashToCheckNames))
        return;

    if (!checkNewNames(hashToCheckNames))
        return;

    if (m_builderChain->isEmpty())
        return;

//    qInfo() << QStringLiteral("Finished creating new name(s).");

    emit completed();
}

bool ThreadCreateNewNames::isStopRequested() const
{
    QReadLocker locker(&m_lock);

    return m_isStopRequested;
}

bool ThreadCreateNewNames::checkNewNames(HashToCheckEntities &hashToCheckNames)
{
//    qInfo() << QStringLiteral("Start checking new name(s).");

    bool isOk = true;

    for (auto itr = hashToCheckNames.begin(); itr != hashToCheckNames.end(); ++itr) {
        std::sort(itr->begin(), itr->end()
                , [](const EntityToIndex &lhs, const EntityToIndex &rhs)
        {
            return lhs.first->newName() < rhs.first->newName();
        });

        const qsizetype count = itr->size();

        if (count == 1)
            isOk &= itr->first().first->checkSelfNewName();

        for (qsizetype i = 0; i < count - 1; ++i) {
            const EntityToIndex &lhs = itr->at(i);
            const EntityToIndex &rhs = itr->at(i + 1);

            if (!lhs.first->checkForNewNameCollisions(rhs.first)) {
                isOk = false;
                emit newNameCollisionDetected({lhs.second, rhs.second});
            }

            if (isStopRequested())
                return false;
        }
    }

    if (isOk)
        emit newNameCollisionNotDetected();

//    qInfo() << QStringLiteral("Finished checking new name(s).");

    return true;
}

bool ThreadCreateNewNames::createNewNames(HashToCheckEntities &hashToCheckNames)
{
    QSharedPointer<Path::PathRoot> root = m_pathRoot.lock();

    for (int i = 0, count = int(root->entityCount()); i < count; ++i) {
        createOneNewName({root->entity(i), i}, hashToCheckNames);

        if (isStopRequested())
            return false;
    }

    return true;
}

void ThreadCreateNewNames::createOneNewName(EntityToIndex entityToIndex
                                          , HashToCheckEntities &hashToCheckNames)
{
    QSharedPointer<Path::PathEntity> &entity = entityToIndex.first;

    m_lock.lockForRead();

    m_builderChain->setFileInfo(QSharedPointer<Path::PathEntityInfo>::create(entity));
    entity->setNewName(m_builderChain->build());

    hashToCheckNames[quintptr(entity->parent().lock().get())] << entityToIndex;

    m_lock.unlock();

    emit newNameCreated(entityToIndex.second);
}
