#include "server.h"
#include "tcpconnection.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

Server::Server(MainWindow* mainWindow) : mainWindow_(mainWindow) {}

bool Server::openTCPConnection() {
    connection_ = new TCPConnection(mainWindow_, FD_ACCEPT);
    connect(connection_, SIGNAL(status(QString)),
            mainWindow_->getUi()->server_log_output, SLOT(append(QString)));

    return true;
}

void Server::start() {
    connection_->startServer();
}
