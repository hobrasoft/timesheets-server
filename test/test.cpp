/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "test.h"
#include "msettings.h"
#include "logger.h"
#include "pdebug.h"
#include "api.h"
#include <QDebug>
#include <QSqlQuery>
#include <QNetworkReply>
#include <unistd.h>


using namespace TestApi;

void messageOutput(QtMsgType, const QMessageLogContext&, const QString&) {}

#define APIGET(url) { \
    if (!api()->get(url)) { \
        QFAIL(api()->errorString().toUtf8().data()); \
        } \
}

#define APIGET_IGNORE_ERROR(url) { \
    api()->get(url); \
}

#define APIPUT(url, data) { \
    if (!api()->put(url, data)) { \
        QFAIL(api()->errorString().toUtf8().data()); \
        } \
}

#define APIDEL(url) { \
    if (!api()->del(url)) { \
        QFAIL(api()->errorString().toUtf8().data()); \
        } \
}


Test::Test() : QObject() {
    m_api = new Api(this);
}


void Test::initTestCase() {
    MSettings::instance(this);
    QVERIFY(MSETTINGS != nullptr);
    Logger::instance(this);
    QVERIFY(Logger::instance() != nullptr);
    Logger::instance()->m_log_all = true;
    Logger::instance()->m_log_exclude = QString();
    Logger::instance()->m_log_include = QString();
    Logger::instance()->m_log_file    = QString();
    if (!QCoreApplication::arguments().contains("--verbose")) {
        qInstallMessageHandler(messageOutput);
        }

    m_process = new QProcess(this);

    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->start(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("timesheet-server"));
    if (!m_process->waitForStarted()) {
        QFAIL("Server did not started");
        }
    sleep(1);

    connect(m_process, &QProcess::readyReadStandardOutput, [this](){
        qDebug() << m_process->readAllStandardOutput();
        qDebug() << m_process->readAllStandardError();
        });
}

void Test::cleanupTestCase() {
    QVERIFY (m_process->state() == QProcess::Running);
    
    m_process->terminate();
    if (!m_process->waitForFinished(1000)) {
        m_process->kill();
        QFAIL("Process timesheets-server refused to die!");
        }

    sleep(1);
    m_process->kill();

}


void Test::getServerInfo() {
    QVariantMap data;
    APIGET("/server/about");
    data  = api()->variant().toMap();
    QVERIFY(data.contains("configfile"));
    QVERIFY(data.contains("name"));
    QVERIFY(data.contains("description"));
    QVERIFY(data.contains("git_commit"));
    QVERIFY(data.contains("git_branch"));
    QVERIFY(data.contains("version"));
    data = api()->variant().toMap();
    APIGET_IGNORE_ERROR("/server/config");
    QVERIFY(api()->error() == QNetworkReply::AuthenticationRequiredError);
}


void Test::authenticate() {
    APIGET_IGNORE_ERROR("/authenticate?user=ADMIN&password=heslokleslo");
    QVERIFY(api()->error() == QNetworkReply::AuthenticationRequiredError);
    APIGET("/authenticate?user=admin&password=admin");
    QVariantMap data  = api()->variant().toMap();
    QVERIFY(data["username"] == "admin");
    QVERIFY(data["userid"] == "1");
}


void Test::getUsers() {
    APIGET("/users");
    QVariantList list = api()->variant().toList();
    QVERIFY(list.isEmpty() == false);
    APIGET("/users/1");
    QVariantMap data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["user"].toInt() == 1);
    QVERIFY(data["login"].toString() == "admin");
    QVERIFY(data["name"].toString() == "Administrator");
    QVERIFY(data["lang"].toString() == "en");
    QVERIFY(data["enabled"].toBool() == true);
    QVERIFY(data["admin"].toBool() == true);
}


void Test::putUsers() {
    QVariantMap data;
    data["login"] = "test";
    data["name"] =  "Test user";
    data["lang"] =  "cs";
    data["enabled"] = true;
    data["admin"] = false;
    APIPUT("/users/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    m_key = api()->variant().toMap()["key"].toString();

    APIGET("/users/"+m_key);
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["login"].toString() == "test");
    QVERIFY(data["name"].toString() == "Test user");
    QVERIFY(data["lang"].toString() == "cs");
    QVERIFY(data["enabled"].toBool() == true);
    QVERIFY(data["admin"].toBool() == false);
}


void Test::delUsers() {
    APIDEL("/users/" + m_key);

    // Admin musí projít
    APIGET_IGNORE_ERROR("/users/"+m_key);
    QVERIFY(api()->error() != QNetworkReply::NoError);
    QVERIFY(api()->error() == QNetworkReply::ContentNotFoundError);

    // Neadmin nesmí projít
    APIGET("/authenticate?user=tycho&password=heslo");

}


