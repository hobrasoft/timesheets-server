/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerServer_H_
#define _Httpd_ControllerServer_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerServer - /server - informace o serveru
 * @}
 */

namespace Httpd {

/**
@brief Vrací informace o serveru


GET /server/about
---
Vrací informace o serveru
@code
{
  "configfile": "/home/tycho/.timesheet-server.conf",               // Cesta k použitému konfiguračnímu souboru
  "name":       "Hobrasoft",                                        // Krátké jméno serveru
  "description": "Pokusný server pro vývoj pracovních výkazů",      // Podrobnější popis serveru
  "git_branch": "master",                                           // Použitá větev GITu, nastavuje se při překladu
  "git_commit": "7dfd959",                                          // Použitý commit GITu, nastavuje se při překladu
  "version":    "1.0.0"                                             // Verze, bere se z version.h
}
@endcode


GET /server/configfile
---
Vrací obsah konfiguračního souboru, hesla (položka password) jsou zamaskovaná:
@code
{
  "config": "[db]\nplugin   = postgres\nname     = timesheets\nserver   = hrabos.hobrasoft.cz\nport     = 5432\nuser     = tycho\npassword = ***\n\n[httpd]\nthreads = false\n\n[server]\nname = Hobrasoft\ndescription = Pokusný server pro vývoj pracovních výkazů\n\n[log]\nall = true\n\n",
  "configfile": "/home/tycho/.timesheet-server.conf"
}
@endcode


GET /server/restart?reallyrestart=YES
---
Restartuje server. Závisí na položce "server/enable-remote-restart" v konfiguračním souboru.
Ve skutečnosti se server nerestartuje, ale ukončí. O opětovné nastartování by se měl
postarat systemd. 

Vrací jednoduchou informaci, že požadavek byl přijat. Pokud není restartovaní umožněno, vrací chybu 405.
@code
{
    "ok": true;
}
@endcode


GET /server/objects
---
Vypíše objekty v paměti, pouze pro ladění serveru.
@code
[
  {
    "address": 94788673941232,
    "children": [],
    "class": "MSettings",
    "name": "unnamed"
  },
  {
    "address": 94788673967808,
    "children": [],
    "class": "Logger",
    "name": "unnamed"
  },
  {
    "address": 94788673960384,
    "children": [
      {
        "address": 94788673966688,
        "children": [],
        "class": "Db::Plugins::DatabasePluginPostgres",
        "name": "DatabasePluginPostgres"
      }
    ],
    "class": "Db::Database",
    "name": "Database 94788673922752"
  },
  {
    "address": 94788674023968,
    "children": [],
    "class": "Security::Guard",
    "name": "unnamed"
  },
  {
    "address": 94788674024368,
    "children": [],
    "class": "HobrasoftHttpd::HttpSettings",
    "name": "unnamed"
  },
  {
    "address": 94788674028784,
    "children": [],
    "class": "MyThread",
    "name": "threadHttpServer"
  }
]
@endcode


*/
class ControllerServer : public AbstractController {
    Q_OBJECT
  public:
    ControllerServer(HobrasoftHttpd::HttpConnection *parent);

  protected:

    virtual void serviceIdGet     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id);

  private slots:

  private:
    QVariantList objects(const QObject *object, int dept = 0);

};

}

#endif
