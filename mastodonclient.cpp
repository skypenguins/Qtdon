#include "mastodonclient.h"

MastodonClient::MastodonClient(QObject *parent)
    : QObject(parent)
    , m_oauth(new OAuthMastodon(this))
    , m_networkManager(new QNetworkAccessManager(this))
{
    m_oauth->setNetworkAccessManager(m_networkManager);

    connect(m_oauth, &OAuthMastodon::accessTokenReceived, this, [this]() {
        m_authenticated = true;
        emit authenticatedChanged();
    });

    connect(m_oauth, &OAuthMastodon::errorOccurred, this,
            &MastodonClient::setErrorMessage);
}

// ---------------------------------------------------------------------------
// Property getters
// ---------------------------------------------------------------------------

bool MastodonClient::isAuthenticated() const
{
    return m_authenticated;
}

QString MastodonClient::errorMessage() const
{
    return m_errorMessage;
}

// ---------------------------------------------------------------------------
// Q_INVOKABLE methods (called from QML)
// ---------------------------------------------------------------------------

void MastodonClient::startAuth(const QString &mastodonHost)
{
    m_oauth->requestAuthorization(mastodonHost);
}

void MastodonClient::postAuthCode(const QString &authCode)
{
    m_oauth->requestAccessToken(authCode);
}

void MastodonClient::postStatus(const QString &status)
{
    if (!m_authenticated) {
        setErrorMessage(tr("Not authenticated. Please log in first."));
        return;
    }

    auto *poster = new QMastodonPostStatus(m_oauth, this);

    connect(poster, &QMastodonPostStatus::statusPosted,
            this, [this, poster]() {
                poster->deleteLater();
                emit statusPosted();
            });

    connect(poster, &QMastodonNetBase::error,
            this, [this, poster](QMastodonNetBase::ErrorCode /*code*/,
                                 const QString &msg) {
                poster->deleteLater();
                setErrorMessage(msg);
            });

    poster->postStatus(status);
}

// ---------------------------------------------------------------------------
// Internals
// ---------------------------------------------------------------------------

void MastodonClient::setErrorMessage(const QString &message)
{
    if (m_errorMessage == message)
        return;
    m_errorMessage = message;
    emit errorMessageChanged();
}
