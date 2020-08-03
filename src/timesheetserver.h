/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */
#ifndef _TimesheetServer_H_
#define _TimesheetServer_H_

#include <QObject>
#include <QDateTime>
#include <QThread>


/**
 * @brief Hlavní třída - startovaná z main, inicializuje aplikaci a startuje potřebné objekty
 */
class TimesheetServer : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief Vrací ukazatel na singleton instanci třídy
     */
    static TimesheetServer *instance();

   ~TimesheetServer();

    const QDateTime& startTime() const { return m_startTime; }

  public slots:

    /**
     * @brief Restartuje (nebo ukončí) server
     */
    void restart();


  private slots:

    /**
     * @brief Funkce volaná přes timer po startu aplikace
     *
     * Vytváří potřebné objekty: Databaze, FrontaHovoru, SeznamJednotek, MedibusServer, SipServer
     */
    void init();


    /**
     * @brief Ukončí aplikaci
     */
    void destroy();


    /**
     * @brief Ukončí aplikaci natrvrdo, quit
     */
    void quit();


  private:

    /**
     * @brief Konstruktor, přes timer volá funkci init()
     */
    TimesheetServer();


    /**
     * @brief Singleton instance třídy
     */
    static TimesheetServer *m_timesheetServer;

    static QDateTime m_startTime;       ///< Čas startu

};

#endif
