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

#include "cryptographichash.h"
#include "ifileinfo.h"
#include "stringbuilder/widgets/widgetfilehashsetting.h"
#include "utilityshtml.h"

#include <QFile>
#include <QMetaEnum>
#include <QSettings>

namespace StringBuilder {
namespace OnFile {

namespace Settings {
constexpr char groupName[] = "FileHash";
constexpr char keyAlgorithm[] = "keyAlgorithm";
} // Settings

CryptographicHash::CryptographicHash()
    : CryptographicHash(QCryptographicHash::Md5, 0, nullptr)
{
}

CryptographicHash::CryptographicHash(QCryptographicHash::Algorithm algorithm, int pos,
                                     QObject *parent)
    : AbstractNeedFileInfo{pos, parent},
      m_algorithm{algorithm}
{
}

void CryptographicHash::build(QString &result)
{
    emit needFileInfo(this);

    QString hashHex = m_fileInfo->hashHex(m_algorithm);

    if (hashHex.isEmpty()) {
        QFile file(m_fileInfo->fullPath());

        if (!file.open(QFile::ReadOnly))
            return;

        QCryptographicHash hash(m_algorithm);

        if (!hash.addData(&file))
            return;

        hashHex = hash.result().toHex();

        m_fileInfo->setHashHex(m_algorithm, hashHex);
    }

    result.insert(actualInsertPosition(result.size()), hashHex);
}

QString CryptographicHash::toHtmlString() const
{
    auto metaEnum = QMetaEnum::fromType<QCryptographicHash::Algorithm>();

    const char *algorithmName = metaEnum.valueToKey(m_algorithm);

    if (isLeftMost())
        return Html::leftAligned(QStringLiteral("&lt;&lt; <b>%1</b>").arg(algorithmName));

    if (isRightMost())
        return Html::rightAligned(QStringLiteral("<b>%1</b> &gt;&gt;").arg(algorithmName));

    return Html::leftAligned(
                QStringLiteral("__%1 <b>%2</b>").arg(insertPosition()).arg(algorithmName));
}

AbstractWidget *CryptographicHash::settingsWidget()
{
    auto widget = new WidgetFileHashSetting(m_algorithm, insertPosition());

    connect(widget, &AbstractWidget::accepted, this, [&, this]() {
        auto settingsWidget = qobject_cast<WidgetFileHashSetting *>(sender());

        m_algorithm = settingsWidget->algorithm();
        setInsertPosition(settingsWidget->insertPosition());
    });

    return widget;
}

void CryptographicHash::loadSettings(QSettings *qSet)
{
    qSet->beginGroup(Settings::groupName);

    QVariant value = qSet->value(Settings::keyAlgorithm, int(QCryptographicHash::Md5)).toInt();

    m_algorithm = value.value<QCryptographicHash::Algorithm>();

    AbstractInsertString::loadSettings(qSet);

    qSet->endGroup();
}

void CryptographicHash::saveSettings(QSettings *qSet) const
{
    qSet->beginGroup(Settings::groupName);

    qSet->setValue(Settings::keyAlgorithm, int(m_algorithm));
    AbstractInsertString::saveSettings(qSet);

    qSet->endGroup();
}

} // OnFile
} // StringBuilder
