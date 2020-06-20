/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Permissions_h_
#define _Permissions_h_

#include <QString>

namespace Security {

/**
 * @brief Třída obsahuje pouze seznam jednotlivých oprávnění 
 *
 * Oprávnění odpovídají jednotlivým URL adresám dostupným přes webové API
 */
struct Permissions {
    /**
     * @brief Seznam jednotlivých oprávnění
     */
    enum Permission {
        Server,
        ServerRestart,
        ServerConfigFile,
        ImportersQueue,
        DataPowerToday,
        SystemPower,
        Systems,
        GroupHistogram,
        Invertors,
        InvertorStatus,
        Importers,
        Translations 
        };

    /**
     * @brief Vrací název oprávnění v textu
     */
    static QString toString(Permission);
};

}

#endif
