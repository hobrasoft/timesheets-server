# INCLUDEPATH += $$PWD # Nesmí tu být
DEPENDPATH  += $$PWD

GIT_COMMIT = $$system(git --git-dir $$PWD/../../.git --work-tree $$PWD/../.. describe --always --tags)
DEFINES += GIT_COMMIT=\\\"$$GIT_COMMIT\\\"
GIT_BRANCH = $$system(git rev-parse --abbrev-ref HEAD)
DEFINES += GIT_BRANCH=\\\"$$GIT_BRANCH\\\"

SOURCES += \
 $$PWD/controllerserver.cpp \
 $$PWD/controllerusers.cpp \
 $$PWD/controllercategories.cpp \
 $$PWD/controllertickets.cpp \
 $$PWD/controllerticketsvw.cpp \
 $$PWD/controllerticketstatus.cpp \
 $$PWD/controllertickettimesheets.cpp \
 $$PWD/controllerticketvalues.cpp \
 $$PWD/controllerticketfiles.cpp \
 $$PWD/controllerstatusorder.cpp \
 $$PWD/controllerstatuses.cpp \

HEADERS += \
 $$PWD/controllerserver.h \
 $$PWD/controllerusers.h \
 $$PWD/controllercategories.h \
 $$PWD/controllertickets.h \
 $$PWD/controllerticketsvw.h \
 $$PWD/controllerticketstatus.h \
 $$PWD/controllertickettimesheets.h \
 $$PWD/controllerticketvalues.h \
 $$PWD/controllerticketfiles.h \
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
