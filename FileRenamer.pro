QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    application.cpp \
    applicationlog/applicationlog.cpp \
    applicationlog/debuglog.cpp \
    applicationlog/logdata.cpp \
    filenamevlidator.cpp \
    imagehash/imagehashcalculator.cpp \
    path/parentdir.cpp \
    path/pathentity.cpp \
    path/pathentityinfo.cpp \
    path/pathheaderview.cpp \
    path/pathmodel.cpp \
    path/pathroot.cpp \
    path/pathtableview.cpp \
    path/pathtableviewmenu.cpp \
    pathsanalyzer.cpp \
    renamestate/renamestateinitial.cpp \
    searchindirs.cpp \
    stringbuilder/builderchain.cpp \
    stringbuilder/insertstring.cpp \
    stringbuilder/number.cpp \
    stringbuilder/regexpreplace.cpp \
    stringbuilder/replacestring.cpp \
    stringbuilderonfile/builderchainonfile.cpp \
    stringbuilderonfile/cryptographichash.cpp \
    stringbuilderonfile/imagehash.cpp \
    stringbuilderonfile/originalname.cpp \
    stringbuilderwidgetfactory.cpp \
    threadcreatenewnames.cpp \
    threadrename.cpp \
    threadundorenaming.cpp \
    main.cpp \
    mainwindow.cpp \
    widgets/counterlabel.cpp \
    widgets/dialogdroppeddir.cpp \
    widgets/dialogloadrenamesettings.cpp \
    widgets/dialogsaverenamesettings.cpp \
    widgets/elidelabel.cpp \
    widgets/formstringbuilder.cpp \
    widgets/formstringbuilderchain.cpp \
    widgets/historycombobox.cpp \
    widgets/savedsettingslistwidget.cpp \
    widgets/widgetapplicationlogs.cpp \
    widgets/widgetfilehashsetting.cpp \
    widgets/widgetimagehashsetting.cpp \
    widgets/widgetinserttextsetting.cpp \
    widgets/widgetnumbersetting.cpp \
    widgets/widgetonlypositionfixer.cpp \
    widgets/widgetoriginalnamesetting.cpp \
    widgets/widgetpositionfixer.cpp \
    widgets/widgetreplacesetting.cpp

HEADERS += \
    application.h \
    applicationlog/applicationlog.h \
    applicationlog/debuglog.h \
    applicationlog/logdata.h \
    filenamevlidator.h \
    imagehash/imagehashcalculator.h \
    mainwindow.h \
    path/parentdir.h \
    path/pathentity.h \
    path/pathentityinfo.h \
    path/pathheaderview.h \
    path/pathmodel.h \
    path/pathroot.h \
    path/pathtableview.h \
    path/pathtableviewmenu.h \
    path/usingpathentity.h \
    pathsanalyzer.h \
    renamestate/renamestateistate.h \
    renamestate/renamestateinitial.h \
    searchindirs.h \
    stringbuilder/abstractinsertstring.h \
    stringbuilder/abstractstringbuilder.h \
    stringbuilder/builderchain.h \
    stringbuilder/insertstring.h \
    stringbuilder/number.h \
    stringbuilder/regexpreplace.h \
    stringbuilder/replacestring.h \
    stringbuilderonfile/abstractneedfileinfo.h \
    stringbuilderonfile/builderchainonfile.h \
    stringbuilderonfile/cryptographichash.h \
    stringbuilderonfile/ifileinfo.h \
    stringbuilderonfile/imagehash.h \
    stringbuilderonfile/originalname.h \
    stringbuilderwidgetfactory.h \
    threadcreatenewnames.h \
    threadrename.h \
    threadundorenaming.h \
    widgets/abstractstringbuilderwidget.h \
    widgets/counterlabel.h \
    widgets/dialogdroppeddir.h \
    widgets/dialogloadrenamesettings.h \
    widgets/dialogsaverenamesettings.h \
    widgets/elidelabel.h \
    widgets/formstringbuilder.h \
    widgets/formstringbuilderchain.h \
    widgets/historycombobox.h \
    widgets/savedsettingslistwidget.h \
    widgets/widgetapplicationlogs.h \
    widgets/widgetfilehashsetting.h \
    widgets/widgetimagehashsetting.h \
    widgets/widgetinserttextsetting.h \
    widgets/widgetnumbersetting.h \
    widgets/widgetonlypositionfixer.h \
    widgets/widgetoriginalnamesetting.h \
    widgets/widgetpositionfixer.h \
    widgets/widgetreplacesetting.h

FORMS += \
    mainwindow.ui \
    widgets/dialogdroppeddir.ui \
    widgets/dialogloadrenamesettings.ui \
    widgets/dialogsaverenamesettings.ui \
    widgets/formstringbuilder.ui \
    widgets/formstringbuilderchain.ui \
    widgets/widgetapplicationlogs.ui \
    widgets/widgetfilehashsetting.ui \
    widgets/widgetinserttextsetting.ui \
    widgets/widgetnumbersetting.ui \
    widgets/widgetonlypositionfixer.ui \
    widgets/widgetpositionfixer.ui \
    widgets/widgetreplacesetting.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

VERSION = 1.3.0.0
RC_ICONS = res/images/filerenamer.ico
QMAKE_TARGET_PRODUCT = FileRenamer
QMAKE_TARGET_COPYRIGHT = "Copyright 2021-2022 Takashi Inoue"
