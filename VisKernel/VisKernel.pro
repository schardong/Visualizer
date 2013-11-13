#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T00:35:17
#
#-------------------------------------------------

QT       += opengl

QT       -= core gui

TARGET = VisKernel
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    volumedata.cpp \
    shader.cpp \
    resourcemanager.cpp \
    gl3w.c \
    kernel.cpp

HEADERS += \
    volumedata.h \
    singleton.h \
    shader.h \
    scene.h \
    resourcemanager.h \
    kernel.h

INCLUDEPATH += ../include

header.path = ../include
header.files = $$HEADERS
#target.path = $$OUT_PWD

INSTALLS +=  header

#unix:!symbian {
#    maemo5 {
#        target.path = /opt/usr/lib
#    } else {
#        target.path = /usr/lib
#    }
#    INSTALLS += target
#}
