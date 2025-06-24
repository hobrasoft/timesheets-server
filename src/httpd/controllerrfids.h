#ifndef _Httpd_ControllerRfids_H_
#define _Httpd_ControllerRfids_H_

#include <QObject>
#include "abstractcontroller.h"

namespace Httpd {

/**
 * @addtogroup apiurl
 * @{ 
 * - @ref Httpd::ControllerRfids - /rfids - Manipuluje s informacemi o RFID kartach
 * @}
 */

/**
 * @brief Manipuluje s informacemi o RFID kartách v docházce
 *
 * Vrací seznam RFID karet.
 *
 * Implementované metody:
 * - get
 * - put, post
 * - delete
 */
class ControllerRfids : public AbstractController {
    Q_OBJECT
  public:
    ControllerRfids(HobrasoftHttpd::HttpConnection *parent);

  protected:

    void serviceList     (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
    void serviceIdGet    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
    void serviceIdPut    (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdPost   (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QVariantMap& data) Q_DECL_OVERRIDE;
    void serviceIdDelete (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) Q_DECL_OVERRIDE;
};

}

#endif
