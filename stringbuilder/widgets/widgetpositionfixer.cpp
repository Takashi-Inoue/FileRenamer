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

#include "widgetpositionfixer.h"
#include "ui_widgetpositionfixer.h"

#include <QDebug>

namespace StringBuilder {

WidgetPositionFixer::WidgetPositionFixer(QWidget *parent) :
    QFrame{parent},
    ui{new Ui::WidgetPositionFixer}
{
    ui->setupUi(this);

    ui->labelDisplay->setVisible(false);

    connect(ui->spinBox, &QSpinBox::valueChanged, this, &WidgetPositionFixer::changeStarted);
}

WidgetPositionFixer::~WidgetPositionFixer()
{
    delete ui;
}

void WidgetPositionFixer::setValue(int value)
{
    if (value < ui->spinBox->minimum()) {
        setAlignmentUI(PositionAlignment::Leftmost);
    } else if (value > ui->spinBox->maximum()) {
        setAlignmentUI(PositionAlignment::Rightmost);
    } else {
        setAlignmentUI(PositionAlignment::Specified);
        ui->spinBox->setValue(value);
    }
}

int WidgetPositionFixer::value() const
{
    if (ui->buttonLeftmost->isChecked())
        return std::numeric_limits<int>::min();

    if (ui->buttonRightmost->isChecked())
        return std::numeric_limits<int>::max();

    return ui->spinBox->value();
}

bool WidgetPositionFixer::isLeftMost() const
{
    return ui->buttonLeftmost->isChecked();
}

bool WidgetPositionFixer::isRightMost() const
{
    return ui->buttonRightmost->isChecked();
}

void WidgetPositionFixer::setFocusToFirstWidget()
{
    (!isLeftMost() && !isRightMost())
            ? ui->spinBox->setFocus() : ui->buttonLeftmost->setFocus();
}

void WidgetPositionFixer::onPushButtonClicked(bool checked)
{
    if (checked) {
        PositionAlignment alignment = (sender() == ui->buttonLeftmost)
                                      ? PositionAlignment::Leftmost : PositionAlignment::Rightmost;
        setAlignmentUI(alignment);
    } else {
        setAlignmentUI(PositionAlignment::Specified);
    }

    emit changeStarted();
}

void WidgetPositionFixer::setAlignmentUI(PositionAlignment alignment)
{
    QHash<PositionAlignment, QList<bool>> uiSettings = {
        {PositionAlignment::Leftmost,  {true, false, false}},
        {PositionAlignment::Rightmost, {false, true, false}},
        {PositionAlignment::Specified, {false, false, true}},
    }; // leftmost button, rightmost button, spinbox

    ui->buttonLeftmost->setChecked(uiSettings[alignment][int(PositionAlignment::Leftmost)]);
    ui->buttonRightmost->setChecked(uiSettings[alignment][int(PositionAlignment::Rightmost)]);
    ui->spinBox->setVisible(uiSettings[alignment][int(PositionAlignment::Specified)]);
    ui->labelDisplay->setVisible(ui->spinBox->isHidden());

    if (alignment == PositionAlignment::Specified)
        return;

    QHash<PositionAlignment, QPair<QString, Qt::Alignment>> labelSettings = {
        {PositionAlignment::Leftmost,  qMakePair(tr("Leftmost"),  Qt::AlignLeft)}
      , {PositionAlignment::Rightmost, qMakePair(tr("Rightmost"), Qt::AlignRight)}
    };

    ui->labelDisplay->setAlignment(labelSettings[alignment].second | Qt::AlignVCenter);
    ui->labelDisplay->setText(labelSettings[alignment].first);
}

} // StringBuilder
