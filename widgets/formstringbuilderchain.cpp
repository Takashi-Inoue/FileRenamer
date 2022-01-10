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

#include "formstringbuilderchain.h"
#include "ui_formstringbuilderchain.h"

#include "application.h"
#include "formstringbuilder.h"
#include "stringbuilder/widgets/abstractstringbuilderwidget.h"

#include <QSettings>
#include <QTimer>

namespace {
constexpr char settingsKeyCount[] = "SettingsCount";
}

FormStringBuilderChain::FormStringBuilderChain(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::FormStringBuilderChain),
      m_timer(new QTimer(this))
{
    ui->setupUi(this);

    m_timer->setSingleShot(true);
    m_timer->setInterval(QApplication::keyboardInputInterval());

    connect(this, &FormStringBuilderChain::settingsIndicesChanged,
            this, &FormStringBuilderChain::onSettingsIndicesChanged);

    connect(m_timer, &QTimer::timeout, this, [this]() {
        emit settingsChanged(builderChain());
    });

    loadBuilderSettings(Application::mainQSettings());
}

FormStringBuilderChain::~FormStringBuilderChain()
{
    int count = ui->vLayout->count();

    for (int i = 0; i < count; ++i) {
        QLayoutItem *item = ui->vLayout->itemAt(i);
        auto widget = qobject_cast<FormStringBuilder *>(item->widget());

        if (widget != nullptr) {
            disconnect(widget, &FormStringBuilder::destroyed,
                       this, &FormStringBuilderChain::startTimer);
        }
    }

    delete ui;
}

QSharedPointer<StringBuilder::OnFile::BuilderChainOnFile> FormStringBuilderChain::builderChain() const
{
    auto builderChain = QSharedPointer<StringBuilder::OnFile::BuilderChainOnFile>::create();

    int count = ui->vLayout->count();

    for (int i = 0; i < count; ++i) {
        QLayoutItem *item = ui->vLayout->itemAt(i);
        auto widget = qobject_cast<FormStringBuilder *>(item->widget());

        if (widget != nullptr)
            builderChain->addBuilder(widget->stringBuilder());
    }

    return builderChain;
}

void FormStringBuilderChain::loadBuilderSettings(QSharedPointer<QSettings> qSettings)
{
    removeAllBuilderWidgets();

    int count = qSettings->value(settingsKeyCount, 1).toInt();

    for (int i = 0; i < count; ++i) {
        qSettings->beginGroup(QString::number(i));
        createNewSetting(0)->loadBuilderSettings(qSettings);
        qSettings->endGroup();
    }
}

void FormStringBuilderChain::saveCurrentBuilderSettings(QSharedPointer<QSettings> qSettings) const
{
    QList<FormStringBuilder *> formsBuilder = findChildren<FormStringBuilder *>();

    qsizetype formsBuilderSize = formsBuilder.size();

    qSettings->setValue(settingsKeyCount, formsBuilderSize);

    if (formsBuilder.isEmpty())
        return;

    QStringList childGroups = qSettings->childGroups();
    QString groupName;
    qsizetype groupNumber = 0;

    while (childGroups.contains(groupName = QString::number(groupNumber++))) {
        qSettings->beginGroup(groupName);
        qSettings->remove("");
        qSettings->endGroup();
    }

    for (qsizetype i = 0; i < formsBuilderSize; ++i) {
        qSettings->beginGroup(QString::number(i));
        formsBuilder[i]->saveCurrentBuilderSettings(qSettings);
        qSettings->endGroup();
    }
}

FormStringBuilder *FormStringBuilderChain::createNewSetting(int builderIndex)
{
    auto widget = new FormStringBuilder(this);

    widget->setCurrentBuilderIndex(builderIndex);
    widget->setAttribute(Qt::WA_DeleteOnClose, true);

    connect(widget, &FormStringBuilder::changeStarted, this, &FormStringBuilderChain::startTimer);
    connect(widget, &FormStringBuilder::destroyed, this, &FormStringBuilderChain::startTimer);
    connect(widget, &FormStringBuilder::requestDown, this, &FormStringBuilderChain::onBuilderRequestedDown);
    connect(widget, &FormStringBuilder::requestUp, this, &FormStringBuilderChain::onBuilderRequestedUp);
    connect(widget, &FormStringBuilder::requestRemove, this, &FormStringBuilderChain::onBuilderRequestedRemove);

    int settingCount = settingsCount();

    ui->vLayout->insertWidget(settingCount, widget);

    emit settingsIndicesChanged();

    startTimer();

    return widget;
}

void FormStringBuilderChain::saveLatestSettings() const
{
    QList<StringBuilder::AbstractWidget *> builderWidgets = findChildren<StringBuilder::AbstractWidget *>();

    if (builderWidgets.isEmpty())
        return;

    QSharedPointer<QSettings> qSettings = Application::qSettingsForLatestSettings();

    for (StringBuilder::AbstractWidget *builderWidget : builderWidgets)
        builderWidget->saveSettings(qSettings);
}

void FormStringBuilderChain::startTimer()
{
    if (findChildren<FormStringBuilder *>().isEmpty())
        emit builderCleared();

    m_timer->start();
    emit changeStarted();
}

void FormStringBuilderChain::onBuilderRequestedDown()
{
    QWidget *widget = qobject_cast<QWidget *>(sender());

    int layoutIndex = ui->vLayout->indexOf(widget);

    if (layoutIndex >= ui->vLayout->count() - 3)
        return;

    ui->vLayout->removeWidget(widget);
    ui->vLayout->insertWidget(layoutIndex + 1, widget);

    emit settingsIndicesChanged();
}

void FormStringBuilderChain::onBuilderRequestedUp()
{
    QWidget *widget = qobject_cast<QWidget *>(sender());

    int layoutIndex = ui->vLayout->indexOf(widget);

    if (layoutIndex == 0)
        return;

    ui->vLayout->removeWidget(widget);
    ui->vLayout->insertWidget(layoutIndex - 1, widget);

    emit settingsIndicesChanged();
}

void FormStringBuilderChain::onBuilderRequestedRemove()
{
    auto widget = qobject_cast<QWidget *>(sender());

    if (widget != nullptr)
        ui->vLayout->removeWidget(widget);

    emit settingsIndicesChanged();
}

void FormStringBuilderChain::onSettingsIndicesChanged()
{
    for (int i = 0, count = settingsCount(); i < count; ++i) {
        QLayoutItem *item = ui->vLayout->itemAt(i);
        auto widget = qobject_cast<FormStringBuilder *>(item->widget());

        if (widget != nullptr)
            widget->notifySettingIndexChanged(i, count);
    }

    startTimer();
}

void FormStringBuilderChain::removeAllBuilderWidgets()
{
    for (int i = 0, count = settingsCount(); i < count; ++i) {
        QLayoutItem *layoutItem = ui->vLayout->takeAt(0);
        layoutItem->widget()->close();
        delete layoutItem;
    }
}

int FormStringBuilderChain::settingsCount() const
{
    return ui->vLayout->count() - 2; // vLayout has "Plus" button and Spacer item.
}
