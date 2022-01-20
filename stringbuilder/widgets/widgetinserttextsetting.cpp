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

#include "filenamevalidator.h"
#include "stringbuilder/insertstring.h"

namespace StringBuilder {

namespace {
constexpr char settingsGroupName[] = "TextInsertion";
constexpr char settingsKeyText[] = "Text";
}

WidgetInsertTextSetting::WidgetInsertTextSetting(QWidget *parent)
    : WidgetInsertTextSetting(QString{}, 0, parent) {}

WidgetInsertTextSetting::WidgetInsertTextSetting(QStringView text, int insertPos, QWidget *parent)
    : AbstractWidget{parent},
      ui{new Ui::WidgetInsertTextSetting}
{
    ui->setupUi(this);

    setWindowTitle(tr("Insert Text"));

    WidgetInsertTextSetting::loadSettings();

    ui->combox->setValidator(new FileNameVlidator(this));
    ui->combox->setCurrentText(text.toString());
    ui->widgetPositionFixer->setValue(insertPos);

    connect(ui->widgetPositionFixer, SIGNAL(changeStarted()), this, SIGNAL(changeStarted()));
    connect(ui->combox, SIGNAL(currentTextChanged(QString)), this, SIGNAL(changeStarted()));
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

void WidgetInsertTextSetting::loadSettings()
{
    QSettings *qSet = qSettings();

    qSet->beginGroup(settingsGroupName);
    ui->combox->clear();
    ui->combox->loadSettings(qSet, settingsKeyText);
    qSet->endGroup();
}

void WidgetInsertTextSetting::saveSettings() const
{
    QSettings *qSet = qSettings();

    qSet->beginGroup(settingsGroupName);
    ui->combox->saveSettings(qSet, settingsKeyText);
    qSet->endGroup();
}

void WidgetInsertTextSetting::setFocusToFirstWidget()
{
    ui->combox->setFocus();
}

QString WidgetInsertTextSetting::insertText() const
{
    return ui->combox->currentText();
}

int WidgetInsertTextSetting::insertPosition() const
{
    return ui->widgetPositionFixer->value();
}

void WidgetInsertTextSetting::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange && !isEnabled())
        ui->combox->insertCurrentTextToItem(0);

    AbstractWidget::changeEvent(event);
}

} // StringBuilder
