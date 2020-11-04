/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerOverview_H_
#define _Httpd_ControllerOverview_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerOverview - /ticketsvw - Manipuluje s informacemi o ticketech
 * @}
 */


namespace Httpd {

/**
@brief Vrací podklady pro tištěný pracovní výkaz

GET /overviedw/&lt;categoryid&gt;
---

@code
@endcode


*/
class ControllerOverview : public AbstractController {
    Q_OBJECT
  public:
    ControllerOverview(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
