#ifndef _JSON_H_
#define _JSON_H_

#include <QVariant>
#include <QByteArray>
#include <QVariant>
#include <QList>
#include <QSet>

class JSON {
  public:
    static QByteArray json(const QVariant& data);               ///< Converts data to json
    static QByteArray json(const QVariant& data, bool *ok);     ///< Converts data to json
    static QVariant   data(const QByteArray& json);             ///< Converts json to data
    static QVariant   data(const QByteArray& json, bool *ok);   ///< Converts json to data
};

#endif
