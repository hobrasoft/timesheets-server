/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerStatuses_H_
#define _Httpd_ControllerStatuses_H_

#include <QObject>
#include "abstractcontroller.h"



/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerStatuses - /statuses - Manipulace se stavy ticketů
 * @}
 */


namespace Httpd {

/**
@brief Manipulace se stavy ticketů

GET
----
Vrací seznam všech stavů, které může ticket nabývat
@code
[
  {
    "abbreviation": "NEW",      // Zkratka
    "color": "transparent",     // Barva, kterou se má zobrazovat v GUI
    "description": "Nový",      // Popis stavu
    "status": "NEW"             // Primární klíč, identifikátor stavu
  },
  {
    "abbreviation": "PRI",
    "color": "red",
    "description": "Vysoká priorita",
    "status": "PRIORITY"
  },
  {
    "abbreviation": "CLOSED",
    "color": "green",
    "description": "Uzavřeno",
    "status": "CLOSED"
  },
  {
    "abbreviation": "ARCHIVE",
    "color": "brown",
    "description": "V archivu",
    "status": "ARCHIVE"
  }
]

@endcode


*/
class ControllerStatuses : public AbstractController {
    Q_OBJECT
  public:
    ControllerStatuses(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
