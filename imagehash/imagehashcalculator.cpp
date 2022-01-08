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

#include "imagehashcalculator.h"

#include <QImage>
#include <QDebug>

ImageHashCalculator::ImageHashCalculator(QStringView filePath)
    : m_filePath(filePath.toString())
{
}

QString ImageHashCalculator::resultString()
{
    QImage imageOrigin{m_filePath};

    if (imageOrigin.isNull())
        return QString{};

    QImage image = imageOrigin.scaled(9, 8).convertToFormat(QImage::Format_Grayscale8);

    QString result;
    QString binary;

    for (int y = 0; y < 8; ++y) {
        for (int margin = 0; margin < 5; margin += 4) {
            for (int x = 0; x < 4; ++x) {
                int lhs = qGray(image.pixel(x + margin, y));
                int rhs = qGray(image.pixel(x + margin + 1, y));

                binary += (lhs < rhs) ? "0" : "1";
            }

            int fourBit = binary.toInt(nullptr, 2);

            result += QString::number(fourBit, 16);
            binary.clear();
        }
    }

    return result;
}
