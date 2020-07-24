# INCLUDEPATH += $$PWD # Nesmí tu být
DEPENDPATH  += $$PWD

SOURCES += \
 $$PWD/controllerserver.cpp \
 $$PWD/controllercategories.cpp \
 $$PWD/controllertickets.cpp \
 $$PWD/controllerticketstatus.cpp \
 $$PWD/controllerstatusorder.cpp \
 $$PWD/controllerstatuses.cpp \

HEADERS += \
 $$PWD/controllerserver.h \
 $$PWD/controllercategories.h \
 $$PWD/controllertickets.h \
 $$PWD/controllerticketstatus.h \
 $$PWD/controllerstatusorder.h \
 $$PWD/controllerstatuses.h \

HEADERS += \
 $$PWD/httpd.h \
 $$PWD/requestmapper.h \
 $$PWD/sessionstore.h \
 $$PWD/abstractcontroller.h \
 $$PWD/requestauthorizer.h \
 $$PWD/plugininterface.h \
 $$PWD/plugin.h \
 $$PWD/pluginstore.h \


SOURCES += \
 $$PWD/httpd.cpp \
 $$PWD/requestmapper.cpp \
 $$PWD/sessionstore.cpp \
 $$PWD/abstractcontroller.cpp \
 $$PWD/requestauthorizer.cpp \
 $$PWD/pluginstore.cpp \
