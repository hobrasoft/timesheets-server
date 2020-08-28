/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllercategoriestree.h"
#include "pdebug.h"
#include "db.h"

using namespace Httpd;


ControllerCategoriesTree::ControllerCategoriesTree(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
}


void ControllerCategoriesTree::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    return serviceIdGet(request, response, QString());
}


void ControllerCategoriesTree::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    serviceOK(request, response, subtree(id));
}


QVariantList ControllerCategoriesTree::subtree(const QString& id) {
    QList<Dbt::Categories> list = db()->subcategories(id);
    QVariantList tree;
    for (int i=0; i<list.size(); i++) {
        QVariantMap x = list[i].toMap();
        x["categories"] = subtree(list[i].category);
        tree << x;
        }
    PDEBUG << id << list.size();
    return tree;
}


