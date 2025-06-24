#ifndef _Httpd_ControllerEvents_H_
#define _Httpd_ControllerEvents_H_

#include <QObject>
#include "abstractcontroller.h"

/**
 * @addtogroup apiurl
 * @{ 
 * - @ref Httpd::ControllerEvents - /events - Manipuluje se záznamy událostí v docházce
 * @}
 */

namespace Httpd {

/**
 * @brief Manipuluje se záznamy událostí v docházce
 *
 * Implementované metody:
 * - get
 * - put, post
 * - delete
 *
 * Struktura jednoho záznamu:
 * @code
 * {
 *     "event"      : 123,
 *     "date"       : "2023-07-28T12:34:56",
 *     "event_type" : "P",
 *     "employee"   : 12,
 *     "valid"      : true,
 *     "user_edited": 5
 * }
 * @endcode
 */
class ControllerEvents : public AbstractController {
    Q_OBJECT
  public:
    ControllerEvents(HobrasoftHttpd::HttpConnection *parent);

  protected:
    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;

};

}

#endif
