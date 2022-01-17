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

#include "mainwindow.h"
#include "application.h"
#include "applicationlog/applicationlog.h"
#include "applicationlog/debuglog.h"

#include <QApplication>
#include <QStyleFactory>
//#include <QLibraryInfo>
//#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QCoreApplication::setApplicationName("FileRenamer");
    QCoreApplication::setApplicationVersion("1.4.0.0");

    Application::loadMainSettings();

//    const QString localName{QLocale::system().name()};

//    if (QTranslator translator;
//            translator.load("qt_" + localName, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
//        a.installTranslator(&translator);
//    }

//    if (QTranslator translator;
//            translator.load("fr_" + localName, QApplication::applicationDirPath() + "/translations")) {
//        a.installTranslator(&translator);
//    }

    DebugLog::init();

    MainWindow w;
    w.show();

    int result = a.exec();

    Application::saveMainSettings();

    ApplicationLog::instance().writeFile();
    DebugLog::writeFile();

    return result;
}
