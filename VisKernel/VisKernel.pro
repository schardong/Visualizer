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

SOURCES += viskernel.cpp \
    volumedata.cpp \
    shader.cpp \
    resources.cpp \
    resourcemanager.cpp \
    gl3w.c

HEADERS += viskernel.h \
    volumedata.h \
    singleton.h \
    shader.h \
    scene.h \
    resources.h \
    resourcemanager.h

INCLUDEPATH += ../include

header.path = ../include
header.files = $$HEADERS
target.path = $$OUT_PWD

INSTALLS +=  header

#unix:!symbian {
#    maemo5 {
#        target.path = /opt/usr/lib
#    } else {
#        target.path = /usr/lib
#    }
#    INSTALLS += target
#}
