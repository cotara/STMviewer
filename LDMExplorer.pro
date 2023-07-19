#-------------------------------------------------
#
# Project created by QtCreator 2021-04-15T15:43:06
#
#-------------------------------------------------

QT       += core gui serialport printsupport
QMAKE_LFLAGS = -static -static-libgcc
greaterThan(QT_MAJOR_VERSION, 5): QT += widgets


EXTRA_QMAKE_TARGET_PRODUCT =  LDMExplorer
QMAKE_TARGET_DESCRIPTION = LDMExplorer (Client for LDM tuning)
QMAKE_TARGET_COPYRIGHT = NurRuslan
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
    console.cpp \
    firfilter.cpp \
    main.cpp \
    mainwindow.cpp \
    savelog.cpp \
    qcustomplot/qcustomplot.cpp \
    shotviewer.cpp \
    statusbar.cpp \

HEADERS += \
    console.h \
    fftw3.h \
    firfilter.h \
    mainwindow.h \
    savelog.h \
    qcustomplot/qcustomplot.h \
    shotviewer.h \
    statusbar.h \
    types.h \

FORMS += \
        mainwindow.ui \
        savelog.ui \

include(managementPanel/managementPanel.pri)
include(controlPanel/controlPanel.pri)
include(shadowSettings/shadowSettings.pri)
include(transport/transport.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
RC_ICONS += icon.ico

DISTFILES +=

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS +=  -fopenmp
LIBS += -fopenmp
#LIBS += -lgomp -lpthread


INCLUDEPATH += "c:\fftw"
LIBS += "$$PWD/libfftw3-3.dll"

