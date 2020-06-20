/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "permissions.h"
#include <QString>

using namespace Security;

QString Permissions::toString(Permissions::Permission x) {
    switch (x) {
        case Server:                return "Server";
        case ServerRestart:         return "ServerRestart";
        case ServerConfigFile:      return "ServerConfigFile";
        case ImportersQueue:        return "ImportersQueue";
        case Translations:          return "Translations";
        case DataPowerToday:        return "DataPowerToday";
        case SystemPower:           return "SystemPower";
        case Systems:               return "Systems";
        case GroupHistogram:        return "GroupHistogram";
        case Invertors:             return "Invertors";
        case InvertorStatus:        return "InvertorStatus";
        case Importers:             return "Importers";
        }
    return QString();
}

