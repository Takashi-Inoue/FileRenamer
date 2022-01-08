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

ApplicationLog &ApplicationLog::instance()
{
    static ApplicationLog appLogInstance;
    return appLogInstance;
}

int ApplicationLog::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    QReadLocker locker{&m_lock};

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
        return QVariant();

    static const QHash<int, QString> hashData = {
        {0, QStringLiteral("Time")},
        {1, QStringLiteral("Category")},
        {2, QStringLiteral("Content")},
    };

    return hashData[section];
}

QVariant ApplicationLog::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid())
        return QVariant{};

    QReadLocker locker{&m_lock};

    const SharedLogData &logData = m_applicationLogs.at(index.row());

    if (index.column() == 0)
        return logData->time(QStringLiteral("hh:mm:ss.zzz"));
    else if (index.column() == 1)
        return logData->group();

    return logData->log();
}

void ApplicationLog::clear()
{
    QWriteLocker locker{&m_lock};

    m_applicationLogs.clear();
}

bool ApplicationLog::isEmpty() const
{
    return m_applicationLogs.size() == 0;
}

void ApplicationLog::log(QStringView log, QStringView groupName)
{
    m_lock.lockForRead();
    int row = int(m_applicationLogs.size());
    m_lock.unlock();

    beginInsertRows(QModelIndex{}, row, row);

    m_lock.lockForWrite();
    m_applicationLogs.append(SharedLogData::create(groupName, log));
    m_lock.unlock();

    endInsertRows();
}

void ApplicationLog::writeFile() const
{
    QReadLocker locker{&m_lock};

    if (m_applicationLogs.size() == 0)
        return;

    const QString logDirectory = logDir();

    if (!QDir().mkpath(logDirectory)) {
        qInfo() << QStringLiteral("Failed to create log directory. %1").arg(logDirectory);
        return;
    }

    removeOldestLogFile();

    QString time = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh-mm-ss"));
    QString filePathName = QStringLiteral("%1/%2 %3.log")
                          .arg(logDirectory, QApplication::applicationName(), time);

    QFile file(filePathName);

    if (!file.open(QIODevice::WriteOnly)) {
        qInfo() << QStringLiteral("Failed to write log to [%1]. Error: %2")
                  .arg(filePathName, file.errorString());
        return;
    }

    for (const SharedLogData &logData : m_applicationLogs)
        logData->write(file);

    qInfo() << QStringLiteral("Wrote log to the file. [%1]").arg(filePathName);
}

void ApplicationLog::writeSpecifiedLogs(QStringView groupName) const
{
    QReadLocker locker(&m_lock);

    if (m_applicationLogs.size() == 0)
        return;

    const QString logDirectory = logDir();

    if (!QDir().mkpath(logDirectory)) {
        qInfo() << QStringLiteral("Failed to create log directory. %1").arg(logDirectory);
        return;
    }

    QString filePathName = QStringLiteral("%1/%2.log").arg(logDirectory, groupName);

    QFile file(filePathName);

    if (!file.open(QIODevice::WriteOnly)) {
        qInfo() << QStringLiteral("Failed to write log to [%1]. Error: %2")
                  .arg(filePathName, file.errorString());
        return;
    }

    for (const SharedLogData &logData : m_applicationLogs) {
        if (logData->group() == groupName)
            logData->write(file);
    }

    qInfo() << QStringLiteral("Wrote log to the file. [%1]").arg(filePathName);
}

QString ApplicationLog::logDir() const
{
    return QStringLiteral("%1/logs").arg(QApplication::applicationDirPath());
}

void ApplicationLog::removeOldestLogFile() const
{
    QDir dir{logDir()};

    if (!dir.exists())
        return;

    QStringList fileNames = dir.entryList({QApplication::applicationName() + "*.log"}
                                         , QDir::Files, QDir::Name);

    if (fileNames.size() < 10)
        return;

    QString oldestFile = dir.absoluteFilePath(fileNames.first());

    QFile().remove(oldestFile)
            ? qInfo() << QStringLiteral("Removed oldest log file. [%1]").arg(oldestFile)
            : qInfo() << QStringLiteral("Failed to remove oldest log file. [%1]").arg(oldestFile);
}
