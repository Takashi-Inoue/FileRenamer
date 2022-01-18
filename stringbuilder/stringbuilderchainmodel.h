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

#include "usingstringbuilder.h"

#include <QAbstractTableModel>

class QSettings;

namespace StringBuilder {

class AbstractWidget;
class DialogBuilderSettings;

class BuilderChainModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BuilderChainModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action,
                         int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;

signals:
    void requestToSelect(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void settingsChanged(SharedBuilderChainOnFile);

public:
    void appendBuilders(StringBuilderList &&builders);
    SharedBuilderChainOnFile builderChain() const;
    StringBuilderList builders(const QModelIndexList &indexes) const;
    void clearSettings();
    bool isEmpty() const;
    DialogBuilderSettings *settingsDialog(QList<int> showIndexes, QWidget *parent) const;
    void removeSpecifiedRows(QModelIndexList &&indexes);

    void loadSettings(QSettings *qSet);
    void saveSettings(QSettings *qSet) const;

private:
    StringBuilderList m_builders;
};

} // namespace StringBuilder

