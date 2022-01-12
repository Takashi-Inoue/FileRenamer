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

#include "widgetloadsavebuildersettings.h"
#include "ui_widgetloadsavebuildersettings.h"

#include "application.h"
#include "filenamevalidator.h"
#include "stringbuilder/stringbuildersettingsmodel.h"

#include <QDir>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

namespace {

constexpr char lastSettingsFileName[] = "Last used.ini";

void initSettingsComboBox(QComboBox *combox)
{
    combox->addItem(QStringLiteral("New settings"));

    QDir dir{Application::settingsDirPath()};

    if (!dir.exists())
        return;

    QStringList &&inis = dir.entryList({QStringLiteral("*.ini")}, QDir::Files, QDir::Name);
    qsizetype indexLastUsed = inis.indexOf(QString{lastSettingsFileName});

    if (indexLastUsed != -1)
        inis.removeAt(indexLastUsed);

    for (QStringView iniName : inis)
        combox->addItem(iniName.chopped(4).toString(), iniName.toString());

    if (indexLastUsed != -1)
        combox->addItem(QString{lastSettingsFileName}.chopped(4), QString{lastSettingsFileName});
}

QString requestNewIniName()
{
    QInputDialog dlg;

    dlg.setInputMode(QInputDialog::TextInput);

    auto lineEdit = dlg.findChild<QLineEdit *>();

    if (lineEdit)
        lineEdit->setValidator(new FileNameVlidator{lineEdit});

    dlg.setWindowTitle(QStringLiteral("Save as new"));
    dlg.setLabelText(QStringLiteral("Settings name to save:"));

    if (dlg.exec() == QDialog::Rejected)
        return QString{};

    return QStringLiteral("%1.ini").arg(dlg.textValue());
}

} // anonymous

WidgetLoadSaveBuilderSettings::WidgetLoadSaveBuilderSettings(
        StringBuilder::SettingsModel *settingsModel, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WidgetLoadSaveBuilderSettings),
      m_settingsModel(settingsModel),
      m_saveButtonMenu(new QMenu{this})
{
    Q_CHECK_PTR(settingsModel);

    ui->setupUi(this);

    initSettingsComboBox(ui->comboxSettings);

    connect(ui->comboxSettings, &QComboBox::currentIndexChanged,
            this, &WidgetLoadSaveBuilderSettings::loadSettings);

    const int lastIndex = ui->comboxSettings->count() - 1;

    ui->comboxSettings->itemData(lastIndex) == QString{lastSettingsFileName}
        ? ui->comboxSettings->setCurrentIndex(lastIndex)
        : ui->comboxSettings->setCurrentIndex(0);

    connect(ui->buttonSave, &QPushButton::clicked,
            this, &WidgetLoadSaveBuilderSettings::saveSettings);

    QAction *actionSaveAsNew = m_saveButtonMenu->addAction(
                                   QStringLiteral("Save as new"), this,
                                   &WidgetLoadSaveBuilderSettings::saveNewSettings,
                                   QKeySequence{QStringLiteral("Ctrl+Shift+S")});
    addAction(actionSaveAsNew); // to enable shortcut
}

WidgetLoadSaveBuilderSettings::~WidgetLoadSaveBuilderSettings()
{
    delete ui;
}

void WidgetLoadSaveBuilderSettings::saveLatestSettings() const
{
    QSettings qSet{Application::settingsIniPath(QString{lastSettingsFileName}),
                QSettings::IniFormat};

    m_settingsModel->saveSettings(&qSet);
}

void WidgetLoadSaveBuilderSettings::loadSettings(int comboBoxIndex)
{
    QVariant &&iniFileNameData = ui->comboxSettings->itemData(comboBoxIndex);

    if (!iniFileNameData.isValid()) {
        m_settingsModel->clearSettings();
        return;
    }

    QSettings qSet{Application::settingsIniPath(iniFileNameData.toString()), QSettings::IniFormat};

    m_settingsModel->loadSettings(&qSet);
}

void WidgetLoadSaveBuilderSettings::saveSettings() const
{
    QString iniFileName = ui->comboxSettings->currentData().toString();

    if (iniFileName.isEmpty() || iniFileName == lastSettingsFileName) {
        saveNewSettings();
        return;
    } else {
        m_saveButtonMenu->popup(QCursor::pos());
    }

//    if (iniFileName.isEmpty())
//        return;

//    QSettings qSet{Application::settingsIniPath(iniFileName), QSettings::IniFormat};

//    m_settingsModel->saveSettings(&qSet);
}

void WidgetLoadSaveBuilderSettings::saveNewSettings() const
{
    const QString iniFileName = requestNewIniName();

    if (iniFileName.isEmpty())
        return;

    const QString iniFilePath = Application::settingsIniPath(iniFileName);

    if (QFileInfo::exists(iniFilePath)) {
        const QString text{QStringLiteral("%1 is already exists. Overwrite?")
                    .arg(iniFileName.chopped(4))};
        QMessageBox::StandardButton button
                = QMessageBox::question(nullptr, QStringLiteral("Confirm overwriting"), text);

        if (button == QMessageBox::No)
            return;
    }

    QSettings qSet{iniFilePath, QSettings::IniFormat};

    m_settingsModel->saveSettings(&qSet);
}
