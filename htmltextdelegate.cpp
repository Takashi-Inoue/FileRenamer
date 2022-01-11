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

#include "htmltextdelegate.h"

#include <QApplication>
#include <QPainter>
#include <QTextDocument>

void HtmlTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;

    initStyleOption(&opt, index);

    QString text = opt.text;
    opt.text = QString{};

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    QRect drawRect{opt.rect};
    QTextDocument doc;
    doc.setHtml(text);
    doc.setTextWidth(drawRect.width());

    painter->save();
    painter->translate(drawRect.topLeft());
    painter->setClipRect(drawRect.translated(-drawRect.x(), -drawRect.y()));
    doc.drawContents(painter);
    painter->restore();
}
