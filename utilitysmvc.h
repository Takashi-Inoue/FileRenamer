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

#include <QAbstractItemView>
#include <QList>
#include <QMimeData>

namespace {

constexpr char mimeTypeModelDataList[] = "application/x-qabstractitemmodeldatalist";

QList<int> rowsFromMimeData(const QMimeData *mimeData)
{
    if (!mimeData->hasFormat(mimeTypeModelDataList))
        return {};

    QList<int> rows;
    QByteArray encodedData = mimeData->data(mimeTypeModelDataList);
    QDataStream stream(&encodedData, QIODeviceBase::ReadOnly);

    while (!stream.atEnd()) {
        int row;
        int col;
        QMap<int, QVariant> roleDataMap;

        stream >> row >> col >> roleDataMap;

        if (col == 0)
            rows << row;
    }

    std::sort(rows.begin(), rows.end());

    return rows;
}

QList<int> intListFromMimeData(const QMimeData *mimeData, const QString &mimeType)
{
    if (!mimeData->hasFormat(mimeType))
        return {};

    QList<int> nums;
    int num;
    QByteArray encodedData = mimeData->data(mimeType);
    QDataStream stream(&encodedData, QIODeviceBase::ReadOnly);

    while (!stream.atEnd()) {
        stream >> num;
        nums.append(num);
    }

    return nums;
}

} // anonymous
