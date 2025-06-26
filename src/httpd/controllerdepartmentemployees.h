#ifndef _Httpd_ControllerDepartmentEmployees_H_
#define _Httpd_ControllerDepartmentEmployees_H_

#include "abstractcontroller.h"

namespace Httpd {

class ControllerDepartmentEmployees : public AbstractController {
    Q_OBJECT
public:
    ControllerDepartmentEmployees(HobrasoftHttpd::HttpConnection *parent);
protected:
    void service(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) Q_DECL_OVERRIDE;
};

}

#endif
