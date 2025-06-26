#ifndef _Httpd_ControllerRfidEmployees_H_
#define _Httpd_ControllerRfidEmployees_H_

#include "abstractcontroller.h"

namespace Httpd {

class ControllerRfidEmployees : public AbstractController {
    Q_OBJECT
public:
    ControllerRfidEmployees(HobrasoftHttpd::HttpConnection *parent);
protected:
    void service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
};

}

#endif
