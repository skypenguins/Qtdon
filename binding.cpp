#include "binding.h"

Binding::Binding(QObject* parent)
    : QObject(parent)
{
    m_oauthMastodon = new OAuthMastodon(this);
    m_oauthMastodon->setNetworkAccessManager(new QNetworkAccessManager(this));
}

void Binding::startAuth(const QString &mastodonHost)
{
    m_oauthMastodon->requestAuthorization(mastodonHost);
}

void Binding::postAuthCode(const QByteArray& authCode)
{
    m_oauthMastodon->requestAccessToken(authCode);
}

void Binding::postStatus(const QString &status)
{
    QMastodonPostStatus* statusPost = new QMastodonPostStatus(m_oauthMastodon, this);
    statusPost->postStatus(status);
}
