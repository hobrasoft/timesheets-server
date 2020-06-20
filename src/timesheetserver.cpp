/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "timesheetserver.h"
#include "msettings.h"
#include "pdebug.h"
#include "logger.h"
#include "mythread.h"
#include "httpd/httpd.h"
#include "security/security.h"
#include "security/guard.h"
#include <QFile>
#include <QTimer>
#include <QCoreApplication>

TimesheetServer *TimesheetServer::m_timesheetServer = NULL;
QDateTime TimesheetServer::m_startTime;

/**
 * @addtogroup konfigurace
 * @{
 * Konfigurace logování (třída TimesheetServer)
 * ------------
 *
 * - __log/file__ - Cesta k souboru, do kterého se ukládá log. Pokud je prázdné, vypisuje se na stdout
 * - __log/all__ - Pokud je true, loguje se vše, kromě exclude
 * - __log/include__ - Výraz se loguje, pokud ovšem není v exclude (regulární výraz)
 * - __log/exclude__ - Výraz se neloguje (regulární výraz)
 * @}
 */


TimesheetServer::TimesheetServer() {
    m_timesheetServer = this;
    MSettings::mSettings(this);
    QTimer::singleShot(1, this, SLOT(init()));
    m_startTime = QDateTime::currentDateTime();
}


TimesheetServer *TimesheetServer::instance() {
    if (m_timesheetServer == NULL) {
        new TimesheetServer();
        }
    return m_timesheetServer;
}


TimesheetServer::~TimesheetServer() {
    PDEBUG;
    QCoreApplication::quit();
}


void TimesheetServer::restart() {
    QTimer::singleShot(1000, this, SLOT(destroy()));
    QTimer::singleShot(5000, this, SLOT(quit()));
}


void TimesheetServer::quit() {
    PDEBUG;
    QCoreApplication::quit();
}


void TimesheetServer::destroy() {
    PDEBUG;
    deleteLater();
}


void TimesheetServer::init() {
    PDEBUG;
    Logger::logger(this);
    PDEBUG << "Starting server";

    // tady start jednotlivych komponent
    /*
    new Db::Databaze(this);
    MyThread::takeObject(this, "threadDataGroups",      Data::DataGroups::dataGroups(this));
    MyThread::takeObject(this, "threadDataSystems",     Data::DataSystems::dataSystems(this));
    MyThread::takeObject(this, "threadDataInvertors",   Data::DataInvertors::dataInvertors(this));
    MyThread::takeObject(this, "threadDataSensors",     Data::DataSensors::dataSensors(this));
    MyThread::takeObject(this, "threadAggregatorToday", Data::AggregatorToday::aggregatorToday(this));
    MyThread::takeObject(this, "threadAggregatorPast",  Data::AggregatorPast::aggregatorPast(this));
    Importers::ImportersList::importersList(this);
    */

    Security::Guard::guard(this);
    MyThread::takeObject(this, "threadHttpServer",      Httpd::Httpd::httpd(this));

    /*
    QTimer::singleShot(0, Data::DataGroups::dataGroups(), SLOT(init()));
    QTimer::singleShot(0, Data::DataSystems::dataSystems(), SLOT(init()));
    QTimer::singleShot(0, Data::DataInvertors::dataInvertors(), SLOT(init()));
    QTimer::singleShot(0, Data::DataSensors::dataSensors(), SLOT(init()));
    QTimer::singleShot(10000, Data::AggregatorToday::aggregatorToday(), SLOT(aggregate()));
    QTimer::singleShot(15000, Data::AggregatorPast::aggregatorPast(), SLOT(aggregate()));
    */

    PDEBUG << "Started";
}

