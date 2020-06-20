#ifndef _Plugin_H_
#define _Plugin_H_

#include "plugininterface.h"
#include <QString>

/**
 * @brief Abstraktní třída pro tvorbu pluginů
 *
 * Plugin má přístup do celé aplikace, takže lze plugin použít prakticky k čemukoliv.
 * Ze strany serveru je plugin přístupný jen jako kontroler pro www server - plugin
 * tak může sloužit jednoduše jako rozšíření webového serveru o nové adresy.
 *
 * Plugin existuje po celou dobu běhu aplikace - nelze jej ukončit.
 *
 * Při odvození je nutné implementovat metodu description().
 * Pokud má být plugin přístupný přes www server, musí být reimplemetovaná metoda createController() a
 * metoda path();
 *
 * @see Demo
 */
class Plugin : public QObject, private PluginInterface {
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

  public:
    Plugin() {}

    virtual QString path() { return QString(); }
    virtual Httpd::AbstractController *createController(HobrasoftHttpd::HttpConnection *) { return NULL; }
};

#endif
