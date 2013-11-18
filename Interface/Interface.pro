#-------------------------------------------------
#
# Project created by QtCreator 2013-11-10T03:41:04
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

TARGET = Interface
TEMPLATE = app

SOURCES += \
    viewport.cpp \
    renderer.cpp \
    mainwindow.cpp \
    main.cpp

HEADERS  += \
    viewport.h \
    renderer.h \
    mainwindow.h

OTHER_FILES += \
    secondPass.vert \
    secondPass.frag \
    firstPass.vert \
    firstPass.frag

INCLUDEPATH += ../include
INCLUDEPATH += ../VisKernel

config.path = $$OUT_PWD/
config.files = $$OTHER_FILES

INSTALLS += config

LIBS += -L$$OUT_PWD/../VisKernel -lVisKernel

LIBS += -ldl
