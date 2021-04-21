#include <QCoreApplication>
#include "CheckNodeWorker.h"
#include <QThread>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CheckNodeWorker* _checkWorker = new CheckNodeWorker();
    QThread* checkworkerThread = new QThread();
    _checkWorker->moveToThread(checkworkerThread);
    checkworkerThread->start();
    _checkWorker->login();
    return a.exec();
}

