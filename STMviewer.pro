#-------------------------------------------------
#
# Project created by QtCreator 2021-04-15T15:43:06
#
#-------------------------------------------------

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

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
    asynchronbutton.cpp \
    autofindwizard.cpp \
    catchdatadialog.cpp \
    centerviewer.cpp \
    console.cpp \
    diametertransmition.cpp \
    entervaluewidget.cpp \
    firfilter.cpp \
    historysettings.cpp \
        main.cpp \
    maincontrolwidget.cpp \
        mainwindow.cpp \
    managementwidget.cpp \
    plissettings.cpp \
    resultswidget.cpp \
    savelog.cpp \
    serialsettings.cpp \
    qcustomplot/qcustomplot.cpp \
    settingsshadowsfinddialog.cpp \
    shotviewer.cpp \
    signalerrwidget.cpp \
	statusbar.cpp \
    transmitionsettings.cpp \
	transp.cpp \
	slip.cpp \

HEADERS += \
    asynchronbutton.h \
    autofindwizard.h \
    catchdatadialog.h \
    centerviewer.h \
    console.h \
    diametertransmition.h \
    entervaluewidget.h \
    firfilter.h \
    historysettings.h \
    maincontrolwidget.h \
        mainwindow.h \
    managementwidget.h \
    plissettings.h \
    resultswidget.h \
    savelog.h \
    serialsettings.h \
    qcustomplot/qcustomplot.h \
    settingsshadowsfinddialog.h \
    shotviewer.h \
    signalerrwidget.h \
	statusbar.h \
    transmitionsettings.h \
	transp.h \
	slip.h \
	types.h \

FORMS += \
        autofindwizard.ui \
        catchdatadialog.ui \
        entervaluewidget.ui \
        mainwindow.ui \
        savelog.ui \
        serialsettings.ui \
        settingsshadowsfinddialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES +=

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS +=  -fopenmp
LIBS += -fopenmp
#LIBS += -lgomp -lpthread
