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

#include "abstractstringbuilderwidget.h"
#include "application.h"

#include <QSettings>
#include <QSharedPointer>

namespace StringBuilder {

AbstractWidget::AbstractWidget(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags),
      m_qSet(Application::mainQSettings())
{
}

void AbstractWidget::accept()
{
    saveSettings();

    emit accepted();
}

QSettings *AbstractWidget::qSettings() const
{
    return m_qSet.get();
}

void AbstractWidget::setQSettings(const QSharedPointer<QSettings> &qSet)
{
    if (m_qSet != qSet)
        m_qSet = qSet;
}

} // StringBuilder
