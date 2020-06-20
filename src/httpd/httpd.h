/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_H_
#define _Httpd_H_

#include <QObject>
#include "httpserver.h"

/** @addtogroup Makra
 * @{
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define HTTPD Httpd::Httpd::httpd()
#endif
/** @} */

namespace HobrasoftHttpd {
class HttpServer;
class HttpSettings;
class HttpConnection;
class HttpRequestHandler;
}

/**
 * @brief Jmenný prostor pro obsluhu konkrétních HTTP požadavků aplikace
 */
namespace Httpd {


/**
 * @brief Konkrétní implementace HTTP serveru, rozšiřuje obecnou třídu HobrasoftHttpd::HttpServer, singleton
 *
 * Pro rozšíření je potřeba přepsat virtuální metodu requestHandler()
 */
class Httpd : public HobrasoftHttpd::HttpServer {
    Q_OBJECT
  public:

    /**
     * @brief Vrací ukazatal na singleton instanci třídy
     */
    static Httpd *httpd(QObject *parent = NULL);

    /**
     * @brief Vrací ukazatel na nově vytvořený handler 
     */
    virtual HobrasoftHttpd::HttpRequestHandler *requestHandler(HobrasoftHttpd::HttpConnection *);

  private:
    #ifndef DOXYGEN_SHOULD_SKIP_THIS
    Httpd(const HobrasoftHttpd::HttpSettings *settings, QObject *parent);

    static Httpd *m_httpd;
    #endif

};

}

#endif
