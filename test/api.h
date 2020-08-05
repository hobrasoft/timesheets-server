/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Test_Api_H_
#define _Test_Api_H_

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include "json.h"

namespace TestApi {

/**
 * @brief
 */
class Api : public QObject {
    Q_OBJECT
  public:
    Api(QObject *parent = nullptr);

    bool get(const QString &);
    bool put(const QString &url, const QVariantMap& data);
    bool del(const QString &url);
    QString errorString() const { return m_errorString; }
    QNetworkReply::NetworkError error() const { return m_error; }
    const QByteArray& data() const { return m_data; }
    const QVariant variant() const { return JSON::data(m_data); }

  private:
    QString toString(QNetworkReply::NetworkError);

    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QByteArray m_data;
    QNetworkReply::NetworkError m_error;
    QString m_errorString;
    QString m_serverString;

    static QNetworkCookieJar *m_cookieJar;


};

}

#endif

