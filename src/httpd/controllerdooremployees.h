#ifndef _Httpd_ControllerDoorEmployees_H_
#define _Httpd_ControllerDoorEmployees_H_

#include "abstractcontroller.h"

namespace Httpd {

class ControllerDoorEmployees : public AbstractController {
    Q_OBJECT
public:
    ControllerDoorEmployees(HobrasoftHttpd::HttpConnection *parent);
protected:
    void service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
};

}

#endif
