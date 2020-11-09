/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerOverviewPublic_H_
#define _Httpd_ControllerOverviewPublic_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerOverviewPublic - /overview/0x&lt;id&gt; - Vrací podklady pro tištěný pracovní výkaz
 * @}
 */


namespace Httpd {

/**
@brief Vrací podklady pro tištěný pracovní výkaz

GET /overviedw/0x&lt;categoryid&gt;
---

@code
@endcode


*/
class ControllerOverviewPublic : public AbstractController {
    Q_OBJECT
  public:
    ControllerOverviewPublic(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceIdGet (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
