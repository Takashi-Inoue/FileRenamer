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

#include "widgetfilehashsetting.h"
#include "ui_widgetfilehashsetting.h"

#include "stringbuilderonfile/cryptographichash.h"

namespace {
constexpr char settingsGroupName[] = "CryptographicHash";
constexpr char settingsKeyAlgorithm[] = "Algorithm";
}

WidgetFileHashSetting::WidgetFileHashSetting(QWidget *parent)
    : AbstractStringBuilderWidget(parent),
      ui(new Ui::WidgetFileHashSetting)
{
    ui->setupUi(this);

    using Algorithm = QCryptographicHash::Algorithm;

    ui->comboBoxHashType->addItem(QStringLiteral("SHA2-224"), Algorithm::Sha224);
    ui->comboBoxHashType->addItem(QStringLiteral("SHA2-256"), Algorithm::Sha256);
//    ui->comboBoxHashType->addItem(QStringLiteral("SHA2-512"), Algorithm::Sha512); // too long for filename
    ui->comboBoxHashType->addItem(QStringLiteral("SHA3-224"), Algorithm::Sha3_224);
    ui->comboBoxHashType->addItem(QStringLiteral("SHA3-256"), Algorithm::Sha3_256);
//    ui->comboBoxHashType->addItem(QStringLiteral("SHA3-512"), Algorithm::Sha3_512);
    ui->comboBoxHashType->addItem(QStringLiteral("MD5"), Algorithm::Md5);
    ui->comboBoxHashType->addItem(QStringLiteral("SHA1"), Algorithm::Sha1);

    connect(ui->comboBoxHashType, &QComboBox::currentIndexChanged,
            this, &AbstractStringBuilderWidget::changeStarted);

    connect(ui->widgetPositionFixer, &WidgetPositionFixer::changeStarted,
            this, &AbstractStringBuilderWidget::changeStarted);
}

WidgetFileHashSetting::~WidgetFileHashSetting()
{
    delete ui;
}

QSharedPointer<StringBuilder::AbstractStringBuilder> WidgetFileHashSetting::StringBuilder() const
{
    using Algorithm = QCryptographicHash::Algorithm;

    auto algorithm = Algorithm(ui->comboBoxHashType->currentData().toInt());

    return QSharedPointer<StringBuilderOnFile::CryptographicHash>::create(
                algorithm, ui->widgetPositionFixer->value());
}

void WidgetFileHashSetting::loadSettings(QSharedPointer<QSettings> qSettings)
{
    qSettings->beginGroup(settingsGroupName);

    QVariant algorithm = qSettings->value(settingsKeyAlgorithm, QCryptographicHash::Algorithm::Sha224);
    ui->widgetPositionFixer->loadSettings(qSettings);

    qSettings->endGroup();

    int index = ui->comboBoxHashType->findData(algorithm);

    if (index != -1)
        ui->comboBoxHashType->setCurrentIndex(index);
}

void WidgetFileHashSetting::saveSettings(QSharedPointer<QSettings> qSettings) const
{
    qSettings->beginGroup(settingsGroupName);

    qSettings->setValue(settingsKeyAlgorithm, ui->comboBoxHashType->currentData());
    ui->widgetPositionFixer->saveSettings(qSettings);

    qSettings->endGroup();
}
