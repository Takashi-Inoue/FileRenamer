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

#include "widgetinserttextsetting.h"
#include "ui_widgetinserttextsetting.h"

#include "filenamevlidator.h"
#include "stringbuilder/insertstring.h"

namespace StringBuilder {

namespace {
constexpr char settingsGroupName[] = "TextInsertion";
constexpr char settingsKeyText[] = "Text";
}

WidgetInsertTextSetting::WidgetInsertTextSetting(QWidget *parent)
    : WidgetInsertTextSetting(QString{}, 0, parent)
{
}

WidgetInsertTextSetting::WidgetInsertTextSetting(QStringView text, int insertPos, QWidget *parent)
    : AbstractWidget{parent},
      ui{new Ui::WidgetInsertTextSetting}
{
    ui->setupUi(this);

    ui->combox->setValidator(new FileNameVlidator(this));
    ui->combox->setCurrentText(text.toString());
    ui->widgetPositionFixer->setValue(insertPos);

    connect(ui->widgetPositionFixer, &WidgetPositionFixer::changeStarted,
            this, &AbstractWidget::changeStarted);

    connect(ui->combox, &QComboBox::currentTextChanged,
            this, &AbstractWidget::changeStarted);
}

WidgetInsertTextSetting::~WidgetInsertTextSetting()
{
    delete ui;
}

QSharedPointer<AbstractStringBuilder> WidgetInsertTextSetting::stringBuilder() const
{
    return QSharedPointer<InsertString>::create(
                ui->widgetPositionFixer->value(), ui->combox->currentText());
}

void WidgetInsertTextSetting::loadSettings(QSharedPointer<QSettings> qSettings)
{
    qSettings->beginGroup(settingsGroupName);

    ui->combox->clear();
    ui->combox->loadSettings(qSettings, settingsKeyText);
    ui->combox->setEditText(qSettings->value(settingsKeyText).toString());
    ui->widgetPositionFixer->loadSettings(qSettings);

    qSettings->endGroup();
}

void WidgetInsertTextSetting::saveSettings(QSharedPointer<QSettings> qSettings) const
{
    qSettings->beginGroup(settingsGroupName);

    qSettings->setValue(settingsKeyText, ui->combox->currentText());
    ui->combox->saveSettings(qSettings, settingsKeyText);
    ui->widgetPositionFixer->saveSettings(qSettings);

    qSettings->endGroup();
}

void WidgetInsertTextSetting::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange && !isEnabled())
        ui->combox->insertCurrentTextToItem(0);

    AbstractWidget::changeEvent(event);
}

} // StringBuilder
