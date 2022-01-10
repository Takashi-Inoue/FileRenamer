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

#include "widgetnumbersetting.h"
#include "ui_widgetnumbersetting.h"

#include "filenamevlidator.h"
#include "stringbuilder/number.h"

namespace StringBuilder {

namespace {
constexpr char settingsGroupName[] = "Number";
constexpr char settingsKeyStart[] = "Start";
constexpr char settingsKeyIncremental[] = "Incremental";
constexpr char settingsKeyDigit[] = "Digit";
constexpr char settingsKeyPrefix[] = "Prefix";
constexpr char settingsKeySuffix[] = "Suffix";
} // anonymous

WidgetNumberSetting::WidgetNumberSetting(QWidget *parent) :
    AbstractWidget{parent},
    ui{new Ui::WidgetNumberSetting}
{
    ui->setupUi(this);

    auto fileNameValidator = new FileNameVlidator{this};

    ui->comboxPrefix->setValidator(fileNameValidator);
    ui->comboxSuffix->setValidator(fileNameValidator);

    connect(ui->spinBoxStart, &QSpinBox::valueChanged,
            this, &AbstractWidget::changeStarted);

    connect(ui->spinBoxStep, &QSpinBox::valueChanged,
            this, &AbstractWidget::changeStarted);

    connect(ui->spinBoxDigit, &QSpinBox::valueChanged,
            this, &AbstractWidget::changeStarted);

    connect(ui->comboxPrefix, &QComboBox::currentTextChanged,
            this, &AbstractWidget::changeStarted);

    connect(ui->comboxSuffix, &QComboBox::currentTextChanged,
            this, &AbstractWidget::changeStarted);

    connect(ui->widgetPositionFixer, &WidgetPositionFixer::changeStarted,
            this, &AbstractWidget::changeStarted);
}

WidgetNumberSetting::~WidgetNumberSetting()
{
    delete ui;
}

QSharedPointer<AbstractWidget> WidgetNumberSetting::clone() const
{
    auto widget = QSharedPointer<WidgetNumberSetting>::create();

    widget->ui->spinBoxStart->setValue(ui->spinBoxStart->value());
    widget->ui->spinBoxStep->setValue(ui->spinBoxStep->value());
    widget->ui->spinBoxDigit->setValue(ui->spinBoxDigit->value());
    widget->ui->comboxPrefix->setCurrentText(ui->comboxPrefix->currentText());
    widget->ui->comboxSuffix->setCurrentText(ui->comboxSuffix->currentText());
    widget->ui->widgetPositionFixer->setValue(ui->widgetPositionFixer->value());

    return widget;
}

QString WidgetNumberSetting::builderName() const
{
    return QStringLiteral("Number");
}

QString WidgetNumberSetting::toString() const
{
    return QStringLiteral("Number %1%2%3, inc %4 > pos:%5")
            .arg(ui->comboxPrefix->currentText())
            .arg(ui->spinBoxStart->value(), ui->spinBoxDigit->value(), 10, QLatin1Char('0'))
            .arg(ui->comboxSuffix->currentText())
            .arg(ui->spinBoxStep->value())
            .arg(ui->widgetPositionFixer->value());
}

QFont WidgetNumberSetting::fontForDisplay() const
{
    return QFont{};
}

Qt::Alignment WidgetNumberSetting::alignForDisplay() const
{
    if (ui->widgetPositionFixer->isRightMost())
        return Qt::AlignRight | Qt::AlignVCenter;

    return Qt::AlignLeft | Qt::AlignVCenter;
}

QSharedPointer<AbstractStringBuilder> WidgetNumberSetting::stringBuilder() const
{
    return QSharedPointer<Number>::create(
                ui->widgetPositionFixer->value(),
                ui->spinBoxStart->value(), ui->spinBoxStep->value(), ui->spinBoxDigit->value(),
                ui->comboxPrefix->currentText(), ui->comboxSuffix->currentText());
}

void WidgetNumberSetting::loadSettings(QSharedPointer<QSettings> qSettings)
{
    qSettings->beginGroup(settingsGroupName);

    ui->comboxPrefix->clear();
    ui->comboxSuffix->clear();
    ui->comboxPrefix->loadSettings(qSettings, settingsKeyPrefix);
    ui->comboxSuffix->loadSettings(qSettings, settingsKeySuffix);
    ui->comboxPrefix->setEditText(qSettings->value(settingsKeyPrefix).toString());
    ui->comboxSuffix->setEditText(qSettings->value(settingsKeySuffix).toString());

    ui->spinBoxStart->setValue(qSettings->value(settingsKeyStart, 0).toInt());
    ui->spinBoxStep->setValue(qSettings->value(settingsKeyIncremental, 1).toInt());
    ui->spinBoxDigit->setValue(qSettings->value(settingsKeyDigit, 0).toInt());
    ui->widgetPositionFixer->loadSettings(qSettings);

    qSettings->endGroup();
}

void WidgetNumberSetting::saveSettings(QSharedPointer<QSettings> qSettings) const
{
    qSettings->beginGroup(settingsGroupName);

    qSettings->setValue(settingsKeyStart, ui->spinBoxStart->value());
    qSettings->setValue(settingsKeyIncremental, ui->spinBoxStep->value());
    qSettings->setValue(settingsKeyDigit, ui->spinBoxDigit->value());
    qSettings->setValue(settingsKeyPrefix, ui->comboxPrefix->currentText());
    qSettings->setValue(settingsKeySuffix, ui->comboxSuffix->currentText());
    ui->widgetPositionFixer->saveSettings(qSettings);

    ui->comboxPrefix->saveSettings(qSettings, settingsKeyPrefix);
    ui->comboxSuffix->saveSettings(qSettings, settingsKeySuffix);

    qSettings->endGroup();
}

void WidgetNumberSetting::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange && !isEnabled()) {
        ui->comboxPrefix->insertCurrentTextToItem(0);
        ui->comboxSuffix->insertCurrentTextToItem(0);
    }

    AbstractWidget::changeEvent(event);
}

} // StringBuilder