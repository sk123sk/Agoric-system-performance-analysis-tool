#ifndef CHECKNODEWORKER_H
#define CHECKNODEWORKER_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <Chilkat_C.h>
#include <CkSsh.h>
#include <CkStringBuilder.h>
#include <iostream>
#include <CkGlobal.h>
#include <QTimer>

class CheckNodeWorker: public QObject
{
    Q_OBJECT
public:
    CheckNodeWorker();

signals:
    void checkingStateFinished(QString,int);

public slots:
    void login();
    void checkPeers();
    void checkStatus();
    void checkCPU();
    void checkMemory();
    void notifyTelegram(QString msg);

private:
    QNetworkAccessManager* _networkManager;
    QUrlQuery _query;
    bool _isReplyFinished;
    CkSsh ssh;
    bool success;
    int _sshChannel;
    QString _node_ip;
    QString _login;
    QString _password;
    QString _tg_chat_id;
    QString _tg_bot_id;
};

#endif // CHECKNODEWORKER_H
