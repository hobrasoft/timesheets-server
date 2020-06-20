/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "logger.h"
#include "msettings.h"
#include <QTextStream>
#include <QDateTime>
#include <QRegExp>
#include <QFile>

Logger *Logger::m_logger = nullptr;
QString Logger::m_log_include;
QString Logger::m_log_exclude;
bool    Logger::m_log_all;
QString Logger::m_log_file;
#ifdef USE_UDP_DEBUG
QUdpSocket *Logger::m_socket = nullptr;
QHostAddress Logger::m_debugAddress;
int Logger::m_debugPort;
#endif


Logger::Logger(QObject *parent) : QObject (parent) {
    m_logger = this;
    m_log_all      = MSETTINGS->logAll();
    m_log_include  = MSETTINGS->logInclude();
    m_log_exclude  = MSETTINGS->logExclude();
    m_log_file     = MSETTINGS->logFile();

    
    #ifdef USE_UDP_DEBUG
    m_debugAddress = QHostAddress("2a0a:1c01:0:1005::99");
    m_debugPort = 9011;
    m_socket = new QUdpSocket(this);
    m_socket->bind();
    m_socket->writeDatagram(QByteArray("Nastartovany UDP socket\n"), m_debugAddress, m_debugPort);
    #endif


    qInstallMessageHandler(Logger::messageOutput);
}


Logger *Logger::logger(QObject *parent) {
    if (m_logger == NULL) {
        Q_ASSERT(parent);
        new Logger(parent);
        }
    return m_logger;
}


void Logger::messageOutput(const QString& text) {
    Logger::messageOutput(QtDebugMsg, QMessageLogContext(), text);
}


void Logger::messageOutput(QtMsgType type, const QMessageLogContext&, const QString& text) {
    Q_UNUSED(type);
    QTextStream ostream(stderr);
    QFile file;
    if (!m_log_file.isEmpty()) {
        file.setFileName(m_log_file);
        if (file.open(QIODevice::Append)) {
            ostream.setDevice(&file);
            }
        }

    auto outstring = [&]() {
        ostream 
            << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " "
            << text
            << "\r\n"
            ;
        #ifdef USE_UDP_DEBUG
        QString xxx = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " " + text + "\n";
        m_socket->writeDatagram(xxx.toUtf8(), m_debugAddress, m_debugPort);
        #endif

        };

    if (m_log_all && m_log_exclude.isEmpty()) {
        outstring();
        return;
        }

    if (m_log_all && !m_log_exclude.isEmpty()) {
        if (!text.contains(QRegExp(m_log_exclude))) {
            outstring();
            }
        return;
        }

    if (m_log_all) {
        outstring();
        return;
        }

    if (m_log_include.isEmpty()) {
        return;
        }

    if (!m_log_exclude.isEmpty() && text.contains(QRegExp(m_log_include))) {
        if (text.contains(QRegExp(m_log_exclude))) {
            return;
            }
        outstring();
        return;
        }

    if (text.contains(QRegExp(m_log_include))) {
        outstring();
        }

}


