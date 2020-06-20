CONFIG -= debug
QMAKE_CXXFLAGS += -ftree-parallelize-loops=8 -O3

# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES -= QT_DEPRECATED_WARNINGS


OBJECTS_DIR = .tmp
MOC_DIR     = .tmp
RCC_DIR     = .tmp
UI_DIR      = .tmp
QT         -= gui

