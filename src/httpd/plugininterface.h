#ifndef _PluginInterface_H_
#define _PluginInterface_H_

#include "abstractcontroller.h"

/**
 * @brief Interface pro pluginy serveru 
 *
 * V této podobě vidí plugin server. Pro odvození pluginu použijte třídu Plugin.
 */

class PluginInterface {
  public:

    /**
     * @brief Vrací název pluginu
     *
     * Tato metoda musí být vždy reimplementovaná.
     */
    virtual QString description() = 0;

    /**
     * @brief Vrací cestu pro www server
     *
     * Pokud má být plugin přístupný přes www rozhraní serveru, musí metoda vracet nepoužitou cestu v
     * hierarchii www stránek, například:
     * "/demo-plugin"
     */
    virtual QString path() = 0;

    /**
     * @brief Vytváří nový kontroler pro www server a vrací jeho adresu
     *
     * Pokud má být plugin přístupný přes www rozhraní serveru, musí být v pluginu vytvořený kontroler
     * odvozený od třídy Httpd::AbstractController a musí být reimplementovaná metoda createController().
     */
    virtual Httpd::AbstractController *createController(HobrasoftHttpd::HttpConnection *) = 0;
};

Q_DECLARE_INTERFACE(PluginInterface, "cz.hobrasoft.fotomonserver.plugin");

#endif
