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
#include "filenamevalidator.h"
#include "stringbuilder/replacestring.h"

#include <QSettings>
#include <QDebug>

namespace StringBuilder {

namespace {
constexpr char settingsGroupName[] = "TextReplace";
constexpr char settingsKeyReplace[]  = "Replace";
constexpr char settingsKeySearch[] = "Search";
} // anonymous

WidgetReplaceSetting::WidgetReplaceSetting(QWidget *parent)
    : WidgetReplaceSetting(QString{}, QString{}, false, true) {}

WidgetReplaceSetting::WidgetReplaceSetting(QStringView find, QStringView replace, bool isUseRegExp,
                                           bool isCaseSensitive, QWidget *parent)
    : AbstractWidget{parent},
      ui{new Ui::WidgetReplaceSetting}
{
    ui->setupUi(this);

    setWindowTitle(tr("Replace"));

    WidgetReplaceSetting::loadSettings();

    ui->comboxReplace->setValidator(new FileNameVlidator(this));
    ui->comboxSearch->setCurrentText(find.toString());
    ui->comboxReplace->setCurrentText(replace.toString());
    ui->checkBoxUseRegex->setChecked(isUseRegExp);
    ui->checkBoxCaseSensitive->setChecked(isCaseSensitive);

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

QSharedPointer<AbstractStringBuilder> WidgetReplaceSetting::stringBuilder() const
{
    QString replace = ui->comboxReplace->currentText();
    QString search = ui->comboxSearch->currentText();
    bool isUseRegExp = ui->checkBoxUseRegex->isChecked();
    bool isCaseSensitive = ui->checkBoxCaseSensitive->isChecked();

    return QSharedPointer<ReplaceString>::create(search, replace, isUseRegExp, isCaseSensitive);
}

void WidgetReplaceSetting::loadSettings()
{
    QSettings *qSet = qSettings();

    qSet->beginGroup(settingsGroupName);

    ui->comboxReplace->clear();
    ui->comboxSearch->clear();
    ui->comboxReplace->loadSettings(qSet, settingsKeyReplace);
    ui->comboxSearch->loadSettings(qSet, settingsKeySearch);

    qSet->endGroup();
}

void WidgetReplaceSetting::saveSettings() const
{
    QSettings *qSet = qSettings();

    qSet->beginGroup(settingsGroupName);

    ui->comboxReplace->saveSettings(qSet, settingsKeyReplace);
    ui->comboxSearch->saveSettings(qSet, settingsKeySearch);

    qSet->endGroup();
}

void WidgetReplaceSetting::setFocusToFirstWidget()
{
    ui->comboxSearch->setFocus();
}

QString WidgetReplaceSetting::findString() const
{
    return ui->comboxSearch->currentText();
}

QString WidgetReplaceSetting::replaceString() const
{
    return ui->comboxReplace->currentText();
}

bool WidgetReplaceSetting::isUseRegExp() const
{
    return ui->checkBoxUseRegex->isChecked();
}

bool WidgetReplaceSetting::isCaseSensitive() const
{
    return ui->checkBoxCaseSensitive->isChecked();
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
