/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "controllercategoriestree.h"
#include "httprequest.h"
#include "pdebug.h"
#include "db.h"

using namespace Httpd;


ControllerCategoriesTree::ControllerCategoriesTree(HobrasoftHttpd::HttpConnection *parent) : AbstractController(parent) {
    m_maxdepth = 128;
}


void ControllerCategoriesTree::serviceList (HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response) {
    return serviceIdGet(request, response, QString());
}


void ControllerCategoriesTree::serviceIdGet(HobrasoftHttpd::HttpRequest *request, HobrasoftHttpd::HttpResponse *response, const QString& id) {
    if (request->parameter("maxdepth") != "") {
        m_maxdepth = request->parameter("maxdepth").toInt();
        }
    serviceOK(request, response, subtree(id, 0));
}


QVariantList ControllerCategoriesTree::subtree(const QString& id, int depth) {
    if (depth > m_maxdepth) { return QVariantList(); }
    depth += 1;
    QList<Dbt::Categories> list = db()->subcategories(id);
    QVariantList tree;
    for (int i=0; i<list.size(); i++) {
        QVariantMap x = list[i].toMap();
        x["categories"] = subtree(list[i].category, depth);
        tree << x;
        }
    return tree;
}


