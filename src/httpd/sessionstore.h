/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _SessionStore_H_
#define _SessionStore_H_

#include <QObject>
#include "httpsessionstore.h"
#include "httpsettings.h"

namespace Httpd {

/**
 * @brief Seesion store - zajišťuje singleton instanci třídy
 */
class SessionStore : public HobrasoftHttpd::HttpSessionStore {
    Q_OBJECT
  public:

    /**
     * @brief Vrací ukazatel na singleton instanci třídy
     */
    static SessionStore *sessionStore(const HobrasoftHttpd::HttpSettings *settings = NULL, QObject *parent = NULL);

  private:
    #ifndef DOXYGEN_SHOULD_SKIP_THIS
    SessionStore(const HobrasoftHttpd::HttpSettings *settings, QObject *parent);

    static SessionStore *m_sessionStore;
    #endif
};

}

#endif
