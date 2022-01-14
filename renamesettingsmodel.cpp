/*
 * Copyright YEAR Takashi Inoue
 *
 * This file is part of APPNAME.
 *
 * APPNAME is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * APPNAME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with APPNAME.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "renamesettingsmodel.h"

#include "application.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QDebug>

namespace {
constexpr char lastSettingsFileName[] = "Last used.ini";

QString completeIniName(QStringView baseName){
    if (baseName.right(4) == QStringLiteral(".ini"))
        return baseName.toString();

    return QStringLiteral("%1.ini").arg(baseName);
}

} // anonymous

//--------------------------------------------------------------------------------------------------

RenameSettingsModel::RenameSettingsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    load();
}

int RenameSettingsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_iniBaseNames.size();
}

int RenameSettingsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 1;
}

QVariant RenameSettingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return m_iniBaseNames.at(index.row());

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    return QVariant();
}

QVariant RenameSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || section != 0)
        return QVariant{};

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    if (role == Qt::DisplayRole)
        return tr("Rename Settings");

    return QVariant{};
}

bool RenameSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return false;

    if (data(index, role) != value) {
        QDir settingsDir{Application::settingsDirPath()};

        QString &&oldName = completeIniName(m_iniBaseNames.at(index.row()));
        QString &&newName = completeIniName(value.toString());

        if (!settingsDir.rename(oldName, newName))
            return false;

        m_iniBaseNames[index.row()] = value.toString();

        emit dataChanged(index, index, {role});

        return true;
    }

    return false;
}

Qt::ItemFlags RenameSettingsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

bool RenameSettingsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    for (int i = 0; i < count; ++i) {
        QString &&iniBaseName = m_iniBaseNames.takeAt(row + i);
        QString &&iniPath = Application::settingsIniPath(completeIniName(iniBaseName));

        if (!QFile::moveToTrash(iniPath))
            qWarning() << tr("Failed to remove a file. [%1]").arg(iniPath);
    }

    endRemoveRows();

    return true;
}

bool RenameSettingsModel::existsLastUsedSettings() const
{
    return (m_iniBaseNames.last() == QString{lastSettingsFileName}.chopped(4));
}

int RenameSettingsModel::insertNewSettings(QStringView settingsName)
{
    auto itr = std::upper_bound(++m_iniBaseNames.cbegin(), --m_iniBaseNames.cend(), settingsName);

    m_iniBaseNames.insert(itr, settingsName.toString());

    return std::distance(m_iniBaseNames.cbegin(), itr);
}

QSharedPointer<QSettings> RenameSettingsModel::qSettings(int row) const
{
    Q_ASSERT(row > 0 && row < m_iniBaseNames.count());

    QString iniPath{Application::settingsIniPath(completeIniName(m_iniBaseNames.at(row)))};

    return QSharedPointer<QSettings>::create(iniPath, QSettings::IniFormat);
}

QSharedPointer<QSettings> RenameSettingsModel::qSettingsForLastUsed() const
{
    QString iniPath{Application::settingsIniPath(QString{lastSettingsFileName})};

    return QSharedPointer<QSettings>::create(iniPath, QSettings::IniFormat);
}

void RenameSettingsModel::load()
{
    beginResetModel();

    m_iniBaseNames = {tr("New settings")};

    QDir dir{Application::settingsDirPath()};

    if (!dir.exists()) {
        endResetModel();
        return;
    }

    QStringList completeNames = dir.entryList({"*.ini"}, QDir::Files, QDir::Name);
    qsizetype indexLastUsed = completeNames.indexOf(QString{lastSettingsFileName});

    if (indexLastUsed != -1)
        completeNames.removeAt(indexLastUsed);

    for (QStringView completeName : completeNames)
        m_iniBaseNames.append(completeName.chopped(4).toString());

    if (indexLastUsed != -1)
        m_iniBaseNames.append(QString{lastSettingsFileName}.chopped(4));

    endResetModel();

    emit loaded();
}
