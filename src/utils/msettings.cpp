/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "msettings.h"
#include "pdebug.h"
#include <QCoreApplication>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

MSettings *MSettings::m_mSettings = NULL;


MSettings::~MSettings() {
    m_mSettings = NULL;
}


MSettings::MSettings(const QString& filename, QObject *parent) : QSettings(filename, QSettings::NativeFormat, parent) {
    m_mSettings = this;
    setIniCodec("UTF-8");
}


void MSettings::setValue(const QString& key, const QVariant& value) {
    QSettings::setValue(key, value);
    sync();
}


QVariant MSettings::value(const QString& key, const QVariant& defaultValue) const {
    return QSettings::value(key, defaultValue);
}


/**
 * @brief Vytváří signleton instanci třídy, vyhledá konfigurační soubor
 *
 * Konfigurační soubor vyhledává v tomto pořadí (jména adresářů se mohou lišit v závislosti na uživateli):
 * - z povelové řádky, parametr -C jméno_souboru
 * - ~/.config/hobrasoft.cz/wellness.conf
 * - ~/.wellness.conf
 * - /etc/wellness.conf
 */
MSettings *MSettings::mSettings(QObject *parent) {
    if (m_mSettings == NULL) {
        Q_ASSERT(parent);

        QString filename;

        while (true) {

            // Konfigurační soubor z povelové řádky
            QString configFileName;
            QStringList arguments = QCoreApplication::arguments();
            for (int i=0; i<arguments.size(); i++) {
                const QString& argument = arguments[i];
                if (argument == "-C" && i<arguments.size()-1) {
                    configFileName = arguments[i+1];
                    PDEBUG << "trying" << configFileName;
                    }
                }
            if (!configFileName.isEmpty()) {
                QFileInfo fileinfo(configFileName);
                if (fileinfo.exists() && fileinfo.isReadable()) {
                    filename = configFileName;
                    break;
                    }
                }


            // Konfigurační soubor ~/.config/hobrasoft.cz/timesheet-server.conf
            QFileInfo fileinfo1( QDir::homePath() + "/.config/hobrasoft.cz/timesheet-server.conf");
            PDEBUG << "Testing: " << fileinfo1.filePath();
            if (fileinfo1.exists() && fileinfo1.isReadable()) {
                filename = fileinfo1.filePath();
                break;
                }

            // Konfigurační soubor ~/.mdcv04ip.conf"
            QFileInfo fileinfo2( QDir::homePath() + "/.timesheet-server.conf");
            PDEBUG << "Testing: " << fileinfo2.filePath();
            if (fileinfo2.exists() && fileinfo2.isReadable()) {
                filename = fileinfo2.filePath();
                break;
                }


            // "/etc/mdcv04ip.conf"
            QFileInfo fileinfo3("/etc/timesheet-server.conf");
            PDEBUG << "Testing: " << fileinfo3.filePath();
            if (fileinfo3.exists() && fileinfo3.isReadable()) {
                filename = fileinfo3.filePath();
                break;
                }

            break;
            }

        PDEBUG << "Using configuration file: " << filename;
        new MSettings(filename, parent);
        }
    return m_mSettings;
}


