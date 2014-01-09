TARGET = ContourTree
#TEMPLATE = lib
TEMPLATE = app
#CONFIG += staticlib

SOURCES += main.cpp \
    ctfunc.cpp \
    data.cpp \
    mesh.cpp \
    simplification.cpp \
    contourtree.cpp

HEADERS += \
    ctfunc.h \
    data.h \
    featureset.h \
    global.h \
    simplification.h \
    mesh.h \
    contourtree.h

QMAKE_CXXFLAGS += -std=c++11 -DUSE_ZLIB -MMD

INCLUDEPATH += ../include

LIBS += -lz
LIBS += -ltourtre

header.path = ../include
header.files = $$HEADERS

INSTALLS += header

CONFIG(release, debug|release) {
    LIBS += -ltbb -ltbbmalloc_proxy
    QMAKE_CXXFLAGS += -g0 -O2
    message(Release)
    message($$QMAKE_CXXFLAGS)
    message($$LIBS)
}

CONFIG(debug, debug|release) {
    LIBS += -ltbb_debug -ltbbmalloc_proxy_debug
    QMAKE_CXXFLAGS += -g3 -pg -O0
    message(Debug)
    message($$QMAKE_CXXFLAGS)
    message($$LIBS)
}
