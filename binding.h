#ifndef BINDING_H
#define BINDING_H

#include <QObject>
#include <QNetworkAccessManager>
#include "oauthmastodon.h"
#include "qmastodonpoststatus.h"

class Binding : public QObject
{
    Q_OBJECT
    /* Binding C++ code and QML code */
    Q_PROPERTY(QString startAuth WRITE startAuth)
    Q_PROPERTY(QByteArray postAuthCode WRITE postAuthCode)
    Q_PROPERTY(QByteArray toot WRITE postStatus)

public slots:

public:
    Binding(QObject* parent = 0);
    void startAuth(const QString& mastodonHost);
    void postAuthCode(const QByteArray& authCode);
    void postStatus(const QString& status);

private:
    OAuthMastodon* m_oauthMastodon;
    QString m_mastodonHost;
    QByteArray m_authCode;
};

#endif // BINDING_H
