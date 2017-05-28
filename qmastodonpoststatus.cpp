#include "qmastodonpoststatus.h"

QMastodonPostStatus::QMastodonPostStatus(QObject *parent)
    : QMastodonNetBase(parent)
{
}

QMastodonPostStatus::QMastodonPostStatus(OAuthMastodon *oauthMastodon, QObject *parent)
    :QMastodonNetBase(oauthMastodon, parent)
{
}

void QMastodonPostStatus::postStatus(const QString& status,
                               qint64 inReplyToId,
                               QList<QString> mediaIds,
                               bool sensitive,
                               QString spoilerText,
                               QString statusVisibility)
{
    if (!isAuthenticationEnabled()) {
            qCritical("Authentication is disabled.");
            return;
    }

    QUrl url("https://" + oauthMastodon()->mastodonHost() + "/api/v1/statuses");

    QUrlQuery postData;
    postData.addQueryItem("status", status);

    if(inReplyToId != 0)
        postData.addQueryItem("in_reply_to_id", QString::number(inReplyToId));

    if(!mediaIds.isEmpty()){
        QListIterator<QString> i(mediaIds);
        while(i.hasNext()){
            postData.addQueryItem("media_ids", i.next());
        }
    }

    if(sensitive)
        postData.addQueryItem("sensitive", "true");

    if(!spoilerText.isEmpty())
        postData.addQueryItem("spoiler_text", spoilerText);

    if(statusVisibility != "public")
        postData.addQueryItem("visibility", statusVisibility);

    QByteArray oauthHeader = oauthMastodon()->generateAuthorizationHeader(oauthMastodon()->accessToken());

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", oauthHeader);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = oauthMastodon()->networkAccessManager()->post(request, postData.toString().toUtf8());
    connect(reply, SIGNAL(finished()), this, SLOT(reply()));
}

void QMastodonPostStatus::parseJsonFinished(const QJsonDocument& jsonDoc)
{
    if(jsonDoc.isObject()){
        QMessageBox errMegBox;
        errMegBox.setIcon(QMessageBox::Information);
        errMegBox.setWindowTitle(tr("Succeed"));
        errMegBox.setText(tr("Response Data"));
        errMegBox.setInformativeText(jsonDoc.toVariant().toString());
        errMegBox.exec();
        // emit postedStatus(jsonDoc.toVariant().toString());
    }
}
