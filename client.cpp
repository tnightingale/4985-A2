#include "client.h"
#include "tcpconnection.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

Client::Client(MainWindow* mainWindow) : mainWindow_(mainWindow) {}

bool Client::openTCPConnection() {
    connection_ = new TCPConnection(mainWindow_, FD_WRITE);
    connect(connection_, SIGNAL(signalCloseConnection()),
            this, SLOT(deleteLater()));
    return true;
}

void Client::start() {
    char* buff = (char*) malloc(1024 * sizeof(char));
    memset(buff, 'p', 1024);

    connection_->setData(buff, 1024);
    connection_->startClient("127.0.0.1", 7000);
}
