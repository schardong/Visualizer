QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11 -MMD

TARGET = CTInterface
TEMPLATE = app

SOURCES += \
    viewport.cpp \
    multidimrenderer.cpp \
    mainwindow.cpp \
    main.cpp

HEADERS  += \
    mainwindow.h \
    multidimrenderer.h \
    viewport.h

OTHER_FILES += \
    firstPass.frag \
    firstPass.vert \
    secondPassMultiTf.frag \
    secondPass.vert

INCLUDEPATH += ../include

LIBS += -L$$OUT_PWD/../Kernel -lggraf
LIBS += -ldl

config.path = $$OUT_PWD/
config.files = $$OTHER_FILES

INSTALLS += config

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
