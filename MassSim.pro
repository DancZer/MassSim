#-------------------------------------------------
#
# Project created by QtCreator 2012-10-31T14:21:09
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MassSim
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    worldframe.cpp \
    worldrenderthread.cpp \
    object.cpp \
    sphere.cpp

HEADERS  += mainwindow.h \
    worldframe.h \
    worldrenderthread.h \
    object.h \
    sphere.h \
    mathvector.h

FORMS    += mainwindow.ui
