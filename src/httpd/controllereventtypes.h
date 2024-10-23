/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerEventTypes_H_
#define _Httpd_ControllerEventTypes_H_

#include <QObject>
#include "abstractcontroller.h"


/**
 * @addtogroup apiurl
 * @{
 * - @ref Httpd::ControllerEventTypes - /event_type - Manipuluje s informacemi o typu událostí v event logu v docházce
 * @}
 */


namespace Httpd {

/**
@brief Manipuluje s informacemi o typu události v event logu v docházce

Implementované metody:
- get
- put, post
- delete

Struktura jednoho záznamu: 

@code
{
"event_type": "P",
"description": "Příchod",
"passage": false,
"end_state": false,
"arrival": true,
"vacation": false,
"sick_leave": false,
"compensatory_leave": false,
"business_trup": false,
"break_time": false,
"unpaid_leave": false,
"sick_care": false
}
@endcode


*/
class ControllerEventTypes : public AbstractController {
    Q_OBJECT
  public:
    ControllerEventTypes(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

  private slots:

  private:

};

}

#endif
