#include "client.h"
#include "tcpconnection.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

Client::Client(QObject *parent) :
    QObject(parent)
{
}

bool Client::openTCPConnection(MainWindow* mainWindow) {
    connection_ = new TCPConnection(mainWindow);

    return true;
}

void Client::start() {
    connection_->startClient("127.0.0.1", 7001);
    connection_->sendMessage("AAAA", 4);
}
