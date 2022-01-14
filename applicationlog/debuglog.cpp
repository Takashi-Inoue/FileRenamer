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

#include "debuglog.h"
#include "logdata.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

void DebugLog::init()
{
    qInstallMessageHandler(debugMessageHandler);
}

void DebugLog::writeFile()
{
    if (m_debugLogs.size() == 0)
        return;

    QFile file{QStringLiteral("%1/debug.log").arg(QApplication::applicationDirPath())};

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << QObject::tr("failed to write log to %1 : %2")
                    .arg(file.fileName(), file.errorString());
        return;
    }

    for (const SharedLogData &logData : m_debugLogs)
        logData->write(file);
}

void DebugLog::debugMessageHandler(QtMsgType type, const QMessageLogContext &context,
                                   const QString &msg)
{
    LogData *data = nullptr;

    switch (type) {
    case QtDebugMsg:
        data = new LogData(QStringLiteral("Debug"), QString("%1 (%2)").arg(msg, context.function));
        break;

    case QtInfoMsg:
        data = new LogData(QStringLiteral("Info"), msg);
        break;

    case QtWarningMsg:
        data = new LogData(QStringLiteral("Warning"), QString("%1 (%2:%4, %3)")
                          .arg(msg, context.file, context.function).arg(context.line));
        break;

    case QtCriticalMsg:
        data = new LogData(QStringLiteral("Critical"), QString("%1 (%2:%4, %3)")
                          .arg(msg, context.file, context.function).arg(context.line));
        break;

    case QtFatalMsg:
        outputMessage(QString("Fatal: %1 (%2:%4, %3)")
                      .arg(msg, context.file, context.function).arg(context.line));
        abort();
    }

    outputMessage(QString("[%1] %2").arg(data->group(), data->log()));

    m_debugLogs.emplace_back(data);
}

QString DebugLog::outputMessage(const QString &msg)
{
#ifdef Q_OS_WIN
    #ifdef UNICODE
        OutputDebugString((msg + "\r\n").toStdWString().c_str());
    #else
        OutputDebugString((msg + "\r\n").toStdString().c_str());
    #endif
#else
    fprintf(stderr, qPrintable(msg + "\n"));
#endif

    return msg;
}
