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

inline void addIniNameToComboBox(QComboBox *combox, QStringView completeIniFileName)
{
    Q_CHECK_PTR(combox);
    Q_ASSERT(completeIniFileName.right(4) == QString{".ini"});

    combox->addItem(completeIniFileName.chopped(4).toString(), completeIniFileName.toString());
}

inline void insertIniNameToComboBox(QComboBox *combox, int insertIndex,
                                    QStringView completeIniFileName)
{
    Q_CHECK_PTR(combox);
    Q_ASSERT(completeIniFileName.right(4) == QString{".ini"});

    combox->insertItem(insertIndex, completeIniFileName.chopped(4).toString(),
                       completeIniFileName.toString());
}

void initSettingsComboBox(QComboBox *combox)
{
    combox->addItem(QObject::tr("New settings"));

    QDir dir{Application::settingsDirPath()};

    if (!dir.exists())
        return;

    QStringList &&inis = dir.entryList({QStringLiteral("*.ini")}, QDir::Files, QDir::Name);
    qsizetype indexLastUsed = inis.indexOf(QString{lastSettingsFileName});

    if (indexLastUsed != -1)
        inis.removeAt(indexLastUsed);

    for (QStringView iniName : inis)
        addIniNameToComboBox(combox, iniName);

    if (indexLastUsed != -1)
        addIniNameToComboBox(combox, QString{lastSettingsFileName});
}

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

WidgetLoadSaveBuilderSettings::WidgetLoadSaveBuilderSettings(
        StringBuilder::SettingsModel *settingsModel, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WidgetLoadSaveBuilderSettings),
      m_settingsModel(settingsModel),
      m_saveButtonMenu(new QMenu{this})
{
    Q_CHECK_PTR(settingsModel);

    ui->setupUi(this);

    m_actionSave = m_saveButtonMenu->addAction(
                       tr("Save"), this, &WidgetLoadSaveBuilderSettings::saveOverwrite,
                       QKeySequence{QKeySequence::Save});
    QAction *actionSaveAs = m_saveButtonMenu->addAction(
                                tr("Save As"), this, &WidgetLoadSaveBuilderSettings::saveNewSettings,
                                QKeySequence{QStringLiteral("Ctrl+Shift+S")});
    addAction(m_actionSave); // to enable shortcut
    addAction(actionSaveAs);

    initSettingsComboBox(ui->comboxSettings);

    connect(ui->comboxSettings, SIGNAL(currentIndexChanged(int)), this, SLOT(loadSettings(int)));
    connect(ui->comboxSettings, &QComboBox::currentIndexChanged, this, [this](int index) {
        bool isEnabled = (index > 0) && (index < ui->comboxSettings->count() - 1);
        m_actionSave->setEnabled(isEnabled);
    });

    const int lastIndex = ui->comboxSettings->count() - 1;

    ui->comboxSettings->itemData(lastIndex) == QString{lastSettingsFileName}
        ? ui->comboxSettings->setCurrentIndex(lastIndex)
        : ui->comboxSettings->setCurrentIndex(0);

    connect(ui->buttonSave, &QPushButton::clicked,
            this, &WidgetLoadSaveBuilderSettings::saveSettings);
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

void WidgetLoadSaveBuilderSettings::saveOverwrite() const
{

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

    QStringList iniFiles;

    for (int i = 1, count = ui->comboxSettings->count() - 1; i < count; ++i)
        iniFiles.append(ui->comboxSettings->itemData(i).toString());

    int insertIndex = std::distance(iniFiles.begin(),
        std::upper_bound(iniFiles.begin(), iniFiles.end(), iniFileName)) + 1;

    insertIniNameToComboBox(ui->comboxSettings, insertIndex, iniFileName);

    ui->comboxSettings->blockSignals(true);
    ui->comboxSettings->setCurrentIndex(insertIndex);
    ui->comboxSettings->blockSignals(false);
}
