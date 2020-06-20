/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Roles_H_
#define _Roles_H_

#include <QString>

namespace Security {

/**
 * @brief Třída obsahuje seznam uživatelský rolí
 *
 * Momentálně jsou role pouze dvě: administrátor a obyčejný uživatel
 */
struct Roles {

    /**
     * @brief Seznam jednotlivých rolí
     */
    enum Role {
        Admin = 0,      ///< Administrátor
        User            ///< Obyčejný uživatel
        };

    /**
     * @brief Vrací název odpovídající role v textu
     */
    static QString toString(Role role) {
        switch  (role) {
            case Admin: return "Admin";
            case User:  return "User";
            }
        return QString();
        }

};

}

#endif
