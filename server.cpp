#include "server.h"
#include "tcpconnection.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

Server::Server(QObject *parent) :
    QObject(parent)
{
}

bool Server::openTCPConnection(MainWindow* mainWindow) {
    connection_ = new TCPConnection(mainWindow);
    connect(connection_, SIGNAL(status(QString)),
            mainWindow->getUi()->server_log_output, SLOT(append(QString)));

    return true;
}

void Server::start() {
    connection_->startServer();
}
