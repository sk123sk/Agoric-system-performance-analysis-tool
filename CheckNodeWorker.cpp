#include "CheckNodeWorker.h"
#include <iostream>
#include <QEventLoop>
#include <QThread>
using namespace std;

CheckNodeWorker::CheckNodeWorker()
{
    _networkManager=new QNetworkAccessManager;
}

void CheckNodeWorker::login() {
     std::string ip;
     std::string login;
     std::string password;
     std::string tg_chat_id;
     std::string tg_bot_id;
     cout<<"Node ip:"<<endl;
     cin>>ip;
     _node_ip=ip.c_str();
     cout<<"Login:"<<endl;
     cin>>login;
     _login=login.c_str();
     cout<<"Password:"<<endl;
     cin>>password;
     _password=password.c_str();
     cout<<"Telegram chat_id:"<<endl;
     cin>>tg_chat_id;
     _tg_chat_id=tg_chat_id.c_str();
     cout<<"Telegram bot_id:"<<endl;
     cin>>tg_bot_id;
     _tg_bot_id=tg_bot_id.c_str();
     qDebug()<<"------------";
     checkPeers();
}

void CheckNodeWorker::checkPeers()
{
    int port = 22;
    success = ssh.Connect(_node_ip.toUtf8(),port);
    if (success != true) {
        std::cout << ssh.lastErrorText() << "\r\n";
        qDebug()<<ssh.lastErrorText();
        return;
    }
    //  Authenticate using login/password:
    success = ssh.AuthenticatePw(_login.toUtf8(),_password.toUtf8());
    if (success != true) {
        qDebug()<<ssh.lastErrorText();
        std::cout << ssh.lastErrorText() << "\r\n";
        return;
    }
    QList<QString> commands;
    commands.push_back("curl -s 127.0.0.1:26657/net_info  | grep n_peers");
    foreach(QString cmd, commands){
        int channel1 = ssh.QuickCmdSend(cmd.toUtf8());
        if (channel1 < 0) {
            qDebug()<< ssh.lastErrorText() << "\r\n";
            return;
        }
        int pollTimeoutMs = 1000;
        int numFinished = 0;
        while (numFinished < 1) {
            int channel = ssh.QuickCmdCheck(pollTimeoutMs);
            if (channel == -2) {
                qDebug()<<channel<<"erorr:"<< ssh.lastErrorText();
                return;
            }
            if (channel >= 0) {
                QString output = ssh.getReceivedText(channel,"ansi");
                numFinished = numFinished + 1;
                QString peers = output.mid(output.lastIndexOf(":")+3,output.lastIndexOf(",")-2-(output.lastIndexOf(":")+2));
                cout<<"Peers:"<<peers.toStdString()<<endl;
                if (peers.toInt()==0)
                    notifyTelegram("0 peers");
            }
        }
    }
    checkStatus();
}

void CheckNodeWorker::checkStatus()
{
    int port = 22;
    success = ssh.Connect(_node_ip.toUtf8(),port);
    if (success != true) {
        std::cout << ssh.lastErrorText() << "\r\n";
        qDebug()<<ssh.lastErrorText();
        return;
    }
    //  Authenticate using login/password:
    success = ssh.AuthenticatePw(_login.toUtf8(),_password.toUtf8());
    if (success != true) {
        qDebug()<<ssh.lastErrorText();
        std::cout << ssh.lastErrorText() << "\r\n";
        return;
    }
    QList<QString> commands;
    commands.push_back(QString("/%1/go/bin/ag-cosmos-helper status").arg(_login));
    foreach(QString cmd, commands){
        int channel1 = ssh.QuickCmdSend(cmd.toUtf8());
        if (channel1 < 0) {
            qDebug()<< ssh.lastErrorText() << "\r\n";
            return;
        }
        int pollTimeoutMs = 1000;
        int numFinished = 0;
        while (numFinished < 1) {
            int channel = ssh.QuickCmdCheck(pollTimeoutMs);
            if (channel == -2) {
                qDebug()<<channel<<"erorr:"<< ssh.lastErrorText();
                return;
            }
            if (channel >= 0) {
                QString output = ssh.getReceivedText(channel,"ansi");
                numFinished = numFinished + 1;
                QByteArray data = output.toUtf8();
                QJsonObject syncInfo = QJsonDocument::fromJson(data).object();
                QString latest_block_time_ms = syncInfo["SyncInfo"].toObject()["latest_block_time"].toString();
                QString time_s= latest_block_time_ms.left(latest_block_time_ms.indexOf("."));
                QDateTime latest_block_time_s = QDateTime::fromString(time_s, "yyyy-MM-ddThh:mm:ss");
                latest_block_time_s.setOffsetFromUtc(0);
                cout<<"Last block (sec. ago):"<<QString::number(latest_block_time_s.secsTo(QDateTime::currentDateTimeUtc())).toStdString()<<endl;
                if (latest_block_time_s.secsTo(QDateTime::currentDateTimeUtc())>60)
                    notifyTelegram("Block delay > 60 sec.");
            }
        }
    }
    checkCPU();
}

