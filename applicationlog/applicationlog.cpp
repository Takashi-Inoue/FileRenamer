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

#include "applicationlog.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QDebug>

namespace {
Q_GLOBAL_STATIC(QReadWriteLock, rwlock)
Q_GLOBAL_STATIC(ApplicationLog, appLog)

QString logDirPath()
{
    return QStringLiteral("%1/logs").arg(QApplication::applicationDirPath());
}

void removeOldestLogFile()
{
    QDir dir{logDirPath()};

    if (!dir.exists())
        return;

    QStringList fileNames = dir.entryList({QApplication::applicationName() + "*.log"},
                                           QDir::Files, QDir::Name);

    if (fileNames.size() < 10)
        return;

    QString oldestFile = dir.absoluteFilePath(fileNames.first());

    QFile().remove(oldestFile)
            ? qInfo() << QObject::tr("Removed oldest log file. [%1]").arg(oldestFile)
            : qInfo() << QObject::tr("Failed to remove oldest log file. [%1]").arg(oldestFile);
}

} // anonymous

ApplicationLog &ApplicationLog::instance()
{
    return *appLog;
}

int ApplicationLog::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    QReadLocker locker{rwlock};

    return int(m_applicationLogs.size());
}

int ApplicationLog::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant ApplicationLog::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant{};

    static const QHash<int, QString> hashData = {
        {0, tr("Time")},
        {1, tr("Category")},
        {2, tr("Content")},
    };

    return hashData[section];
}

QVariant ApplicationLog::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid())
        return QVariant{};

    QReadLocker locker{rwlock};

    const SharedLogData &logData = m_applicationLogs.at(index.row());

    if (index.column() == 0)
        return logData->time(QStringLiteral("hh:mm:ss.zzz"));
    else if (index.column() == 1)
        return logData->group();

    return logData->log();
}

void ApplicationLog::clear()
{
    QWriteLocker locker{rwlock};

    m_applicationLogs.clear();
}

bool ApplicationLog::isEmpty() const
{
    return m_applicationLogs.size() == 0;
}

void ApplicationLog::log(QStringView log, QStringView groupName)
{
    rwlock->lockForRead();
    int row = int(m_applicationLogs.size());
    rwlock->unlock();

    beginInsertRows(QModelIndex{}, row, row);

    rwlock->lockForWrite();
    m_applicationLogs.append(SharedLogData::create(groupName, log));
    rwlock->unlock();

    endInsertRows();
}

void ApplicationLog::writeFile() const
{
    QReadLocker locker{rwlock};

    if (m_applicationLogs.size() == 0)
        return;

    const QString logDirectory = logDirPath();

    if (!QDir{}.mkpath(logDirectory)) {
        qInfo() << tr("Failed to create log directory. %1").arg(logDirectory);
        return;
    }

    removeOldestLogFile();

    QString time = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh-mm-ss"));
    QString filePathName = QStringLiteral("%1/%2 %3.log")
                          .arg(logDirectory, QApplication::applicationName(), time);

    QFile file{filePathName};

    if (!file.open(QIODevice::WriteOnly)) {
        qInfo() << tr("Failed to write log to [%1]. Error: %2")
                  .arg(filePathName, file.errorString());
        return;
    }

    for (const SharedLogData &logData : m_applicationLogs)
        logData->write(file);

    qInfo() << tr("Wrote log to the file. [%1]").arg(filePathName);
}

void ApplicationLog::writeSpecifiedLogs(QStringView groupName) const
{
    QReadLocker locker{rwlock};

    if (m_applicationLogs.size() == 0)
        return;

    const QString logDirectory = logDirPath();

    if (!QDir().mkpath(logDirectory)) {
        qInfo() << tr("Failed to create log directory. %1").arg(logDirectory);
        return;
    }

    QString filePathName = QStringLiteral("%1/%2.log").arg(logDirectory, groupName);

    QFile file{filePathName};

    if (!file.open(QIODevice::WriteOnly)) {
        qInfo() << tr("Failed to write log to [%1]. Error: %2")
                  .arg(filePathName, file.errorString());
        return;
    }

    for (const SharedLogData &logData : m_applicationLogs) {
        if (logData->group() == groupName)
            logData->write(file);
    }

    qInfo() << tr("Wrote log to the file. [%1]").arg(filePathName);
}
