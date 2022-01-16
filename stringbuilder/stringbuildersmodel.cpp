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

#include "stringbuildersmodel.h"

#include "stringbuilderfactory.h"

#include <QDataStream>
#include <QIODeviceBase>
#include <QMimeData>

namespace StringBuilder {

BuildersModel::BuildersModel(QObject *parent)
    : QAbstractTableModel{parent}
{
}

int BuildersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return builderTypeCount();
}

int BuildersModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 1;
}

QVariant BuildersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    if (role == Qt::DisplayRole)
        return builderName(BuilderType(index.row()));

    return QVariant();
}

Qt::ItemFlags BuildersModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled
            | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

QMimeData *BuildersModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray array;
    QDataStream stream(&array, QIODeviceBase::WriteOnly);

    for (const QModelIndex &index : indexes)
        stream << index.row();

    mimeData->setData(mimeTypeBuilderType, array);

    return mimeData;
}

StringBuilderList BuildersModel::createBuilders(const QModelIndexList &indexes) const
{
    StringBuilderList builders;

    for (const QModelIndex &index : indexes) {
        if (!index.isValid())
            continue;

        auto builderType = BuilderType(index.row());

        builders.append(BuilderFactory::createBuilder(builderType));
    }

    return builders;
}

} // namespace StringBuilder
