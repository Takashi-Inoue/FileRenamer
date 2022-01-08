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

#include "logdata.h"

#include <QAbstractTableModel>
#include <QReadWriteLock>
#include <QString>
#include <QSharedPointer>

class ApplicationLog : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ApplicationLog)
public:
    static ApplicationLog &instance();

    // implementation for QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void clear();
    bool isEmpty() const;
    void log(QStringView log, QStringView groupName = QStringLiteral("General"));

    void writeFile() const;
    void writeSpecifiedLogs(QStringView groupName) const;

private:
    using QAbstractTableModel::QAbstractTableModel;

    QString logDir() const;
    void removeOldestLogFile() const;

    using SharedLogData = QSharedPointer<LogData>;

    mutable QReadWriteLock m_lock;
    QList<SharedLogData> m_applicationLogs;
};
