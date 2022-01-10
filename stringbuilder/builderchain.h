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

#include <QObject>
#include <QSharedPointer>
#include <QList>

namespace StringBuilder {

class AbstractStringBuilder;

class BuilderChain : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    virtual void addBuilder(QSharedPointer<AbstractStringBuilder> builder);
    virtual QString build();

    bool isEmpty() const;
    void reset();

protected:
    QList<QSharedPointer<AbstractStringBuilder>> m_builders;
};

} // StringBuilder
