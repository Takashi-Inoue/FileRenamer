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
#include "stringbuilder/stringbuildersettingsmodel.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QDebug>

namespace {
constexpr char lastUsedFileName[] = "Last used.ini";
constexpr char lastTimeFileName[] = "Last time.ini";

QString newSettingsName() { return QObject::tr("New settings"); }
QString lastUsedBaseName() { return QString{lastUsedFileName}.chopped(4); }
QString lastTimeBaseName() { return QString{lastTimeFileName}.chopped(4); }

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

bool RenameSettingsModel::existsLastTimeSettings() const
{
    return m_iniBaseNames.contains(lastTimeBaseName());
}

int RenameSettingsModel::insertNewSettings(QStringView settingsName)
{
    int notEditableCount = 0;

    if (m_iniBaseNames.contains(lastUsedBaseName()))
        ++notEditableCount;

    if (m_iniBaseNames.contains(lastTimeBaseName()))
        ++notEditableCount;

    auto itr = std::upper_bound(++m_iniBaseNames.cbegin(), m_iniBaseNames.cend() - notEditableCount,
                                settingsName);
    const int insertIndex = std::distance(m_iniBaseNames.cbegin(), itr);

    beginInsertRows(QModelIndex{}, insertIndex, insertIndex);
    m_iniBaseNames.insert(insertIndex, settingsName.toString());
    endInsertRows();

    return insertIndex;
}

bool RenameSettingsModel::isEditable(int row)
{
    if (row == 0)
        return false;

    if (row == m_iniBaseNames.indexOf(lastUsedBaseName()))
        return false;

    if (row == m_iniBaseNames.indexOf(lastTimeBaseName()))
        return false;

    return true;
}

bool RenameSettingsModel::isNewSettings(QStringView baseName) const
{
    return baseName == newSettingsName();
}

int RenameSettingsModel::rowForLastTimeSetting() const
{
    return m_iniBaseNames.indexOf(lastTimeBaseName());
}

QList<int> RenameSettingsModel::notEditableRows() const
{
    QList<int> rows = {0};

    if (qsizetype index = m_iniBaseNames.indexOf(lastUsedBaseName()); index != -1)
        rows.append(index);

    if (qsizetype index = m_iniBaseNames.indexOf(lastTimeBaseName()); index != -1)
        rows.append(index);

    return rows;
}

void RenameSettingsModel::saveAsLastUsed(StringBuilder::SettingsModel *builderChainModel)
{
    const QString iniPath{Application::settingsIniPath(QString{lastUsedFileName})};
    QSettings qSet{iniPath, QSettings::IniFormat};

    builderChainModel->saveSettings(&qSet);

    if (!m_iniBaseNames.contains(lastUsedBaseName())) {
        const int insertIndex = m_iniBaseNames.contains(lastTimeBaseName())
                                ? m_iniBaseNames.size() - 1 : m_iniBaseNames.size();
        beginInsertRows(QModelIndex{}, insertIndex, insertIndex);
        m_iniBaseNames.insert(insertIndex, lastUsedBaseName());
        endInsertRows();
    }
}

void RenameSettingsModel::saveAsLastTime(StringBuilder::SettingsModel *builderChainModel)
{
    const QString iniPath{Application::settingsIniPath(QString{lastTimeFileName})};
    QSettings qSet{iniPath, QSettings::IniFormat};

    builderChainModel->saveSettings(&qSet);
}

QSharedPointer<QSettings> RenameSettingsModel::qSettings(int row) const
{
    Q_ASSERT(row > 0 && row < m_iniBaseNames.count());

    QString iniPath{Application::settingsIniPath(completeIniName(m_iniBaseNames.at(row)))};

    return QSharedPointer<QSettings>::create(iniPath, QSettings::IniFormat);
}

void RenameSettingsModel::load()
{
    beginResetModel();

    m_iniBaseNames = {newSettingsName()};

    QDir dir{Application::settingsDirPath()};

    if (!dir.exists()) {
        endResetModel();
        return;
    }

    QStringList completeNames = dir.entryList({"*.ini"}, QDir::Files, QDir::Name);
    qsizetype indexLastUsed = completeNames.indexOf(QString{lastUsedFileName});

    if (indexLastUsed != -1)
        completeNames.removeAt(indexLastUsed);

    qsizetype indexLastTime = completeNames.indexOf(QString{lastTimeFileName});

    if (indexLastTime != -1)
        completeNames.removeAt(indexLastTime);

    for (QStringView completeName : completeNames)
        m_iniBaseNames.append(completeName.chopped(4).toString());

    if (indexLastUsed != -1)
        m_iniBaseNames.append(lastUsedBaseName());

    if (indexLastTime != -1)
        m_iniBaseNames.append(lastTimeBaseName());

    endResetModel();

    emit loaded();
}
