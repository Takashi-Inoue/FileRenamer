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

#include "dialogsaverenamesettings.h"
#include "ui_dialogsaverenamesettings.h"

#include <QCloseEvent>
#include <QMessageBox>
#include <QPushButton>

DialogSaveRenameSettings::DialogSaveRenameSettings(QWidget *parent)
    : QDialog{parent},
      ui{new Ui::DialogSaveRenameSettings}
{
    ui->setupUi(this);

    ui->lineEdit->setFocus();
    ui->lineEdit->selectAll();
}

DialogSaveRenameSettings::~DialogSaveRenameSettings()
{
    delete ui;
}

QString DialogSaveRenameSettings::newSettingFullpath() const
{
    return QString("%1/%2.ini").arg(ui->listWidget->settingsDir(), m_newSettingName);
}

void DialogSaveRenameSettings::accept()
{
    m_newSettingName = ui->lineEdit->text();

    if (ui->listWidget->findItems(m_newSettingName, Qt::MatchFixedString).isEmpty()) {
        QDialog::accept();

        return;
    }

    QString msg = tr("<b>%1</b> already exists.<br>").arg(m_newSettingName)
                + tr("Would you like to replace existing setting?");

    if (QMessageBox::question(this, tr("Confirm Replace"), msg) != QMessageBox::Yes)
        return;

    QDialog::accept();
}

void DialogSaveRenameSettings::onLineEditTextChanged(const QString &text)
{
    ui->buttonOK->setEnabled(!text.isEmpty());
}

void DialogSaveRenameSettings::onListWidgetCurrentTextChanged(const QString &currentText)
{
    if (!currentText.isEmpty())
        ui->lineEdit->setText(currentText);
}

void DialogSaveRenameSettings::onListWidgetItemPressed(QListWidgetItem *item)
{
    ui->lineEdit->setText(item->text());
}
