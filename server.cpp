#include "server.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"
#include "udpsocket.h"

Server::~Server() {}

Server::Server(MainWindow* mainWindow) : mainWindow_(mainWindow) {
    file_ = new QFile(mainWindow->getUi()->file_dest_val->text());
    if (!file_->open(QIODevice::WriteOnly)) {
        mainWindow_->log(QString("Error opening file: ")
                         + mainWindow->getUi()->file_dest_val->text());
        throw "Server::Server; Cannot open file for write.";
    }
}

bool Server::listenTCP(int port) {
    HWND hWnd = mainWindow_->winId();
    SOCKADDR_IN serverSockAddrIn;

    TCPSocket* tcpSocket = new TCPSocket(hWnd);
    tcpSocket->setDataStream(file_);

    connect(tcpSocket, SIGNAL(status(QString)),
            mainWindow_->getUi()->server_log_output, SLOT(append(QString)));

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            tcpSocket, SLOT(slotProcessWSAEvent(PMSG)));

    if (!Socket::init(tcpSocket->getSocket(), hWnd, FD_CLOSE | FD_ACCEPT)) {
        return false;
    }

    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);
    serverSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    mainWindow_->log(QString("Starting TCP server..."));

    return tcpSocket->listen(&serverSockAddrIn);
}

bool Server::listenUDP(int port) {
    HWND hWnd = mainWindow_->winId();
    SOCKADDR_IN serverSockAddrIn;
    UDPSocket* udpSocket = new UDPSocket(hWnd);
    udpSocket->setDataStream(file_);

    connect(udpSocket, SIGNAL(status(QString)),
            mainWindow_->getUi()->server_log_output, SLOT(append(QString)));

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            udpSocket, SLOT(slotProcessWSAEvent(PMSG)));

    if (!Socket::init(udpSocket->getSocket(), hWnd, FD_READ | FD_CLOSE)) {
        return false;
    }

    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);
    serverSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    mainWindow_->log(QString("Starting UDP server..."));
    return udpSocket->listen(&serverSockAddrIn);
}

void Server::slotUpdateStats() {}