void Test::getCategories() {
    APIGET("/categories");
    QVariantList list = api()->variant().toList();
    QVERIFY(list.isEmpty() == false);

    QString id = list[0].toMap()["category"].toString();
    APIGET("/categories/" + id);
    QVariantMap data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["category"].toString() == id);
    QVERIFY(data.contains("parent_category"));
    QVERIFY(data.contains("description"));
    QVERIFY(data.contains("price"));
}


void Test::putCategoriesInsert() {
    QVariantMap data;
    data["description"] = "popis";
    data["parent_category"] = QVariant();
    data["price"] = 1000;
    APIPUT("/categories/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    m_key = api()->variant().toMap()["key"].toString();

    data.clear();
    APIGET("/categories/"+m_key);
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["category"].toString() == m_key);
    QVERIFY(data["description"].toString() == "popis");
    QVERIFY(data["parent_category"].toString() == "0" || 
            data["parent_category"].toString() == "");
    QVERIFY(data["price"].toDouble() == 1000);
}


void Test::putCategoriesUpdate() {
    QVariantMap data;
    data["description"] = "popis-2";
    data["parent_category"] = "1";
    data["price"] = 1367;
    APIPUT("/categories/"+m_key, data);
    QVERIFY(api()->variant().toMap().contains("key"));
    m_key = api()->variant().toMap()["key"].toString();

    data.clear();
    APIGET("/categories/"+m_key);
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["category"].toString() == m_key);
    QVERIFY(data["description"].toString() == "popis-2");
    QVERIFY(data["parent_category"].toString() == "1");
    QVERIFY(data["price"].toDouble() == 1367);
}


void Test::delCategories() {
    APIDEL("/categories/"+m_key);
    APIGET_IGNORE_ERROR("/categories/"+m_key);
    QVERIFY(api()->error() != QNetworkReply::NoError);
    QVERIFY(api()->error() == QNetworkReply::ContentNotFoundError);
}


void Test::getStatuses() {
    QVariantMap data;
    APIGET("/statuses");
    QVariantList list = api()->variant().toList();
    QVERIFY(list.isEmpty() == false);

    QString id = list[0].toMap()["status"].toString();
    APIGET("/statuses/" + id);
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["status"].toString() == id);
    QVERIFY(data.contains("abbreviation"));
    QVERIFY(data.contains("description"));
    QVERIFY(data.contains("closed"));
    QVERIFY(data.contains("color"));
}


void Test::putStatusesInsert() {
    QVariantMap data;
    data["status"] = "TEST-TEST-TEST";
    data["abbreviation"] = "zkratka";
    data["color"] = "#40506070";
    data["description"] = "testovaci status";
    data["closed"] = true;
    APIPUT("/statuses/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    QVERIFY(api()->variant().toMap()["key"].toString() == "TEST-TEST-TEST");

    data.clear();
    APIGET("/statuses/TEST-TEST-TEST");
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["status"].toString() == "TEST-TEST-TEST");
    QVERIFY(data["abbreviation"].toString() == "zkratka");
    QVERIFY(data["color"].toString() == "#40506070");
    QVERIFY(data["description"].toString() == "testovaci status");
    QVERIFY(data["closed"].toBool() == true);
}


void Test::putStatusesInsert2() {
    QVariantMap data;
    data["status"] = "TEST-TEST-TEST-2";
    data["abbreviation"] = "zkratka";
    data["color"] = "#40506070";
    data["description"] = "testovaci status";
    data["closed"] = true;
    APIPUT("/statuses/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    QVERIFY(api()->variant().toMap()["key"].toString() == "TEST-TEST-TEST-2");

    data.clear();
    APIGET("/statuses/TEST-TEST-TEST-2");
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["status"].toString() == "TEST-TEST-TEST-2");
    QVERIFY(data["abbreviation"].toString() == "zkratka");
    QVERIFY(data["color"].toString() == "#40506070");
    QVERIFY(data["description"].toString() == "testovaci status");
    QVERIFY(data["closed"].toBool() == true);
}



void Test::putStatusesUpdate() {
    QVariantMap data;
    data.clear();
    data["status"] = "TEST-TEST-TEST";
    data["abbreviation"] = "zkratka-2";
    data["color"] = "#40506070-2";
    data["description"] = "testovaci status-2";
    data["closed"] = false;
    APIPUT("/statuses/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    QVERIFY(api()->variant().toMap()["key"].toString() == "TEST-TEST-TEST");

    data.clear();
    APIGET("/statuses/TEST-TEST-TEST");
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["status"].toString() == "TEST-TEST-TEST");
    QVERIFY(data["abbreviation"].toString() == "zkratka-2");
    QVERIFY(data["color"].toString() == "#40506070-2");
    QVERIFY(data["description"].toString() == "testovaci status-2");
    QVERIFY(data["closed"].toBool() == false);

}


