/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include <QDebug>
#include <QTextCodec>
#include <QStringList>
#include <QTranslator>
#include <QLibraryInfo>
#include <QCoreApplication>
#include <QFile>
#include "timesheetserver.h"
#include "version.h"
#include <signal.h>

int main (int argc, char *argv[]) {

    signal(SIGPIPE, SIG_IGN);

    QCoreApplication app(argc, argv);
    app.setOrganizationName   (SETTINGS_ORGANIZATION_NAME);
    app.setOrganizationDomain (SETTINGS_ORGANIZATION_DOMAIN);
    app.setApplicationName    (SETTINGS_APPLICATION);
    app.setApplicationVersion (VERSION);

    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());

    TimesheetServer::instance();
    return app.exec();

}



