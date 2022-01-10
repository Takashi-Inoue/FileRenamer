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

#include "stringbuilderwidgetfactory.h"

#include "stringbuilder/widgets/widgetoriginalnamesetting.h"
#include "stringbuilder/widgets/widgetnumbersetting.h"
#include "stringbuilder/widgets/widgetreplacesetting.h"
#include "stringbuilder/widgets/widgetinserttextsetting.h"
#include "stringbuilder/widgets/widgetfilehashsetting.h"
#include "stringbuilder/widgets/widgetimagehashsetting.h"

StringBuilder::AbstractWidget *StringBuilderWidgetFactory::createWidget(
        int builderIndex, QWidget *parent) const
{
    // builderIndex links index of combobox in FormStringBuilder.
    switch (builderIndex) {
    case 0:
        return new StringBuilder::WidgetOriginalNameSetting(parent);

    case 1:
        return new StringBuilder::WidgetNumberSetting(parent);

    case 2:
        return new StringBuilder::WidgetReplaceSetting(parent);

    case 3:
        return new StringBuilder::WidgetInsertTextSetting(parent);

    case 4:
        return new StringBuilder::WidgetFileHashSetting(parent);

    case 5:
        return new StringBuilder::WidgetImageHashSetting(parent);
    }

    Q_ASSERT(false);

    return nullptr;
}