void Test::getStatusOrder() {
    QVariantMap data;
    APIGET("/statusorder");
    QVariantList list = api()->variant().toList();
    QVERIFY(list.isEmpty() == false);

    QString id = list[0].toMap()["id"].toString();
    QVERIFY(id.toInt() > 0);
    APIGET("/statusorder/" + id);
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["id"].toString() == id);
    QVERIFY(data.contains("category"));
    QVERIFY(data.contains("next_status"));
    QVERIFY(data.contains("previous_status"));
}


void Test::putStatusOrderInsert() {
    QVariantMap data;
    // poprvé insert
    data["next_status"] = "TEST-TEST-TEST";
    data["previous_status"] = "TEST-TEST-TEST";
    data["category"] = QVariant();
    APIPUT("/statusorder/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    QVERIFY(api()->variant().toMap()["key"].toInt() > 0);
    m_key = api()->variant().toMap()["key"].toString();

    data.clear();
    APIGET("/statusorder/" + m_key);
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["next_status"].toString() == "TEST-TEST-TEST");
    QVERIFY(data["previous_status"].toString() == "TEST-TEST-TEST");
    QVERIFY(data["category"].toString() == "");
}


void Test::putStatusOrderInsert2() {
    QVariantMap data;
    data["next_status"] = "TEST-TEST-TEST-2";
    data["previous_status"] = "TEST-TEST-TEST-2";
    data["category"] = QVariant();
    APIPUT("/statusorder/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    QVERIFY(api()->variant().toMap()["key"].toInt() > 0);
    m_key = api()->variant().toMap()["key"].toString();

    data.clear();
    APIGET("/statusorder/" + m_key);
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["next_status"].toString() == "TEST-TEST-TEST-2");
    QVERIFY(data["previous_status"].toString() == "TEST-TEST-TEST-2");
    QVERIFY(data["category"].toString() == "");
}


void Test::putStatusOrderUpdate2() {
    QVariantMap data;
    data["next_status"] = "TEST-TEST-TEST";
    data["previous_status"] = "TEST-TEST-TEST-2";
    data["category"] = QVariant();
    APIPUT("/statusorder/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    QVERIFY(api()->variant().toMap()["key"].toInt() > 0);
    m_key = api()->variant().toMap()["key"].toString();

    data.clear();
    APIGET("/statusorder/" + m_key);
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["next_status"].toString() == "TEST-TEST-TEST");
    QVERIFY(data["previous_status"].toString() == "TEST-TEST-TEST-2");
    QVERIFY(data["category"].toString() == "");
}


void Test::putStatusOrderUpdate3() {
    QVariantMap data;
    data["next_status"] = "TEST-TEST-TEST-2";
    data["previous_status"] = "TEST-TEST-TEST";
    data["category"] = QVariant();
    APIPUT("/statusorder/x", data);
    QVERIFY(api()->variant().toMap().contains("key"));
    QVERIFY(api()->variant().toMap()["key"].toInt() > 0);
    m_key = api()->variant().toMap()["key"].toString();

    data.clear();
    APIGET("/statusorder/" + m_key);
    data = api()->variant().toMap();
    QVERIFY(data.isEmpty() == false);
    QVERIFY(data["next_status"].toString() == "TEST-TEST-TEST-2");
    QVERIFY(data["previous_status"].toString() == "TEST-TEST-TEST");
    QVERIFY(data["category"].toString() == "");
}



void Test::delStatuses2() {
    APIDEL("/statuses/TEST-TEST-TEST-2");
    APIGET_IGNORE_ERROR("/statuses/TEST-TEST-TEST-2");
    QVERIFY(api()->error() != QNetworkReply::NoError);
    QVERIFY(api()->error() == QNetworkReply::ContentNotFoundError);
}


void Test::delStatuses() {
    APIDEL("/statuses/TEST-TEST-TEST");
    APIGET_IGNORE_ERROR("/statuses/TEST-TEST-TEST");
    QVERIFY(api()->error() != QNetworkReply::NoError);
    QVERIFY(api()->error() == QNetworkReply::ContentNotFoundError);
    APIGET_IGNORE_ERROR("/statusorder/"+m_key);
    QVERIFY(api()->error() == QNetworkReply::ContentNotFoundError);
}


