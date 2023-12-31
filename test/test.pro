TEMPLATE = app
TARGET = test
DESTDIR = ../bin
INCLUDEPATH += ../src

CONFIG -= stl
CONFIG += qt warn_on thread testlib

QT += network testlib sql gui

include(../global.pri)

RESOURCES += \
 test.qrc \

SOURCES += \
 main.cpp \
 test.cpp \

HEADERS += \
 test.h \

# Testované zdrojáky
include(../src/utils/utils.pri)

SOURCES += \
 api.cpp \

HEADERS += \
 api.h \

