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

#pragma once

#include "widgetfilehashsetting.h"
#include "widgetimagehashsetting.h"
#include "widgetinserttextsetting.h"
#include "widgetnumbersetting.h"
#include "widgetoriginalnamesetting.h"
#include "widgetreplacesetting.h"

#include "usingstringbuilderwidget.h"

using WidgetFileHash = StringBuilder::WidgetFileHashSetting;
using WidgetImageHash = StringBuilder::WidgetImageHashSetting;
using WidgetInsertText = StringBuilder::WidgetInsertTextSetting;
using WidgetNumber = StringBuilder::WidgetNumberSetting;
using WidgetOriginalName = StringBuilder::WidgetOriginalNameSetting;
using WidgetReplace = StringBuilder::WidgetReplaceSetting;


BuilderWidgetList allStringBuildersWidgets()
{
    BuilderWidgetList widgets;

    widgets.append(QSharedPointer<WidgetOriginalName>::create());
    widgets.append(QSharedPointer<WidgetInsertText>::create());
    widgets.append(QSharedPointer<WidgetReplace>::create());
    widgets.append(QSharedPointer<WidgetNumber>::create());
    widgets.append(QSharedPointer<WidgetFileHash>::create());
    widgets.append(QSharedPointer<WidgetImageHash>::create());

    return widgets;
}
