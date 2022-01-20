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

#include "application.h"
#include "abstractstringbuilderwidget.h"

#include <QDialogButtonBox>
#include <QTimer>

namespace StringBuilder {

inline SharedBuilderChainOnFile createBuilderChainOnWidgets(const QList<AbstractWidget *> &widgets)
{
    auto builderChain = SharedBuilderChainOnFile::create();

    for (AbstractWidget *widget : widgets)
        builderChain->addBuilder(widget->stringBuilder());

    return builderChain;
}

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

            connect(widget, SIGNAL(changeStarted()), m_timer, SLOT(start()));
            connect(widget, SIGNAL(changeStarted()), this, SIGNAL(changeStarted()));
        }
        ++i;
    }

    if (ui->tabWidget->count() > 0)
        static_cast<AbstractWidget *>(ui->tabWidget->widget(0))->setFocusToFirstWidget();

    m_timer->setSingleShot(true);
    m_timer->setInterval(QApplication::keyboardInputInterval());

    connect(m_timer, &QTimer::timeout, this, [this]() {
        emit settingsChanged(createBuilderChainOnWidgets(m_widgets));
    });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        for (AbstractWidget *widget : m_widgets)
            widget->accept();
    });
}

DialogBuilderSettings::~DialogBuilderSettings()
{
    delete ui;
}

} // namespace StringBuilder
