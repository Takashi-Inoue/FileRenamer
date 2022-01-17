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
#include "savedsettingsmodel.h"
#include "stringbuilder/stringbuilderchainmodel.h"

#include <QDir>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

namespace {

constexpr char propertyCurrentText[] = "propertyCurrentText";

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
        StringBuilder::BuilderChainModel *builderChainModel, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WidgetLoadSaveBuilderSettings),
      m_settingsModel(builderChainModel),
      m_settingsListModel(new SavedSettingsModel{this})
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
        DialogSettingsListConfigurator(m_settingsListModel, this).exec();
    });

    addActions({m_actionSave, actionSaveAs}); // to enable shortcuts

    ui->comboxSettings->setModel(m_settingsListModel);

    connect(ui->comboxSettings, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        if (m_settingsListModel->isNewSettings(text)) {
            m_settingsModel->clearSettings();
            return;
        }

        const int index = ui->comboxSettings->currentIndex();

        m_settingsModel->loadSettings(m_settingsListModel->qSettings(index).get());
    });
    connect(ui->comboxSettings, &QComboBox::currentIndexChanged, this, [this](int index) {
        m_actionSave->setEnabled(m_settingsListModel->isEditable(index));
    });

    connect(m_settingsListModel, &QAbstractItemModel::modelAboutToBeReset, this, [this]() {
        ui->comboxSettings->setProperty(propertyCurrentText, ui->comboxSettings->currentText());
    });
    connect(m_settingsListModel, &QAbstractItemModel::modelReset, this, [this]() {
        QVariant value = ui->comboxSettings->property(propertyCurrentText);
        if (value.isValid()) {
            ui->comboxSettings->setCurrentText(value.toString());
            ui->comboxSettings->setProperty(propertyCurrentText, QVariant{});
        }
    });

    const int rowOfLastTime = m_settingsListModel->rowForLastTimeSetting();

    (rowOfLastTime != -1) ? ui->comboxSettings->setCurrentIndex(rowOfLastTime)
                          : ui->comboxSettings->setCurrentIndex(0);

    ui->buttonConfig->setMenu(configMenu);
}

WidgetLoadSaveBuilderSettings::~WidgetLoadSaveBuilderSettings()
{
    delete ui;
}

QWidget *WidgetLoadSaveBuilderSettings::lastTabOrderWidget() const
{
    return ui->buttonConfig;
}

void WidgetLoadSaveBuilderSettings::saveLastUsedSettings() const
{
    m_settingsListModel->saveAsLastUsed(m_settingsModel);
}

void WidgetLoadSaveBuilderSettings::saveLastTimeSettings() const
{
    m_settingsListModel->saveAsLastTime(m_settingsModel);
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

    ui->comboxSettings->blockSignals(true);
    int insertedIndex = m_settingsListModel->insertNewSettings(iniFileName.chopped(4));
    ui->comboxSettings->blockSignals(false);

    ui->comboxSettings->setCurrentIndex(insertedIndex);
    m_actionSave->setEnabled(true);
}
