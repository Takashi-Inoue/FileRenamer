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

#include <QMimeData>

namespace StringBuilder {

namespace {
constexpr char mimeDataItemDefault[] = "application/x-qabstractitemmodeldatalist";
}

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

//    if (role == Qt::TextAlignmentRole)
//        return m_builders.at(index.row())->alignForDisplay().toInt();

    return QVariant();
}

bool SettingsModel::canDropMimeData(const QMimeData *data, Qt::DropAction action,
                                    int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action)
    Q_UNUSED(column)
    Q_UNUSED(row)

    if (!parent.isValid())
        return false;

    if (data->hasFormat(mimeTypeBuilderType))
        return true;

    if (data->hasFormat(mimeDataItemDefault))
        return true;

    return false;
}

bool SettingsModel::dropMimeData(const QMimeData *data, Qt::DropAction /*action*/,
                                 int row, int /*column*/, const QModelIndex &parent)
{
    return false;
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

} // namespace StringBuilder
