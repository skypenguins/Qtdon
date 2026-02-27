#include "qmastodonpoststatus.h"

#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

QMastodonPostStatus::QMastodonPostStatus(QObject *parent)
    : QMastodonNetBase(parent)
{
}

QMastodonPostStatus::QMastodonPostStatus(OAuthMastodon *oauthMastodon,
                                         QObject *parent)
    : QMastodonNetBase(oauthMastodon, parent)
{
}

void QMastodonPostStatus::postStatus(const QString &status,
                                     qint64 inReplyToId,
                                     const QStringList &mediaIds,
                                     bool sensitive,
                                     const QString &spoilerText,
                                     const QString &visibility)
{
    if (!isAuthenticationEnabled()) {
        qCritical("Authentication is disabled.");
        return;
    }

    auto *oauth = oauthMastodon();
    if (!oauth || !oauth->networkAccessManager()) {
        qCritical("OAuth or network manager not available.");
        return;
    }

    const QUrl url(QStringLiteral("https://") + oauth->mastodonHost()
                   + QStringLiteral("/api/v1/statuses"));

    QUrlQuery postData;
    postData.addQueryItem(QStringLiteral("status"), status);

    if (inReplyToId != 0)
        postData.addQueryItem(QStringLiteral("in_reply_to_id"),
                              QString::number(inReplyToId));

    for (const auto &mediaId : mediaIds)
        postData.addQueryItem(QStringLiteral("media_ids[]"), mediaId);

    if (sensitive)
        postData.addQueryItem(QStringLiteral("sensitive"),
                              QStringLiteral("true"));

    if (!spoilerText.isEmpty())
        postData.addQueryItem(QStringLiteral("spoiler_text"), spoilerText);

    if (visibility != QStringLiteral("public"))
        postData.addQueryItem(QStringLiteral("visibility"), visibility);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         OAuthMastodon::generateBearerHeader(oauth->accessToken()));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));

    auto *reply = oauth->networkAccessManager()->post(
        request, postData.toString().toUtf8());

    connect(reply, &QNetworkReply::finished,
            this,  &QMastodonPostStatus::onReplyFinished);
}

void QMastodonPostStatus::parseJsonFinished(const QJsonDocument &jsonDoc)
{
    if (jsonDoc.isObject()) {
        emit statusPosted(
            QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact)));
    }
}
