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

#include "elidelabel.h"

#include <QPainter>
#include <QPaintEvent>

namespace {
constexpr int iconMargin = 2;
}

void ElideLabel::setElideMode(Qt::TextElideMode elideMode)
{
    m_elideMode = elideMode;
}

void ElideLabel::setTextWithElide(QString text)
{
    m_originalText = text;

    elide();
}

void ElideLabel::setTextWithElide(QIcon icon, QString text)
{
    m_icon = icon;

    setTextWithElide(text);
}

void ElideLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);

    elide();
}

void ElideLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect drawRect(event->rect());

    if (!m_icon.isNull()) {
        drawRect.setWidth(drawRect.height());

        m_icon.paint(&painter, drawRect);

        drawRect = event->rect();
        drawRect.setLeft(drawRect.height() + iconMargin);
    }

    QTextDocument doc;
    doc.setDocumentMargin(0);
    doc.setHtml(text());

    painter.translate(drawRect.topLeft());
    painter.setClipRect(drawRect.translated(-drawRect.x(), -drawRect.y()));
    doc.drawContents(&painter);
}

void ElideLabel::elide()
{
    QFontMetrics fontMetrics(font());

    int textWidth = m_icon.isNull() ? width()
                                    : width() - height() - iconMargin;

    QString elidedText = fontMetrics.elidedText(m_originalText, m_elideMode, textWidth);

    setText(elidedText);
}
