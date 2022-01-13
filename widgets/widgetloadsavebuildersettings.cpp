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
#include "dialogsettingslistconfigurator.h"
#include "filenamevalidator.h"
#include "renamesettingsmodel.h"
#include "stringbuilder/stringbuildersettingsmodel.h"

#include <QDir>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

namespace {

QString requestNewIniName()
{
    QInputDialog dlg;

    dlg.setInputMode(QInputDialog::TextInput);

    auto lineEdit = dlg.findChild<QLineEdit *>();

    if (lineEdit)
        lineEdit->setValidator(new FileNameVlidator{lineEdit});

    dlg.setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    dlg.setWindowTitle(QObject::tr("Save As"));
    dlg.setLabelText(QObject::tr("Settings name to save as:"));

    if (dlg.exec() == QDialog::Rejected)
        return QString{};

    return QStringLiteral("%1.ini").arg(dlg.textValue());
}

} // anonymous

//--------------------------------------------------------------------------------------------------

WidgetLoadSaveBuilderSettings::WidgetLoadSaveBuilderSettings(
        StringBuilder::SettingsModel *settingsModel, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WidgetLoadSaveBuilderSettings),
      m_settingsModel(settingsModel),
      m_settingsListModel(new RenameSettingsModel{this})
{
    Q_CHECK_PTR(settingsModel);

    ui->setupUi(this);

    auto configMenu = new QMenu{this};

    m_actionSave = configMenu->addAction(
                       tr("Save"), this, &WidgetLoadSaveBuilderSettings::saveOverwrite,
                       QKeySequence{QKeySequence::Save});
    QAction *actionSaveAs = configMenu->addAction(
                                tr("Save As"), this, &WidgetLoadSaveBuilderSettings::saveNewSettings,
                                QKeySequence{QStringLiteral("Ctrl+Shift+S")});
    configMenu->addSeparator();
    configMenu->addAction(tr("Edit List"), this, &WidgetLoadSaveBuilderSettings::showConfigureDialog);

    addActions({m_actionSave, actionSaveAs}); // to enable shortcut

    ui->comboxSettings->setModel(m_settingsListModel);

    connect(ui->comboxSettings, SIGNAL(currentIndexChanged(int)), this, SLOT(loadSettings(int)));
    connect(ui->comboxSettings, &QComboBox::currentIndexChanged, this, [this](int index) {
        bool isEnabled = (index > 0) && (index < ui->comboxSettings->count() - 1);
        m_actionSave->setEnabled(isEnabled);
    });

    const int lastIndex = ui->comboxSettings->count() - 1;

    m_settingsListModel->existsLastUsedSettings() ? ui->comboxSettings->setCurrentIndex(lastIndex)
                                                  : ui->comboxSettings->setCurrentIndex(0);

    ui->buttonConfig->setMenu(configMenu);
}

WidgetLoadSaveBuilderSettings::~WidgetLoadSaveBuilderSettings()
{
    delete ui;
}

void WidgetLoadSaveBuilderSettings::saveLatestSettings() const
{
    m_settingsModel->saveSettings(m_settingsListModel->qSettingsForLastUsed().get());
}

void WidgetLoadSaveBuilderSettings::loadSettings(int comboBoxIndex)
{
    if (comboBoxIndex == 0) {
        m_settingsModel->clearSettings();
        return;
    }

    m_settingsModel->loadSettings(m_settingsListModel->qSettings(comboBoxIndex).get());
}

void WidgetLoadSaveBuilderSettings::saveOverwrite() const
{
    const int comboBoxIndex = ui->comboxSettings->currentIndex();

    m_settingsModel->saveSettings(m_settingsListModel->qSettings(comboBoxIndex).get());
}

void WidgetLoadSaveBuilderSettings::saveNewSettings() const
{
    const QString iniFileName = requestNewIniName();

    if (iniFileName.isEmpty())
        return;

    const QString iniFilePath = Application::settingsIniPath(iniFileName);

    if (QFileInfo::exists(iniFilePath)) {
        const QString text{tr("%1 is already exists. Overwrite?").arg(iniFileName.chopped(4))};
        QMessageBox::StandardButton button
                = QMessageBox::question(nullptr, tr("Confirm overwriting"), text);

        if (button == QMessageBox::No)
            return;
    }

    QSettings qSet{iniFilePath, QSettings::IniFormat};

    m_settingsModel->saveSettings(&qSet);

    int insertedIndex = m_settingsListModel->insertNewSettings(iniFileName.chopped(4));

    ui->comboxSettings->blockSignals(true);
    ui->comboxSettings->setCurrentIndex(insertedIndex);
    ui->comboxSettings->blockSignals(false);
}

void WidgetLoadSaveBuilderSettings::showConfigureDialog()
{
    DialogSettingsListConfigurator dlg(m_settingsListModel, this);

    dlg.exec();
}
