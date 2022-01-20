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

#include "comboboxselectionkeeper.h"

#include <QComboBox>

namespace {
constexpr char propertyCurrentText[] = "propertyCurrentText";
} // anonymous

ComboBoxSelectionKeeper::ComboBoxSelectionKeeper(QComboBox *combox, QObject *parent)
    : QObject{parent},
      m_combox(combox)
{
    Q_CHECK_PTR(combox);

    QAbstractItemModel *model = combox->model();

    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, [this]() {
        m_combox->setProperty(propertyCurrentText, m_combox->currentText());
    });

    connect(model, &QAbstractItemModel::modelReset, this, [this]() {
        if (QVariant value = m_combox->property(propertyCurrentText); value.isValid()) {
            QString text = value.toString();

            if (int index = m_combox->findText(text); index != -1)
                m_combox->setCurrentIndex(index);

            m_combox->setProperty(propertyCurrentText, QVariant{});
        }
    });
}
