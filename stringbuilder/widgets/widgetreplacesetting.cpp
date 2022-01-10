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

#include "widgetreplacesetting.h"
#include "ui_widgetreplacesetting.h"

#include "application.h"
#include "filenamevlidator.h"
#include "stringbuilder/replacestring.h"

#include <QSettings>
#include <QDebug>

namespace StringBuilder {

namespace {
constexpr char settingsGroupName[] = "TextReplace";
constexpr char settingsKeyReplace[]  = "Replace";
constexpr char settingsKeySearch[] = "Search";
constexpr char settingsKeyUseRegexp[] = "UseRegexp";
constexpr char settingsKeyCaseSensitive[] = "CaseSensitive";
} // anonymous

WidgetReplaceSetting::WidgetReplaceSetting(QWidget *parent)
    : AbstractWidget{parent},
      ui{new Ui::WidgetReplaceSetting}
{
    ui->setupUi(this);

    ui->comboxReplace->setValidator(new FileNameVlidator(this));

    connect(ui->checkBoxCaseSensitive, &QCheckBox::clicked,
            this, &AbstractWidget::changeStarted);

    connect(ui->checkBoxUseRegex, &QCheckBox::clicked,
            this, &AbstractWidget::changeStarted);

    connect(ui->comboxReplace, &QComboBox::currentTextChanged,
            this, &AbstractWidget::changeStarted);

    connect(ui->comboxSearch, &QComboBox::currentTextChanged,
            this, &AbstractWidget::changeStarted);
}

WidgetReplaceSetting::~WidgetReplaceSetting()
{
    delete ui;
}

QSharedPointer<AbstractWidget> WidgetReplaceSetting::clone() const
{
    auto widget = QSharedPointer<WidgetReplaceSetting>::create();

    widget->ui->comboxSearch->setCurrentText(ui->comboxSearch->currentText());
    widget->ui->comboxReplace->setCurrentText(ui->comboxReplace->currentText());
    widget->ui->checkBoxUseRegex->setChecked(ui->checkBoxUseRegex->isChecked());
    widget->ui->checkBoxCaseSensitive->setChecked(ui->checkBoxCaseSensitive->isChecked());

    return widget;
}

QString WidgetReplaceSetting::builderName() const
{
    return QStringLiteral("Replace");
}

QString WidgetReplaceSetting::toString() const
{
    return QStringLiteral("%1 > %2").arg(ui->comboxSearch->currentText(),
                                         ui->comboxReplace->currentText());
}

QFont WidgetReplaceSetting::fontForDisplay() const
{
    return QFont{};
}

Qt::Alignment WidgetReplaceSetting::alignForDisplay() const
{
    return Qt::AlignCenter;
}

QSharedPointer<AbstractStringBuilder> WidgetReplaceSetting::stringBuilder() const
{
    QString replace = ui->comboxReplace->currentText();
    QString search = ui->comboxSearch->currentText();
    bool isUseRegExp = ui->checkBoxUseRegex->isChecked();
    bool isCaseSensitive = ui->checkBoxCaseSensitive->isChecked();

    return QSharedPointer<ReplaceString>::create(search, replace, isUseRegExp, isCaseSensitive);
}

void WidgetReplaceSetting::loadSettings(QSharedPointer<QSettings> qSettings)
{
    qSettings->beginGroup(settingsGroupName);

    ui->comboxReplace->clear();
    ui->comboxSearch->clear();
    ui->comboxReplace->loadSettings(qSettings, settingsKeyReplace);
    ui->comboxSearch->loadSettings(qSettings, settingsKeySearch);
    ui->comboxReplace->setEditText(qSettings->value(settingsKeyReplace).toString());
    ui->comboxSearch->setEditText(qSettings->value(settingsKeySearch).toString());

    ui->checkBoxUseRegex->setChecked(qSettings->value(settingsKeyUseRegexp).toBool());
    ui->checkBoxCaseSensitive->setChecked(qSettings->value(settingsKeyCaseSensitive, true).toBool());

    qSettings->endGroup();
}

void WidgetReplaceSetting::saveSettings(QSharedPointer<QSettings> qSettings) const
{
    qSettings->beginGroup(settingsGroupName);

    qSettings->setValue(settingsKeyReplace, ui->comboxReplace->currentText());
    qSettings->setValue(settingsKeySearch, ui->comboxSearch->currentText());
    qSettings->setValue(settingsKeyUseRegexp, ui->checkBoxUseRegex->isChecked());
    qSettings->setValue(settingsKeyCaseSensitive, ui->checkBoxCaseSensitive->isChecked());

    ui->comboxReplace->saveSettings(qSettings, settingsKeyReplace);
    ui->comboxSearch->saveSettings(qSettings, settingsKeySearch);

    qSettings->endGroup();
}

void WidgetReplaceSetting::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange && !isEnabled()) {
        ui->comboxReplace->insertCurrentTextToItem(0);
        ui->comboxSearch->insertCurrentTextToItem(0);
    }

    AbstractWidget::changeEvent(event);
}

} // StringBuilder
