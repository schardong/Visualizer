#-------------------------------------------------
#
# Project created by QtCreator 2013-11-10T03:41:04
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Interface
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    viewport.cpp \
    gl3w.c \
    renderer.cpp \
    volumedata.cpp \
    shader.cpp \
    resources.cpp \
    resourcemanager.cpp \
    eventmanager.cpp

HEADERS  += mainwindow.h \
    viewport.h \
    scene.h \
    renderer.h \
    volumedata.h \
    singleton.h \
    shader.h \
    resources.h \
    resourcemanager.h \
    eventmanager.h

OTHER_FILES += \
    secondPass.vert \
    secondPass.frag \
    firstPass.vert \
    firstPass.frag

FORMS    +=

INCLUDEPATH += include

config.path = $$OUT_PWD/
config.files = $$OTHER_FILES

INSTALLS += config

LIBS += -ldl
