#ifndef MASTODONCLIENT_H
#define MASTODONCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QtQml/qqmlregistration.h>

#include "oauthmastodon.h"
#include "qmastodonpoststatus.h"

/**
 * @brief QML-facing façade that exposes the Mastodon client API.
 *
 * Registered as a QML element via QML_ELEMENT — instantiate in QML directly:
 *   MastodonClient { id: client }
 */
class MastodonClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool authenticated READ isAuthenticated NOTIFY authenticatedChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit MastodonClient(QObject *parent = nullptr);

    [[nodiscard]] bool isAuthenticated() const;
    [[nodiscard]] QString errorMessage() const;

    /** Step 1: open the authorisation page in the browser. */
    Q_INVOKABLE void startAuth(const QString &mastodonHost);

    /** Step 2: exchange the auth code for an access token. */
    Q_INVOKABLE void postAuthCode(const QString &authCode);

    /** Post a status (toot). */
    Q_INVOKABLE void postStatus(const QString &status);

signals:
    void authenticatedChanged();
    void errorMessageChanged();
    void statusPosted();

private:
    void setErrorMessage(const QString &message);

    OAuthMastodon *m_oauth = nullptr;
    QNetworkAccessManager *m_networkManager = nullptr;
    bool m_authenticated = false;
    QString m_errorMessage;
};

#endif // MASTODONCLIENT_H
