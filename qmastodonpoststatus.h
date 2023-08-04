#ifndef QMASTODONPOSTSTATUS_H
#define QMASTODONPOSTSTATUS_H

#include "qmastodonnetbase.h"
#include <iterator>

class QMastodonPostStatus : public QMastodonNetBase
{
    Q_OBJECT   
public:
    QMastodonPostStatus(QObject *parent = 0);
    QMastodonPostStatus(OAuthMastodon *oauthMastodon, QObject *parent);
    void postStatus(const QString& status,
              qint64 inReplyToId = 0,
              QList<QString> mediaIds = QList<QString>(),
              bool sensitive = false,
              QString spoilerText = QString(),
              QString statusVisibility = "public");

    /* "direct"     Direct Message
     * "private"    Status can be seen to only followers
     * "unlisted"   Status can be seen only home timeline
     * "public"     Status can be seen all users
     */

signals:
    void postedStatus(const QString& status);

protected:
    void parseJsonFinished(const QJsonDocument &jsonDoc);
};

#endif // QMASTODONPOSTSTATUS_H