void CheckNodeWorker::checkCPU()
{
    int port = 22;
    success = ssh.Connect(_node_ip.toUtf8(),port);
    if (success != true) {
        std::cout << ssh.lastErrorText() << "\r\n";
        qDebug()<<ssh.lastErrorText();
        return;
    }
    //  Authenticate using login/password:
    success = ssh.AuthenticatePw(_login.toUtf8(),_password.toUtf8());
    if (success != true) {
        qDebug()<<ssh.lastErrorText();
        std::cout << ssh.lastErrorText() << "\r\n";
        return;
    }
    QList<QString> commands;
        commands.push_back("cat /proc/loadavg");
        foreach(QString cmd, commands){
            int channel1 = ssh.QuickCmdSend(cmd.toUtf8());
            if (channel1 < 0) {
                qDebug()<< ssh.lastErrorText() << "\r\n";
                return;
            }
            int pollTimeoutMs = 1000;
            int numFinished = 0;
            while (numFinished < 1) {
                int channel = ssh.QuickCmdCheck(pollTimeoutMs);
                if (channel == -2) {
                    qDebug()<<channel<<"erorr:"<< ssh.lastErrorText();
                    return;
                }
                if (channel >= 0) {
                    QString output = ssh.getReceivedText(channel,"ansi");
                    double cpu_usage = output.mid(output.indexOf(" ")+1, 4).toDouble();
                    qDebug()<<"CPU usage: "<<cpu_usage*100<<"%";
                    numFinished = numFinished + 1;
                   if (cpu_usage > 0.9)
                        notifyTelegram("5min avg CPU usage > 90%");
                }
            }
        }
        checkMemory();
}

void CheckNodeWorker::checkMemory()
{
    int port = 22;
    success = ssh.Connect(_node_ip.toUtf8(),port);
    if (success != true) {
        std::cout << ssh.lastErrorText() << "\r\n";
        qDebug()<<ssh.lastErrorText();
        return;
    }
    //  Authenticate using login/password:
    success = ssh.AuthenticatePw(_login.toUtf8(),_password.toUtf8());
    if (success != true) {
        qDebug()<<ssh.lastErrorText();
        std::cout << ssh.lastErrorText() << "\r\n";
        return;
    }
    QList<QString> commands;
       commands.push_back("cat /proc/meminfo");
       foreach(QString cmd, commands){
           int channel1 = ssh.QuickCmdSend(cmd.toUtf8());
           if (channel1 < 0) {
               qDebug()<< ssh.lastErrorText() << "\r\n";
               return;
           }
           int pollTimeoutMs = 1000;
           int numFinished = 0;
           while (numFinished < 1) {
               int channel = ssh.QuickCmdCheck(pollTimeoutMs);
               if (channel == -2) {
                   qDebug()<<channel<<"erorr:"<< ssh.lastErrorText();
                   return;
               }
               if (channel >= 0) {
                   QString output = ssh.getReceivedText(channel,"ansi");
                   QString memTotal = output.mid(output.indexOf("MemTotal:")+9,output.indexOf("kB")-output.indexOf("MemTotal:")-9).remove(" ");
                   QString memAvailable = output.mid(output.indexOf("MemAvailable:")+13,output.indexOf("Buffers")-3-output.indexOf("MemAvailable:")-13).remove(" ");
                   double memoryUsage = 1- memAvailable.toDouble() / memTotal.toDouble();
                   qDebug()<<"Memory usage: "<<memoryUsage*100<<"%";
                   numFinished = numFinished + 1;
                   if (memoryUsage>0.85)
                       notifyTelegram("High RAM usage");
               }
           }
       }
    QThread::currentThread()->msleep(30000);
    checkPeers();
}

void CheckNodeWorker::notifyTelegram(QString msg)
{
    QUrlQuery query;
    QUrl url=QUrl(QString("https://api.telegram.org/%1/sendMessage").arg(_tg_bot_id));
    query.addQueryItem("chat_id", QString("%1").arg(_tg_chat_id).toUtf8());
    query.addQueryItem("text", msg.toUtf8());
    query.addQueryItem("disable_web_page_preview", "true");
    url.setQuery(query);
    QNetworkRequest request=QNetworkRequest(url);
    QNetworkReply* publicReply = _networkManager->get(request);
    QEventLoop loop3;
    QTimer timer3;
    timer3.start(5000);
    QObject::connect(publicReply, SIGNAL(finished()), &loop3, SLOT(quit()));
    QObject::connect(&timer3, SIGNAL(timeout()), &loop3, SLOT(quit()));
    loop3.exec();
    publicReply->deleteLater();
}
