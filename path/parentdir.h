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

#include "usingpathentity.h"

namespace Path {

class ParentDir
{
public:
    ParentDir(QStringView path);

    // Add / Remove entity;
    void addEntity(const SharedEntity &entity);
    void clear();
    void removeEntity(WeakEntity entity);
    void replaceEntities(const EntityList &entities);

    const EntityList &allEntities() const;
    SharedEntity entity(int index) const;
    qsizetype entityCount() const;
    QString path() const;
    void sort(QCollator &collator, Qt::SortOrder order);

private:
    const QString m_path;
    EntityList m_children;
};

} // Path
