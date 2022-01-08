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

#include "formstringbuilder.h"
#include "ui_formstringbuilder.h"

#include "application.h"
#include "stringbuilderwidgetfactory.h"
#include "widgets/abstractstringbuilderwidget.h"
#include "widgets/widgetpositionfixer.h"

#include <QAction>
#include <QPushButton>
#include <QResizeEvent>
#include <QDebug>

FormStringBuilder::FormStringBuilder(QWidget *parent)
    : QFrame(parent),
      ui(new Ui::FormStringBuilder)
{
    ui->setupUi(this);

    connect(ui->buttonRemove, &QPushButton::clicked, this, &FormStringBuilder::requestRemove);
    connect(ui->buttonRemove, &QPushButton::clicked, this, &FormStringBuilder::close);

    connect(ui->buttonDown, &QPushButton::clicked, this, &FormStringBuilder::requestDown);
    connect(ui->buttonUp,   &QPushButton::clicked, this, &FormStringBuilder::requestUp);

    connect(ui->comboBoxBuilders, &QComboBox::currentIndexChanged,
            this, &FormStringBuilder::changeStarted);

    addBuilderWidget(0);
}

FormStringBuilder::~FormStringBuilder()
{
    delete ui;
}

QSharedPointer<StringBuilder::AbstractStringBuilder> FormStringBuilder::stringBuilder() const
{
    return currentBuilderWidget()->StringBuilder();
}

int FormStringBuilder::currentBuilderIndex() const
{
    return ui->comboBoxBuilders->currentIndex();
}

void FormStringBuilder::setCurrentBuilderIndex(int index)
{
    ui->comboBoxBuilders->setCurrentIndex(index);
}

void FormStringBuilder::loadBuilderSettings(QSharedPointer<QSettings> qSettings)
{
    int builderIndex = qSettings->value(QStringLiteral("BuilderType"), -1).toInt();

    if (builderIndex != -1)
        ui->comboBoxBuilders->setCurrentIndex(builderIndex);

    currentBuilderWidget()->loadSettings(qSettings);
}

void FormStringBuilder::saveCurrentBuilderSettings(QSharedPointer<QSettings> qSettings) const
{
    qSettings->setValue(QStringLiteral("BuilderType"), ui->comboBoxBuilders->currentIndex());

    currentBuilderWidget()->saveSettings(qSettings);
}

void FormStringBuilder::notifySettingIndexChanged(int index, int settingsCount)
{
    ui->buttonDown->setEnabled(index < settingsCount - 1);
    ui->buttonUp->setEnabled(index != 0);

    auto positionFixer = currentBuilderWidget()->findChild<WidgetPositionFixer *>();

    if (positionFixer != nullptr)
        positionFixer->setEnabled(index != 0);
}

void FormStringBuilder::onComboBoxBuildersCurrentIndexChanged(int index)
{
    addBuilderWidget(index);
}

void FormStringBuilder::addBuilderWidget(int builderIndex)
{
    QLayoutItem *child = nullptr;

    while ((child = ui->vLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    StringBuilderWidgetFactory factory;

    AbstractStringBuilderWidget *widget = factory.createWidget(builderIndex, this);

    connect(widget, &AbstractStringBuilderWidget::changeStarted,
            this, &FormStringBuilder::changeStarted);

    widget->loadSettings(Application::qSettingsForLatestSettings());

    ui->vLayout->addWidget(widget);
}

AbstractStringBuilderWidget *FormStringBuilder::currentBuilderWidget() const
{
    Q_ASSERT(ui->vLayout->count() > 0);

    return qobject_cast<AbstractStringBuilderWidget *>(ui->vLayout->itemAt(0)->widget());
}
