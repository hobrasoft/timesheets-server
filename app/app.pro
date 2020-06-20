CODECFORTR = UTF-8
TEMPLATE = app
TARGET = timesheet-server
DESTDIR = ../bin
INCLUDEPATH += ../src
LIBS += -ldl -L../bin -ltimesheet-server
#QMAKE_LFLAGS += -Wl,-rpath=$$PWD/../bin,--enable-new-dtags
QMAKE_LFLAGS += -Wl,-rpath=../bin,-rpath=bin,-rpath=/usr/lib/timesheet-server/bin,-rpath=$$PWD/../bin,--enable-new-dtags

PRE_TARGETDEPS += ../bin/libtimesheet-server.so

equals(QT_MAJOR_VERSION, 4) {
    LIBS += -lqjson
    }

CONFIG -= stl
CONFIG += qt warn_on thread

QT += network
QT -= gui

include(../global.pri)

SOURCES += main.cpp

equals(QT_ARCH, arm) {
  DEFINES += ARM
}

