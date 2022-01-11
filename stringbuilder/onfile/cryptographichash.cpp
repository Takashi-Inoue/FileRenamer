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

#include <QFile>
#include <QMetaEnum>

namespace StringBuilder {
namespace OnFile {

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
        return QStringLiteral("<p align=\"left\">&lt;&lt; <b>%1</b></p>").arg(algorithmName);

    if (isRightMost())
        return QStringLiteral("<p align=\"right\"><b>%1</b> &gt;&gt;</p>").arg(algorithmName);

    return QStringLiteral("<p align=\"left\">__%1 <b>%2</b></p>").arg(insertPosition()).arg(algorithmName);
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

} // OnFile
} // StringBuilder
