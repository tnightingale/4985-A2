#include "server.h"
#include "tcpconnection.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"
#include "udpsocket.h"

Server::Server(MainWindow* mainWindow) : mainWindow_(mainWindow) {}

bool Server::openTCPConnection() {
    connection_ = new TCPConnection(mainWindow_, FD_ACCEPT);
    connect(connection_, SIGNAL(status(QString)),
            mainWindow_->getUi()->server_log_output, SLOT(append(QString)));

    return true;
}

bool Server::listenTCP(int port) {
    HWND hWnd = mainWindow_->winId();
    SOCKADDR_IN serverSockAddrIn;
    TCPSocket* tcpSocket = new TCPSocket(hWnd);

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            tcpSocket, SLOT(slotProcessWSAEvent(PMSG)));

    if (!Socket::init(tcpSocket->getSocket(), hWnd, FD_CLOSE | FD_ACCEPT)) {
        return false;
    }

    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);
    serverSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    return tcpSocket->listen(&serverSockAddrIn);
}

bool Server::listenUDP(int port) {
    HWND hWnd = mainWindow_->winId();
    SOCKADDR_IN serverSockAddrIn;
    UDPSocket* udpSocket = new UDPSocket(hWnd);

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            udpSocket, SLOT(slotProcessWSAEvent(PMSG)));

    if (!Socket::init(udpSocket->getSocket(), hWnd, FD_READ | FD_WRITE | FD_CLOSE)) {
        return false;
    }

    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);
    serverSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    return udpSocket->listen(&serverSockAddrIn);
}

void Server::start() {
    connection_->startServer();
}
