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

#include "stringbuildersettingsmodel.h"
#include "abstractstringbuilder.h"
#include "onfile/builderchainonfile.h"
#include "stringbuilder/widgets/dialogbuildersettings.h"
#include "stringbuilderfactory.h"
#include "utilitysmvc.h"

#include <QIODeviceBase>
#include <QMimeData>
#include <QSettings>

namespace StringBuilder {

namespace Settings {
constexpr char groupName[] = "SettingsList";
constexpr char keyTypeList[] = "Types";
} // Settings

namespace {

void moveItems(StringBuilderList &builders, QList<int> rows, int targetRow)
{
    if (rows.isEmpty())
        return;

    StringBuilderList removedItems;

    removedItems.reserve(rows.size());

    for (auto rItr = rows.rbegin(), rEnd = rows.rend(); rItr != rEnd; ++rItr)
        removedItems.append(builders.takeAt(*rItr));

    int lowerCount = std::distance(rows.cbegin(),
                                   std::lower_bound(rows.cbegin(), rows.cend(), targetRow));
    targetRow -= lowerCount;

    for (const SharedStringBuilder &builder : removedItems)
        builders.insert(targetRow, builder);
}

} // anonymous

SettingsModel::SettingsModel(QObject *parent)
    : QAbstractTableModel{parent}
{
}

int SettingsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_builders.size();
}

int SettingsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 1;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
        return m_builders.at(index.row())->toHtmlString();

    return QVariant();
}

bool SettingsModel::canDropMimeData(const QMimeData *data, Qt::DropAction /*action*/,
                                    int /*row*/, int /*column*/, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return false;

    if (data->hasFormat(mimeTypeBuilderType))
        return true;

    if (data->hasFormat(mimeTypeModelDataList))
        return true;

    return false;
}

bool SettingsModel::dropMimeData(const QMimeData *data, Qt::DropAction /*action*/,
                                 int /*row*/, int /*column*/, const QModelIndex &parent)
{
    const int targetRow = parent.row();

    if (targetRow == -1)
        return false;

    if (data->hasFormat(mimeTypeModelDataList)) {
        beginResetModel();
        moveItems(m_builders, rowsFromMimeData(data), targetRow);
        endResetModel();
    } else {
        return false;
    }

    emit settingsChanged(builderChain());

    return true;
}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled
            | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsDropEnabled;
}

void SettingsModel::appendBuilders(StringBuilderList &&builders)
{
    if (builders.isEmpty())
        return;

    const int first = m_builders.size();
    const int last = first + builders.size() - 1;

    beginInsertRows(QModelIndex(), first, last);

    m_builders.append(builders);

    endInsertRows();
}

SharedBuilderChainOnFile SettingsModel::builderChain() const
{
    auto builderChain = SharedBuilderChainOnFile::create();

    for (const SharedStringBuilder &builder : m_builders)
        builderChain->addBuilder(builder);

    builderChain->reset();

    return builderChain;
}

StringBuilderList SettingsModel::builders(const QModelIndexList &indexes) const
{
    StringBuilderList builders;

    for (const QModelIndex &index : indexes) {
        if (index.isValid())
            builders.append(m_builders.at(index.row()));
    }

    return builders;
}

bool SettingsModel::isEmpty() const
{
    return m_builders.isEmpty();
}

DialogBuilderSettings *SettingsModel::settingsDialog(QList<int> showIndexes, QWidget *parent) const
{
    QList<AbstractWidget *> widgets;

    for (const SharedStringBuilder &builder : m_builders)
        widgets.append(builder->settingsWidget());

    auto dlg = new DialogBuilderSettings(widgets, showIndexes, parent);

    dlg->deleteLater();

    return dlg;
}

void SettingsModel::removeSpecifiedRows(QModelIndexList &&indexes)
{
    std::sort(indexes.begin(), indexes.end(), [](const QModelIndex &lhs, const QModelIndex &rhs) {
        return rhs.row() < lhs.row();
    });

    beginResetModel();

    for (const QModelIndex &index : indexes)
        m_builders.removeAt(index.row());

    endResetModel();

    emit settingsChanged(builderChain());
}

void SettingsModel::loadSettings(QSettings *qSet)
{
    beginResetModel();

    m_builders.clear();

    qSet->beginGroup(Settings::groupName);

    QVariant &&typeIntList = qSet->value(Settings::keyTypeList, QVariant::fromValue(QList<int>{}));
    QList<int> types{typeIntList.value<QList<int>>()};

    for (int i = -1; int typeInt : types) {
        auto type = BuilderType(typeInt);

        SharedStringBuilder builder = BuilderFactory::createBuilder(type);

        qSet->beginGroup(QString::number(++i));
        builder->loadSettings(qSet);
        qSet->endGroup();

        m_builders.append(builder);
    }

    qSet->endGroup();

    endResetModel();
}

void SettingsModel::saveSettings(QSettings *qSet) const
{
    qSet->beginGroup(Settings::groupName);
    qSet->remove("");

    QList<int> types;

    for (int i = -1; const SharedStringBuilder &builder : m_builders) {
        types.append(int(builder->builderType()));

        qSet->beginGroup(QString::number(++i));
        builder->saveSettings(qSet);
        qSet->endGroup();
    }

    qSet->setValue(Settings::keyTypeList, QVariant::fromValue(types));

    qSet->endGroup();
}

} // namespace StringBuilder
