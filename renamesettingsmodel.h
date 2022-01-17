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

#pragma once

#include <QAbstractTableModel>
#include <QSharedPointer>

class QSettings;

namespace StringBuilder { class SettingsModel; }

class RenameSettingsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RenameSettingsModel(QObject *parent = nullptr);

public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public:
    bool existsLastTimeSettings() const;
    int insertNewSettings(QStringView settingsName);
    bool isEditable(int row);
    bool isNewSettings(QStringView baseName) const;
    int rowForLastTimeSetting() const;
    QList<int> notEditableRows() const;
    void saveAsLastUsed(StringBuilder::SettingsModel *builderChainModel);
    void saveAsLastTime(StringBuilder::SettingsModel *builderChainModel);
    QSharedPointer<QSettings> qSettings(int row) const;

signals:
    void loaded();

public slots:
    void load();

private:
    QStringList m_iniBaseNames;
};

