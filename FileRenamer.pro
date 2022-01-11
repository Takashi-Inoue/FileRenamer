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
    filenamevalidator.cpp \
    htmltextdelegate.cpp \
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
    stringbuilder/onfile/builderchainonfile.cpp \
    stringbuilder/onfile/cryptographichash.cpp \
    stringbuilder/onfile/imagehash.cpp \
    stringbuilder/onfile/originalname.cpp \
    stringbuilder/replacestring.cpp \
    stringbuilder/stringbuilderfactory.cpp \
    stringbuilder/stringbuildersettingsmodel.cpp \
    stringbuilder/stringbuildersmodel.cpp \
    stringbuilder/widgets/dialogbuildersettings.cpp \
    stringbuilder/widgets/widgetfilehashsetting.cpp \
    stringbuilder/widgets/widgetimagehashsetting.cpp \
    stringbuilder/widgets/widgetinserttextsetting.cpp \
    stringbuilder/widgets/widgetnumbersetting.cpp \
    stringbuilder/widgets/widgetonlypositionfixer.cpp \
    stringbuilder/widgets/widgetoriginalnamesetting.cpp \
    stringbuilder/widgets/widgetpositionfixer.cpp \
    stringbuilder/widgets/widgetreplacesetting.cpp \
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
    widgets/framebuilderlist.cpp \
    widgets/historycombobox.cpp \
    widgets/savedsettingslistwidget.cpp \
    widgets/widgetapplicationlogs.cpp

HEADERS += \
    application.h \
    applicationlog/applicationlog.h \
    applicationlog/debuglog.h \
    applicationlog/logdata.h \
    filenamevalidator.h \
    htmltextdelegate.h \
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
    stringbuilder/onfile/abstractneedfileinfo.h \
    stringbuilder/onfile/builderchainonfile.h \
    stringbuilder/onfile/cryptographichash.h \
    stringbuilder/onfile/ifileinfo.h \
    stringbuilder/onfile/imagehash.h \
    stringbuilder/onfile/originalname.h \
    stringbuilder/replacestring.h \
    stringbuilder/stringbuilderfactory.h \
    stringbuilder/stringbuildersettingsmodel.h \
    stringbuilder/stringbuildersmodel.h \
    stringbuilder/widgets/abstractstringbuilderwidget.h \
    stringbuilder/widgets/dialogbuildersettings.h \
    stringbuilder/widgets/widgetfilehashsetting.h \
    stringbuilder/widgets/widgetimagehashsetting.h \
    stringbuilder/widgets/widgetinserttextsetting.h \
    stringbuilder/widgets/widgetnumbersetting.h \
    stringbuilder/widgets/widgetonlypositionfixer.h \
    stringbuilder/widgets/widgetoriginalnamesetting.h \
    stringbuilder/widgets/widgetpositionfixer.h \
    stringbuilder/widgets/widgetreplacesetting.h \
    threadcreatenewnames.h \
    threadrename.h \
    threadundorenaming.h \
    usingstringbuilder.h \
    usingstringbuilderwidget.h \
    widgets/counterlabel.h \
    widgets/dialogdroppeddir.h \
    widgets/dialogloadrenamesettings.h \
    widgets/dialogsaverenamesettings.h \
    widgets/elidelabel.h \
    widgets/framebuilderlist.h \
    widgets/historycombobox.h \
    widgets/savedsettingslistwidget.h \
    widgets/widgetapplicationlogs.h

FORMS += \
    mainwindow.ui \
    stringbuilder/widgets/dialogbuildersettings.ui \
    stringbuilder/widgets/widgetfilehashsetting.ui \
    stringbuilder/widgets/widgetinserttextsetting.ui \
    stringbuilder/widgets/widgetnumbersetting.ui \
    stringbuilder/widgets/widgetonlypositionfixer.ui \
    stringbuilder/widgets/widgetpositionfixer.ui \
    stringbuilder/widgets/widgetreplacesetting.ui \
    widgets/dialogdroppeddir.ui \
    widgets/dialogloadrenamesettings.ui \
    widgets/dialogsaverenamesettings.ui \
    widgets/framebuilderlist.ui \
    widgets/widgetapplicationlogs.ui

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
