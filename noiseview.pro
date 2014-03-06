#-------------------------------------------------
#
# Project created by QtCreator 2014-02-01T00:19:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = noiseview
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        noiseutils.cpp

HEADERS  += mainwindow.h noiseutils.h

FORMS    += mainwindow.ui

unix|win32: LIBS += -L$$PWD/../../../../../libs/libnoise/bin/ -llibnoise

INCLUDEPATH += $$PWD/../../../../../libs/libnoise/include
DEPENDPATH += $$PWD/../../../../../libs/libnoise/include
