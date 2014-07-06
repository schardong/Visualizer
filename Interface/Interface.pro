#-------------------------------------------------
#
# Project created by QtCreator 2013-11-10T03:41:04
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11 -MMD

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

LIBS += -ldl

config.path = $$OUT_PWD/
config.files = $$OTHER_FILES

INSTALLS += config

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -g0 -O2
    LIBS += -L$$OUT_PWD/../../Kernel/Release -lKernel
    message(Release)
    message($$QMAKE_CXXFLAGS)
    message($$LIBS)
}

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g3 -pg -O0
    LIBS += -L$$OUT_PWD/../../Kernel/Debug -lKernel
    message(Debug)
    message($$QMAKE_CXXFLAGS)
    message($$LIBS)
}
