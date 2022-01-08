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

#pragma once

#include <QMenu>

class PathModel;

class QAbstractItemView;

class PathTableViewMenu : public QMenu
{
    Q_OBJECT
public:
    enum class Action : int {
        RemoveItem, CopyName, DeletePath, OpenPath, OpenMulti
    };

    enum class Section : int {
        FileName, MultiFiles
    };

    Q_ENUM(Action)

    PathTableViewMenu(QAbstractItemView *tableView);

    void addActionsToWidget(QWidget *widget) const;
    void setEnabledActions(Action action, bool isEnable);
    void updateActions();

signals:
    void requestRemoveItem();
    void requestCopyName();
    void requestDeletePath();
    void requestOpenPath();
    void requestOpenMulti();

private:
    void createActions();
    void createMenu();
    QString elide(QStringView text, const QFont &font, int width) const;

    QHash<Action, QAction *> m_actions;
    QHash<Section, QAction *> m_sections;

    QAbstractItemView *m_view;
};
