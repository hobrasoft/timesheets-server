TRANSLATIONS = \

CODECFORTR = UTF-8
TEMPLATE = lib
TARGET = timesheet-server
DESTDIR = ../bin
LIBS += -ldl
VERSION = 1.0.0

equals(QT_MAJOR_VERSION, 4) {
    LIBS += -lqjson
    }

CONFIG -= stl
CONFIG += qt warn_on thread

QT += network sql
QT -= gui

include(../global.pri)
include(../hobrasofthttpd/src/hobrasofthttpd.pri)
include(security/security.pri)
include(database/database.pri)
include(httpd/httpd.pri)
include(utils/utils.pri)
include(src.pri)

equals(QT_ARCH, arm) {
  DEFINES += ARM
}

