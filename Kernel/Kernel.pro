#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T00:35:17
#
#-------------------------------------------------

QT       += opengl

QT       -= core gui

QMAKE_CXXFLAGS += -std=c++11 -MMD

TARGET = ggraf
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    volumedata.cpp \
    multidimvolumedata.cpp\
    shader.cpp \
    resourcemanager.cpp \
    gl3w.c \
    kernel.cpp \
    logger.cpp

HEADERS += \
    volumedata.h \
    multidimvolumedata.h \
    singleton.h \
    shader.h \
    scene.h \
    resourcemanager.h \
    kernel.h \
    logger.h

INCLUDEPATH += ../include

header.path = ../include
header.files = $$HEADERS

INSTALLS += header

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -g0 -O2
    message(Release)
    message($$QMAKE_CXXFLAGS)
    message($$LIBS)
}

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g3 -pg -O0
    message(Debug)
    message($$QMAKE_CXXFLAGS)
    message($$LIBS)
}
