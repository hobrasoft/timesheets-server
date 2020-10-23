/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */
#ifndef _MSETTINGS_H_
#define _MSETTINGS_H_

#define MSETTINGS MSettings::instance()

#include <QSettings>
#include <QString>
#include <QLocale>

/**
 * @addtogroup konfigurace
 * @{
 *
 * Konfigurační soubor se hledá postupně v tomto pořadí:
 * - $HOME/.config/hobrasoft.cz/wellness.conf
 * - $HOME/.wellness.conf
 * - /etc/wellness.conf
 *
 * Použije se první nalezený soubor.
 * Konfigurační soubor lze zadat i z povelové řádky parametrem -C
 *
 * 
 * Logování
 * ----------------
 * - __log/all__ - Pokud je nastaveno true, loguje se vše, kromě regex uvedených v log/exclude
 * - __log/file__ - Soubor s logem, pokud není nastaveno, loguje se na stdout
 * - __log/exclude__ - Uvedené regex se nelogují, a to ani v případě, že je nastaveno log/all
 * - __log/include__ - Uvedené regex se logují, význam má ve spojeni s log/all = false 
 *
 *
 * Databáze
 * ----------------
 * Databáze je normálně uložená v souboru /var/lib/wellness/wellness.sqlite3.
 * Aplikace musí mít do tohoto adresáře přístup.
 * Databázová vrstva je převzata ze složitější aplikace, kde je možné připojení k různým 
 * typům databáze. Některé parametry proto nemají smysl.
 *
 * - __db/plugin__ - Vždy sqlite3
 * - __db/name__ - Cesta k souboru s databází (/var/lib/wellness/wellness.sqlite3)
 * - __db/server__ - u sqlite nevyužito
 * - __db/user__ - u sqlite nevyužito
 * - __db/password__ - u sqlite nevyužito
 * - __db/port__ - u sqlite nevyužito
 * - __db/maxMachineValuesDays__ - maximální počet dnů, po které se uchovávají provozní záznamy stroje
 * - __db/maxTrainingDays__ - maximální počet dnů, po které se uchovávají tréninky registravaných uživatelů
 *
 * Poznámka k položkám maxMachineValuesDays a maxTrainingDays: tréninky a provozní záznamy by se měly
 * kopírovat na portál výrobce a portál cvičence. U provozních záznamů platí, že jakmile jsou zkopírovné
 * na portál, z místní databáze se vymažou. Tréninky se mažou po zde nastavené době.
 *
 *
 * Http přístup
 * ----------------
 * K aplikaci je servisní přístup přes webový server.
 * Nastavení http serveru je oddělené a je popsané v dokumentaci http serveru:
 * https://www.hobrasoft.cz/en/httpserver/doc/group__configuration.html
 *
 * - __httpd/user__ - uživatel pro přihlášení
 * - __httpd/password__ - heslo pro přihlášení
 * - __httpd/authorize__ - pokud je true, je vyžadováno přihlášení, jinak je přístup možný bez hesla
 * - __httpd/htdocs__ - Cesta k souborům se HTML stránkami    
 *
 * Portál cvičence
 * ----------------
 * - __portal/url__ - Plná cesta k portálu (https://www.vacushape.fitness)
 * - __portal/shorturl__ - Zkrácená cesta k portálu (www.vacushape.fitness)
 * - __portal/apiurl__ - Plná cesta k API (https://api.vacushape.fitness/v1)
 *
 * 
 * Komunikace se strojem
 * ----------------
 * Pro ladicí účely může být stroj nahrazen TCP serverem, komunikace může probíhat
 * na různých USB zařízeních atd.
 *
 * - __machine/type__ - Typ připojení, uart nebo tcp (výchozí uart)
 * - __machine/device__ - Cesta k sériové lince (/dev/ttyUSB0)
 * - __machine/address__ - IP adresa tcp serveru virtuálního stroje (127.0.0.1)
 * - __machine/port__ - Port k tcp serveru virtuálního stroje (11112)
 * - __machine/read_delay__ - Pauza [ms] mezi odesláním povelu a čtením odezvy ze stroje (70 msec)
 *
 *
 * Uživatelské rozhraní
 * ----------------
 * - __gui/showexit__ - Na úvodní obrazovce se může zobrazovat ikona pro ukončení (výchozí false)
 * - __gui/locale__ - Jaký se má použít jazyk (překlady atd.)
 * - __gui/fullscreen__ - Jestli se má pustit ve fullscreen (Výchozí true)
 * - __gui/lowerOptimalHeartRate__ - dolní mez pro zobrazení optimální tepové frekvence (0.6)
 * - __gui/upperOptimalHeartRate__ - horní mez pro zobrazení optimální tepové frekvence (0.7)
 * - __gui/alternativeHeartRate__ - alternativní způsob výpočtu optimální tepové frekvence (true), (@ref vypocet)
 * - __gui/antigravity__ - alternativní způsob zobrazování přetlaku, určeno pro taiwance
 * - __gui/blocked__ - pokud je nastaveno true, je stroj zablokován
 *
 *
 * Licencování stroje certifikátem
 * ----------------
 * - __ssl/key__ - Umístění klíče  (Výchozí /etc/openvpn/client/west.vacushape.cz/client.key)
 * - __ssl/certificate__ - Umístění certifikátu stroje - každý stroj má vlastní certifikát (Výchozí /etc/openvpn/client/west.vacushape.cz/client.crt)
 * - __ssl/cacertificate__ - Umístění klíče certifikační autority  (Výchozí /etc/openvpn/client/west.vacushape.cz/ca.crt)
 *
 *
 * @}
 */

