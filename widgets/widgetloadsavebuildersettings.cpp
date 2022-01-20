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
#include "comboboxselectionkeeper.h"
#include "dialogsettingslistconfigurator.h"
#include "filenamevalidator.h"
#include "savedsettingsmodel.h"
#include "stringbuilder/stringbuilderchainmodel.h"
#include "stringbuilder/onfile/builderchainonfile.h"

#include <QDir>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

namespace {

constexpr char settingsKeyToLoad[] = "LoadFirst";

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

void loadSettingsToInit(QComboBox *comboxSettings, SavedSettingsModel *savedSettingsModel)
{
    const QString iniBaseName = Application::mainQSettings()->value(
                              settingsKeyToLoad, QString{}).toString();

    if (iniBaseName.isEmpty() || comboxSettings->findText(iniBaseName) == -1) {
        const int rowOfLastTime = savedSettingsModel->rowForLastTimeSetting();

        (rowOfLastTime != -1) ? comboxSettings->setCurrentIndex(rowOfLastTime)
                              : comboxSettings->setCurrentIndex(0);
    } else {
        comboxSettings->setCurrentText(iniBaseName);
    }
}

} // anonymous

//--------------------------------------------------------------------------------------------------

WidgetLoadSaveBuilderSettings::WidgetLoadSaveBuilderSettings(
        StringBuilder::BuilderChainModel *builderChainModel, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WidgetLoadSaveBuilderSettings),
      m_builderChainModel(builderChainModel),
      m_savedSettingsModel(new SavedSettingsModel{this})
{
    Q_CHECK_PTR(builderChainModel);

    ui->setupUi(this);

    auto configMenu = new QMenu{this};

    m_actionSave = configMenu->addAction(tr("Save"), this, SLOT(saveOverwrite()),
                                         QKeySequence{QKeySequence::Save});
    QAction *actionSaveAs = configMenu->addAction(tr("Save As"), this, SLOT(saveNewSettings()),
                                                  QKeySequence{QStringLiteral("Ctrl+Shift+S")});
    configMenu->addSeparator();
    configMenu->addAction(tr("Edit List"), this, [this]() {
        DialogSettingsListConfigurator(m_savedSettingsModel, this).exec();
    });

    ui->buttonConfig->setMenu(configMenu);
    addActions({m_actionSave, actionSaveAs}); // to enable shortcuts

    ui->comboxSettings->setModel(m_savedSettingsModel);

    connect(ui->comboxSettings, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        const int index = ui->comboxSettings->currentIndex();
        const bool isEditable = m_savedSettingsModel->isEditable(index);

        m_actionSave->setEnabled(isEditable);
        m_builderChainModel->loadSettings(m_savedSettingsModel->qSettings(index).get());
        m_iniBaseToLoadAtNextTime = isEditable ? text : QString{};
    });
    connect(m_builderChainModel, &StringBuilder::BuilderChainModel::settingsChanged, this, [this]() {
        m_iniBaseToLoadAtNextTime.clear();
    });

    auto comboxSelectionKeeper = new ComboBoxSelectionKeeper(ui->comboxSettings);
    comboxSelectionKeeper->setParent(this);

    loadSettingsToInit(ui->comboxSettings, m_savedSettingsModel);
}

WidgetLoadSaveBuilderSettings::~WidgetLoadSaveBuilderSettings()
{
    Application::mainQSettings()->setValue(settingsKeyToLoad, m_iniBaseToLoadAtNextTime);

    delete ui;
}

QWidget *WidgetLoadSaveBuilderSettings::lastTabOrderWidget() const
{
    return ui->buttonConfig;
}

void WidgetLoadSaveBuilderSettings::saveLastUsedSettings() const
{
    m_savedSettingsModel->saveAsLastUsed(m_builderChainModel);
}

void WidgetLoadSaveBuilderSettings::saveLastTimeSettings() const
{
    m_savedSettingsModel->saveAsLastTime(m_builderChainModel);
}

void WidgetLoadSaveBuilderSettings::saveOverwrite()
{
    const int comboBoxIndex = ui->comboxSettings->currentIndex();

    m_builderChainModel->saveSettings(m_savedSettingsModel->qSettings(comboBoxIndex).get());

    m_iniBaseToLoadAtNextTime = ui->comboxSettings->currentText();
}

void WidgetLoadSaveBuilderSettings::saveNewSettings()
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

    m_builderChainModel->saveSettings(&qSet);
    QStringView iniBaseName = iniFileName.chopped(4);

    ui->comboxSettings->blockSignals(true);
    int insertedIndex = m_savedSettingsModel->insertNewSettings(iniBaseName);
    ui->comboxSettings->setCurrentIndex(insertedIndex);
    ui->comboxSettings->blockSignals(false);

    m_actionSave->setEnabled(true);

    m_iniBaseToLoadAtNextTime = iniBaseName.toString();
}
