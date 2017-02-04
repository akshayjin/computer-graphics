#-------------------------------------------------
#
# Project created by QtCreator 2016-04-04T01:14:27
#
#-------------------------------------------------

QT       += core gui opengl
LIBS     += -lGL -lGLU -lm

CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Assignment5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myglwidget.cpp \
    model.cpp \
    plymodel.cpp \
    trackball.cpp \
    scenenode.cpp \
    cuberobot.cpp

HEADERS  += mainwindow.h \
    myglwidget.h \
    model.h \
    plymodel.h \
    trackball.h \
    Vector3.h \
    Vector4.h \
    scenenode.h \
    cuberobot.h

FORMS    += mainwindow.ui

OTHER_FILES +=
