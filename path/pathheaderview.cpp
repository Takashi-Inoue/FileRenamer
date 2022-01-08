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

#include "pathheaderview.h"

#include <QMouseEvent>

PathHeaderView::PathHeaderView(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent)
{
    setDefaultSectionSize(150);
    setStretchLastSection(true);
    setSortIndicatorShown(true);

    connect(this, &QHeaderView::sectionClicked, this, [this]() {
        setSortIndicatorShown(true);
    });
}

void PathHeaderView::setEnableToChangeItems(bool isEnable)
{
    m_isEnableToChangeItems = isEnable;
}

void PathHeaderView::mouseMoveEvent(QMouseEvent *event)
{
    QHeaderView::mouseMoveEvent(event);

    bool sectionIsNewName = logicalIndexAt(event->pos()) == 1;

    setSectionsClickable(m_isEnableToChangeItems & !sectionIsNewName);
}
