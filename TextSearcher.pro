#-------------------------------------------------
#
# Project created by QtCreator 2014-07-03T17:35:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TextSearcher
TEMPLATE = app


SOURCES += main.cpp\
        mainscreen.cpp \
    textsearchworker.cpp \
    worker.cpp \
    findfileworker.cpp \
    workersthreadscontroller.cpp

HEADERS  += mainscreen.h \
    worker.h \
    textsearchworker.h \
    common.h \
    findfileworker.h \
    workersthreadscontroller.h

FORMS    += mainscreen.ui
