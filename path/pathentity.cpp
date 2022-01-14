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

#include "pathentity.h"

#include "applicationlog/applicationlog.h"
#include "parentdir.h"

#include <QDir>
#include <QFileIconProvider>

namespace Path {

PathEntity::PathEntity(QWeakPointer<ParentDir> parent, QStringView name, bool isDir)
    : m_isDir(isDir),
      m_parent(parent),
      m_name(name.toString())
{
    static QFileIconProvider iconProvider; // static for one-time creation.

    Q_ASSERT(parent != nullptr);
    Q_ASSERT(!name.isEmpty());

    m_fileIcon = iconProvider.icon(QFileInfo(fullPath()));
}

bool PathEntity::isDir() const
{
    return m_isDir;
}

QString PathEntity::fullPath() const
{
    QReadLocker locker(&m_lock);

    return QStringLiteral("%1%2").arg(m_parent.lock()->path(), m_name);
}

QString PathEntity::parentPath() const
{
    return m_parent.lock()->path();
}

QString PathEntity::name() const
{
    QReadLocker locker(&m_lock);

    return m_name;
}

QString PathEntity::newName() const
{
    QReadLocker locker(&m_lock);

    return m_newName;
}

QWeakPointer<ParentDir> PathEntity::parent() const
{
    return m_parent;
}

QString PathEntity::hashHex(QCryptographicHash::Algorithm algorithm) const
{
    return m_fileHashs[algorithm];
}

QString PathEntity::imageHash() const
{
    return m_imageHash;
}

void PathEntity::setHashHex(QCryptographicHash::Algorithm algorithm, QStringView hashHex)
{
    m_fileHashs[algorithm] = hashHex.toString();
}

void PathEntity::setImageHash(QStringView imageHash)
{
    m_imageHash = imageHash.toString();
}

void PathEntity::setNewName(QStringView newName)
{
    QWriteLocker locker(&m_lock);

    m_newName = m_isDir ? newName.toString()
                        : QStringLiteral("%1%2").arg(newName, m_name.mid(m_name.indexOf('.')));

    m_state = State::Initial;
}

bool PathEntity::checkForNewNameCollisions(QSharedPointer<PathEntity> other)
{
    if (isDir() && newName().isEmpty()) {
        if (state() != State::Initial)
            setState(State::Initial);

        return false;
    }

    if (newName() == other->newName()) {
        setState(State::SameNewName);
        other->setState(State::SameNewName);

        return false;
    }

    if (state() == State::Initial)
        setState(State::Ready);

    if (other->state() == State::Initial)
        other->setState(State::Ready);

    return true;
}

bool PathEntity::checkSelfNewName()
{
    const QString newname = newName();

    if (newname.isEmpty()/* || newname == name()*/) {
        if (state() != State::Initial)
            setState(State::Initial);

        return false;
    }

    setState(State::Ready);

    return true;
}

QIcon PathEntity::stateIcon() const
{
    static const QHash<int, QIcon> icons = {
        {int(State::Initial),     QIcon(QStringLiteral(":/res/images/circlegray.svg"))},
        {int(State::Ready),       QIcon(QStringLiteral(":/res/images/circlegreen.svg"))},
        {int(State::SameNewName), QIcon(QStringLiteral(":/res/images/collision.svg"))},
        {int(State::Success),     QIcon(QStringLiteral(":/res/images/success.svg"))},
        {int(State::Failure),     QIcon(QStringLiteral(":/res/images/failure.svg"))},
    };

    QReadLocker locker(&m_lock);

    return icons[int(m_state)];
}

QString PathEntity::stateText() const
{
    static const QHash<int, QString> texts = {
        {int(State::Initial),     QObject::tr("Waiting")},
        {int(State::Ready),       QObject::tr("Ready")},
        {int(State::SameNewName), QObject::tr("Same new name")},
        {int(State::Success),     QObject::tr("Succeeded")},
        {int(State::Failure),     QObject::tr("Failed")},
    };

    QReadLocker locker(&m_lock);

    return texts[int(m_state)];
}

QIcon PathEntity::typeIcon() const
{
    return m_fileIcon;
}

QString PathEntity::statusText() const
{
    switch (state()) {
    case State::Initial:
        return fullPath();

    case State::Ready:
        return QStringLiteral("%1 -> %2").arg(fullPath(), newName());

    case State::SameNewName:
        return QObject::tr("New name <b>%1</b> is duplicated.").arg(newName());

    case State::Success:
        return QObject::tr("New path: %1%2").arg(parentPath(), newName());

    case State::Failure:
        if (m_errorCode == ErrorCode::AlreadyExist)
            return QObject::tr("<b>%1%2</b> already exeists.").arg(parentPath(), newName());

        if (m_errorCode == ErrorCode::SourceNotFound)
            return QObject::tr("<b>%1</b> does not exeists.").arg(fullPath());

        if (m_errorCode == ErrorCode::Unknown)
            return QObject::tr("Unknown error occered. Please confirm that file is not opened.");
    }

    return QString();
}

bool PathEntity::rename()
{
    if (state() == State::Success)
        return true;

    if (state() != State::Ready)
        return false;

    m_lock.lockForRead();

    QDir dir(m_parent.lock()->path());

    bool isOk = dir.rename(m_name, m_newName);

    QString result = isOk ? QObject::tr("SUCCEEDED")
                          : QObject::tr("---FAILED");

    QString log = QStringLiteral("%1 - %2/%3 -> %4").arg(result, dir.absolutePath(), m_name, m_newName);

    ApplicationLog::instance().log(log, QStringLiteral("Rename"));

    m_lock.unlock();

    if (isOk) {
        setState(State::Success);
    } else {
        setState(State::Failure);
        findErrorCause();
    }

    return isOk;
}

bool PathEntity::undoRename()
{
    if (state() == State::Failure) {
        setState(State::Ready);
        return true;
    }

    if (state() != State::Success)
        return false;

    m_lock.lockForRead();

    QDir dir(m_parent.lock()->path());

    bool isOk = dir.rename(m_newName, m_name);

    QString result = isOk ? QObject::tr("SUCCEEDED")
                          : QObject::tr("---FAILED");

    QString log = QStringLiteral("%1 - %2/%3 -> %4").arg(result, dir.absolutePath(), m_newName, m_name);

    ApplicationLog::instance().log(log, QStringLiteral("Undo"));

    m_lock.unlock();

    isOk ? setState(State::Ready)
         : setState(State::Success);

    return isOk;
}

void PathEntity::setState(PathEntity::State state)
{
    QWriteLocker locker(&m_lock);

    m_state = state;
}

PathEntity::State PathEntity::state() const
{
    QReadLocker locker(&m_lock);

    return m_state;
}

void PathEntity::findErrorCause()
{
    Q_ASSERT(state() == State::Failure);

    ErrorCode errorCode = ErrorCode::Unknown;

    if (!QFileInfo::exists(fullPath()))
        errorCode = ErrorCode::SourceNotFound;
    else if (QFileInfo::exists(QStringLiteral("%1%2").arg(parentPath(), newName())))
        errorCode = ErrorCode::AlreadyExist;

    QWriteLocker locker(&m_lock);

    m_errorCode = errorCode;
}

} // Path