/**
 * @brief Globální QSettings objekt
 *
 * Během práce zůstává otevřený a přistupuje se k němu přes makro MSETTINGS.
 * Umožňuje přesunout konfigurační soubor na libovolné místo, například /etc.
 *
 * Singleton instance by měla být přístupná i v QML pod jménem MSETTINGS.
 */
class MSettings : public QSettings {
    Q_OBJECT
  public:
   ~MSettings();
    static MSettings *instance(QObject *parent = NULL);

    /*
     * Seznam řetězců pro jednotlivé konfigurační parametry
     */
    static constexpr const char *Initialized            = "wellness/initialized";
    static constexpr const char *LogAll                 = "log/all";
    static constexpr const char *LogInclude             = "log/include";
    static constexpr const char *LogExclude             = "log/exclude";
    static constexpr const char *LogFile                = "log/file";
    static constexpr const char *DbPlugin               = "db/plugin";
    static constexpr const char *DbServer               = "db/server";
    static constexpr const char *DbName                 = "db/name";
    static constexpr const char *DbUser                 = "db/user";
    static constexpr const char *DbPassword             = "db/password";
    static constexpr const char *DbPort                 = "db/port";
    static constexpr const char *DbFilesDirectory       = "db/files-directory";
    static constexpr const char *HttpdUser              = "httpd/user";
    static constexpr const char *HttpdPassword          = "httpd/password";
    static constexpr const char *HttpdAuthorize         = "httpd/authorize";
    static constexpr const char *ServerName             = "server/name";
    static constexpr const char *ServerDescription      = "server/description";
    static constexpr const char *ServerEnableRemoteRestart = "server/enable-remote-restart";


    /*
     * Seznam přístupových metod
     */
    bool    initialized() const { return value(Initialized, false).toBool(); }
    void    setInitialized(bool x) { setValue(Initialized, x); }

    // Logování
    bool    logAll() const { return value(LogAll, true).toBool(); }
    QString logInclude() const { return value(LogInclude).toString(); }
    QString logExclude() const { return value(LogInclude).toString(); }
    QString logFile() const { return value(LogFile).toString(); }

    // Databáze
    QString dbPlugin() const { return value(DbPlugin, "sqlite").toString(); }
    QString dbServer() const { return value(DbServer).toString(); }
    QString dbName() const { return value(DbName, "/var/lib/wellness/wellness.sqlite3").toString(); }
    QString dbUser() const { return value(DbUser).toString(); }
    QString dbPassword() const { return value(DbPassword).toString(); }
    int     dbPort() const { return value(DbPort, 5432).toInt(); }
    QString dbFilesDirectory() const { return value(DbFilesDirectory).toString(); }

    // Httpd
    QString httpdUser() const { return value(HttpdUser).toString(); }
    QString httpdPassword() const { return value(HttpdPassword).toString(); }
    bool    httpdAuthorize() const { return value(HttpdAuthorize).toBool(); }

    // Httpd
    QString serverName() const { return value(ServerName).toString(); }
    QString serverDescription() const { return value(ServerDescription).toString(); }
    bool    serverEnableRemoteRestart() const { return value(ServerEnableRemoteRestart, false).toBool(); }

  protected:

    /**
     * @brief Nastaví hodnotu v konfiguračním souboru
     *
     * Metoda je přetížená z QSettings::setValue() a přesunutá do protected, aby nebylo možné
     * zavolat tuto metodu přímo z aplikace. Vždy je nutné doplnit pro parametr přístupovou metodu.
     */
    void    setValue(const QString& key, const QVariant& value);

    /**
     * @brief Vrátí hodnotu z konfiguračního souboru
     *
     * Metoda je přetížená z QSettings::value() a přesunutá do protected, aby nebylo možné
     * zavolat tuto metodu přímo z aplikace. Vždy je nutné doplnit pro parametr přístupovou metodu.
     */
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

  private:
    /**
     * @brief Konstruktor, otevře zadaný soubor
     */
    MSettings(const QString& filename, QObject *parent);
    #ifndef DOXYGEN_SHOULD_SKIP_THIS
    static MSettings *m_mSettings;
    #endif

};

#endif
