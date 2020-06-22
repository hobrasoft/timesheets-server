INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

include(fotomon/fotomon.pri)

SOURCES += \
 $$PWD/dbt.cpp \
 $$PWD/database.cpp \
 $$PWD/database_plugin.cpp \
 $$PWD/msqlquery.cpp \

HEADERS += \
 $$PWD/db.h \
 $$PWD/dbt.h \
 $$PWD/database.h \
 $$PWD/database_plugin.h \
 $$PWD/msqlquery.h \


