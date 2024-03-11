DEPENDPATH  += $$PWD
INCLUDEPATH += $$PWD

RESOURCES += \
 $$PWD/postgres.qrc \

SOURCES += \
 $$PWD/database_plugin_postgres.cpp \

HEADERS += \
 $$PWD/database_plugin_postgres.h \

FILES += \
 $$PWD/patch.000.sql \
 $$PWD/patch.001.sql \
 $$PWD/patch.002.sql \

