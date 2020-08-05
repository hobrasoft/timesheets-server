/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "api.h"
#include <QEventLoop>
#include <QTimer>
#include <QNetworkRequest>
#include "pdebug.h"

using namespace TestApi;

QNetworkCookieJar *Api::m_cookieJar = nullptr;


Api::Api(QObject *parent) : QObject(parent) {
    if (m_cookieJar == nullptr) {
        m_cookieJar = new QNetworkCookieJar();
        }
    m_manager = new QNetworkAccessManager(this);
    m_manager->setCookieJar(m_cookieJar);
    m_reply = nullptr;
    m_serverString = "http://localhost:8080/api/v1";
    m_error = QNetworkReply::NoError;
}


bool Api::get(const QString& turl) {
    m_data.clear();
    QUrl url = QUrl::fromUserInput(m_serverString + turl);
    QNetworkRequest request(url);
    QEventLoop loop;
    QTimer::singleShot(1000, &loop, &QEventLoop::quit);
    connect(m_manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    m_reply = m_manager->get(request);
    loop.exec();
    if (!m_reply->isFinished()) {
        m_error = QNetworkReply::TimeoutError;
        m_errorString = QString("Timeout: %1").arg(url.toString());
        return false;
        }
    m_error = m_reply->error();
    m_errorString = toString(m_reply->error());
    if (m_reply->error() != QNetworkReply::NoError) {
        return false;
        }
    m_data = m_reply->readAll();
    return true;
}


bool Api::put(const QString& turl, const QVariantMap& data) {
    m_data.clear();
    QUrl url = QUrl::fromUserInput(m_serverString + turl);
    QNetworkRequest request(url);
    QEventLoop loop;
    QTimer::singleShot(1000, &loop, &QEventLoop::quit);
    connect(m_manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_reply = m_manager->put(request, JSON::json(data));
    loop.exec();
    if (!m_reply->isFinished()) {
        m_error = QNetworkReply::TimeoutError;
        m_errorString = QString("Timeout: %1").arg(url.toString());
        return false;
        }
    m_error = m_reply->error();
    m_errorString = toString(m_reply->error());
    if (m_reply->error() != QNetworkReply::NoError) {
        return false;
        }
    m_data = m_reply->readAll();
    return true;
}


bool Api::del(const QString& turl) {
    m_data.clear();
    QUrl url = QUrl::fromUserInput(m_serverString + turl);
    QNetworkRequest request(url);
    QEventLoop loop;
    QTimer::singleShot(1000, &loop, &QEventLoop::quit);
    connect(m_manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_reply = m_manager->deleteResource(request);
    loop.exec();
    if (!m_reply->isFinished()) {
        m_error = QNetworkReply::TimeoutError;
        m_errorString = QString("Timeout: %1").arg(url.toString());
        return false;
        }
    m_error = m_reply->error();
    m_errorString = toString(m_reply->error());
    if (m_reply->error() != QNetworkReply::NoError) {
        return false;
        }
    m_data = m_reply->readAll();
    return true;
}


QString Api::toString(QNetworkReply::NetworkError error) {
    switch (error) {
        case QNetworkReply::ConnectionRefusedError              : return QStringLiteral("The remote server refused the connection (the server is not accepting requests");
        case QNetworkReply::RemoteHostClosedError               : return QStringLiteral("The remote server closed the connection prematurely, before the entire reply was received and processed");
        case QNetworkReply::HostNotFoundError                   : return QStringLiteral("The remote host name was not found (invalid hostname");
        case QNetworkReply::TimeoutError                        : return QStringLiteral("The connection to the remote server timed out");
        case QNetworkReply::OperationCanceledError              : return QStringLiteral("The operation was canceled via calls to abort() or close() before it was finished");
        case QNetworkReply::SslHandshakeFailedError             : return QStringLiteral("The SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted");
        case QNetworkReply::TemporaryNetworkFailureError        : return QStringLiteral("The connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established");
        case QNetworkReply::NetworkSessionFailedError           : return QStringLiteral("The connection was broken due to disconnection from the network or failure to start the network");
        case QNetworkReply::BackgroundRequestNotAllowedError    : return QStringLiteral("The background request is not currently allowed due to platform policy");
        case QNetworkReply::TooManyRedirectsError               : return QStringLiteral("While following redirects, the maximum limit was reached. The limit is by default set to 50 or as set by QNetworkRequest::setMaxRedirectsAllowed()");
        case QNetworkReply::InsecureRedirectError               : return QStringLiteral("While following redirects, the network access API detected a redirect from a encrypted protocol (https) to an unencrypted one (http)");
        case QNetworkReply::ProxyConnectionRefusedError         : return QStringLiteral("The connection to the proxy server was refused (the proxy server is not accepting requests");
        case QNetworkReply::ProxyConnectionClosedError          : return QStringLiteral("The proxy server closed the connection prematurely, before the entire reply was received and processed");
        case QNetworkReply::ProxyNotFoundError                  : return QStringLiteral("The proxy host name was not found (invalid proxy hostname");
        case QNetworkReply::ProxyTimeoutError                   : return QStringLiteral("The connection to the proxy timed out or the proxy did not reply in time to the request send");
        case QNetworkReply::ProxyAuthenticationRequiredError    : return QStringLiteral("The proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)");
        case QNetworkReply::ContentAccessDenied                 : return QStringLiteral("The access to the remote content was denied (similar to HTTP error 403)");
        case QNetworkReply::ContentOperationNotPermittedError   : return QStringLiteral("The operation requested on the remote content is not permitted");
        case QNetworkReply::ContentNotFoundError                : return QStringLiteral("The remote content was not found at the server (similar to HTTP error 404");
        case QNetworkReply::AuthenticationRequiredError         : return QStringLiteral("The remote server requires authentication to serve the content but the credentials provided were not accepted (if any)");
        case QNetworkReply::ContentReSendError                  : return QStringLiteral("The request needed to be sent again, but this failed for example because the upload data could not be read a second time");
        case QNetworkReply::ContentConflictError                : return QStringLiteral("The request could not be completed due to a conflict with the current state of the resource");
        case QNetworkReply::ContentGoneError                    : return QStringLiteral("The requested resource is no longer available at the server");
        case QNetworkReply::InternalServerError                 : return QStringLiteral("The server encountered an unexpected condition which prevented it from fulfilling the request");
        case QNetworkReply::OperationNotImplementedError        : return QStringLiteral("The server does not support the functionality required to fulfill the request");
        case QNetworkReply::ServiceUnavailableError             : return QStringLiteral("The server is unable to handle the request at this time");
        case QNetworkReply::ProtocolUnknownError                : return QStringLiteral("The Network Access API cannot honor the request because the protocol is not know");
        case QNetworkReply::ProtocolInvalidOperationError       : return QStringLiteral("The requested operation is invalid for this protocol");
        case QNetworkReply::UnknownNetworkError                 : return QStringLiteral("An unknown network-related error was detected");
        case QNetworkReply::UnknownProxyError                   : return QStringLiteral("An unknown proxy-related error was detected");
        case QNetworkReply::UnknownContentError                 : return QStringLiteral("An unknown error related to the remote content was detected");
        case QNetworkReply::ProtocolFailure                     : return QStringLiteral("A breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.");
        case QNetworkReply::UnknownServerError                  : return QStringLiteral("An unknown error related to the server response was detected");
        case QNetworkReply::NoError                             : return QStringLiteral();
        }
    return QStringLiteral();
}


