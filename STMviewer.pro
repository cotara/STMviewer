#-------------------------------------------------
#
# Project created by QtCreator 2021-04-15T15:43:06
#
#-------------------------------------------------

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = STMviewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    centerviewer.cpp \
    console.cpp \
    firfilter.cpp \
        main.cpp \
        mainwindow.cpp \
    serialsettings.cpp \
    qcustomplot/qcustomplot.cpp \
    shotviewer.cpp \
	statusbar.cpp \
	transp.cpp \
	slip.cpp \

HEADERS += \
    centerviewer.h \
    console.h \
    firfilter.h \
        mainwindow.h \
    serialsettings.h \
    qcustomplot/qcustomplot.h \
    shotviewer.h \
	statusbar.h \
	transp.h \
	slip.h \
	types.h \

FORMS += \
        mainwindow.ui \
        serialsettings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
