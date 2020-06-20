/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _Httpd_ControllerServer_H_
#define _Httpd_ControllerServer_H_

#include <QObject>
#include "abstractcontroller.h"

namespace Httpd {

/**
 * @brief Vyřizuje požadavky /server - informace a příkazy k fungování serveru (restart, aktivace a podobně)
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
