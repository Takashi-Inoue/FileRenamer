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

#include "dialogbuildersettings.h"
#include "ui_dialogbuildersettings.h"

#include "abstractstringbuilderwidget.h"

#include <QDialogButtonBox>
#include <QTimer>

namespace StringBuilder {

DialogBuilderSettings::DialogBuilderSettings(QList<AbstractWidget *> widgets,
                                             QList<int> showIndexes, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DialogBuilderSettings),
      m_timer(new QTimer{this}),
      m_widgets(widgets)
{
    ui->setupUi(this);

    for (int i = 0; AbstractWidget *widget : widgets) {
        if (showIndexes.contains(i)) {
            ui->tabWidget->addTab(widget, widget->windowTitle());

            connect(widget, &AbstractWidget::changeStarted,
                    this, &DialogBuilderSettings::notifyStartChanging);
        }
        ++i;
    }

    m_timer->setSingleShot(true);
    m_timer->setInterval(QApplication::keyboardInputInterval());

    connect(m_timer, &QTimer::timeout, this, [this]() {
        auto builderChain = SharedBuilderChainOnFile::create();

        for (AbstractWidget *widget : m_widgets)
            builderChain->addBuilder(widget->stringBuilder());

        emit settingsChanged(builderChain);
    });

    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &DialogBuilderSettings::acceptSettingsWidgets);
}

DialogBuilderSettings::~DialogBuilderSettings()
{
    delete ui;
}

void DialogBuilderSettings::acceptSettingsWidgets()
{
    for (AbstractWidget *widget : m_widgets)
        widget->accept();
}

void DialogBuilderSettings::notifyStartChanging()
{
    m_timer->start();

    emit changeStarted();
}

} // namespace StringBuilder
