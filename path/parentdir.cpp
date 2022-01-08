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

#include "parentdir.h"
#include "pathentity.h"

#include <QCollator>

namespace Path {

ParentDir::ParentDir(QStringView path)
    : m_path(path.toString())
{
    Q_ASSERT(!path.isEmpty());
}

void ParentDir::addEntity(const SharedEntity &entity)
{
    QWriteLocker locker(&m_lock);

    m_children << entity;
}

void ParentDir::clear()
{
    QWriteLocker locker(&m_lock);

    m_children.clear();
}

void ParentDir::removeEntity(WeakEntity entity)
{
    QWriteLocker locker(&m_lock);

    m_children.removeOne(entity);
}

void ParentDir::replaceEntities(const EntityList &entities)
{
    QWriteLocker locker(&m_lock);

    Q_ASSERT(m_children.size() == entities.size());

    m_children = entities;
}

const EntityList &ParentDir::allEntities() const
{
    return m_children;
}

SharedEntity ParentDir::entity(int index) const
{
    Q_ASSERT(index < m_children.size());

    return m_children[index];
}

qsizetype ParentDir::entityCount() const
{
    return m_children.size();
}

QString ParentDir::path() const
{
    return m_path;
}

void ParentDir::sort(QCollator &collator, Qt::SortOrder order)
{
    QWriteLocker locker(&m_lock);

    using SharedEntity = QSharedPointer<PathEntity>;

    std::sort(m_children.begin(), m_children.end(),
              [&](const SharedEntity &lhs, const SharedEntity &rhs) {
        return order == Qt::AscendingOrder ? collator.compare(lhs->name(), rhs->name()) < 0
                                           : collator.compare(lhs->name(), rhs->name()) > 0;
    });
}

} // namespace Path
