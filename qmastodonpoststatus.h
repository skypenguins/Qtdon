#ifndef QMASTODONPOSTSTATUS_H
#define QMASTODONPOSTSTATUS_H

#include "qmastodonnetbase.h"

/**
 * @brief Posts a status (toot) via POST /api/v1/statuses.
 *
 * Visibility values:
 *   "direct"   — Direct message
 *   "private"  — Followers only
 *   "unlisted" — Home timeline only
 *   "public"   — Visible to everyone
 */
class QMastodonPostStatus : public QMastodonNetBase
{
    Q_OBJECT

public:
    explicit QMastodonPostStatus(QObject *parent = nullptr);
    explicit QMastodonPostStatus(OAuthMastodon *oauthMastodon,
                                 QObject *parent = nullptr);

    void postStatus(const QString &status,
                    qint64 inReplyToId = 0,
                    const QStringList &mediaIds = {},
                    bool sensitive = false,
                    const QString &spoilerText = {},
                    const QString &visibility = QStringLiteral("public"));

signals:
    void statusPosted(const QString &response);

protected:
    void parseJsonFinished(const QJsonDocument &jsonDoc) override;
};

#endif // QMASTODONPOSTSTATUS_H
