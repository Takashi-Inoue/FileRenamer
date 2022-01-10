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

WidgetInsertTextSetting::WidgetInsertTextSetting(QWidget *parent) :
    AbstractWidget{parent},
    ui{new Ui::WidgetInsertTextSetting}
{
    ui->setupUi(this);

    ui->combox->setValidator(new FileNameVlidator(this));

    connect(ui->widgetPositionFixer, &WidgetPositionFixer::changeStarted,
            this, &AbstractWidget::changeStarted);

    connect(ui->combox, &QComboBox::currentTextChanged,
            this, &AbstractWidget::changeStarted);
}

WidgetInsertTextSetting::~WidgetInsertTextSetting()
{
    delete ui;
}

QSharedPointer<AbstractWidget> WidgetInsertTextSetting::clone() const
{
    auto widget = QSharedPointer<WidgetInsertTextSetting>::create();

    widget->ui->combox->setCurrentText(ui->combox->currentText());
    widget->ui->widgetPositionFixer->setValue(ui->widgetPositionFixer->value());

    return widget;
}

QString WidgetInsertTextSetting::builderName() const
{
    return QStringLiteral("Insert Text");
}

QString WidgetInsertTextSetting::toString() const
{
    QString result = QStringLiteral("%1").arg(ui->combox->currentText());

    if (ui->widgetPositionFixer->isLeftMost())
        return QStringLiteral("<<%1").arg(result);

    if (ui->widgetPositionFixer->isRightMost())
        return QStringLiteral("%1>>").arg(result);

    return QStringLiteral("%1 > pos:%2").arg(result).arg(ui->widgetPositionFixer->value());
}

QFont WidgetInsertTextSetting::fontForDisplay() const
{
    QFont font{};

    font.setItalic(true);

    return font;
}

Qt::Alignment WidgetInsertTextSetting::alignForDisplay() const
{
    if (ui->widgetPositionFixer->isRightMost())
        return Qt::AlignRight | Qt::AlignVCenter;

    return Qt::AlignLeft | Qt::AlignVCenter;
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
