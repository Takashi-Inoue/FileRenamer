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

#include "builderchainonfile.h"
#include "abstractneedfileinfo.h"
#include "path/pathentity.h"

namespace StringBuilderOnFile {

void BuilderChainOnFile::addBuilder(QSharedPointer<StringBuilder::AbstractStringBuilder> builder)
{
    auto needFileInfo = qobject_cast<AbstractNeedFileInfo *>(builder.get());

    if (needFileInfo != nullptr) {
        connect(needFileInfo, &AbstractNeedFileInfo::needFileInfo,
                this, &BuilderChainOnFile::onNeedFileInfo, Qt::DirectConnection);
    }

    BuilderChain::addBuilder(builder);
}

void BuilderChainOnFile::setFileInfo(QSharedPointer<IFileInfo> fileInfo)
{
    if (m_fileInfo != fileInfo)
        m_fileInfo = fileInfo;
}

void BuilderChainOnFile::onNeedFileInfo(AbstractNeedFileInfo *stringBuilder)
{
    Q_ASSERT(m_fileInfo != nullptr);

    stringBuilder->setFileInfo(m_fileInfo);
}

} // StringBuilderOnFile
