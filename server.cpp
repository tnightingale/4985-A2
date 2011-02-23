#include "server.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"
#include "udpsocket.h"

Server::~Server() {
    if (socket_ != NULL) {
        mainWindow_->log(QString("Stopping server..."));
        delete socket_;
    }
}

Server::Server(MainWindow* mainWindow) : mainWindow_(mainWindow) {
    file_ = new QFile(mainWindow->getUi()->file_dest_val->text());
    if (!file_->open(QIODevice::WriteOnly)) {
        mainWindow_->log(QString("Error opening file: ")
                         + mainWindow->getUi()->file_dest_val->text());
        throw "Server::Server; Cannot open file for write.";
    }

    Socket::initStats(stats_);
}

bool Server::listenTCP(int port) {
    HWND hWnd = mainWindow_->winId();
    SOCKADDR_IN serverSockAddrIn;

    socket_ = new TCPSocket(hWnd);
    socket_->setDataStream(file_);

    initGui();

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            socket_, SLOT(slotProcessWSAEvent(PMSG)));

    if (!Socket::init(socket_->getSocket(), hWnd, FD_CLOSE | FD_ACCEPT)) {
        return false;
    }

    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);
    serverSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    mainWindow_->log(QString("Starting TCP server..."));

    return socket_->listen(&serverSockAddrIn);
}

bool Server::listenUDP(int port) {
    HWND hWnd = mainWindow_->winId();
    SOCKADDR_IN serverSockAddrIn;
    socket_ = new UDPSocket(hWnd);
    socket_->setDataStream(file_);

    initGui();

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            socket_, SLOT(slotProcessWSAEvent(PMSG)));

    if (!Socket::init(socket_->getSocket(), hWnd, FD_READ | FD_CLOSE)) {
        return false;
    }

    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);
    serverSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    mainWindow_->log(QString("Starting UDP server..."));
    return socket_->listen(&serverSockAddrIn);
}

void Server::initGui() {
    connect(socket_, SIGNAL(status(QString)),
            mainWindow_->getUi()->server_log_output, SLOT(append(QString)));

    connect(socket_, SIGNAL(signalStatsSetBytes(int)),
            this, SLOT(slotStatsSetBytes(int)));
    connect(socket_, SIGNAL(signalStatsSetPackets(int)),
            this, SLOT(slotStatsSetPackets(int)));
    connect(socket_, SIGNAL(signalStatsStartTime(int)),
            this, SLOT(slotStatsSetStartTime(int)));
    connect(socket_, SIGNAL(signalStatsSetFinishTime(int)),
            this, SLOT(slotStatsSetFinishTime(int)));
}

void Server::slotStatsSetBytes(int bytes) {
    stats_.totalBytes += bytes;
    slotUpdateStats();
}

void Server::slotStatsSetPackets(int packets) {
    stats_.totalPackets += packets;
    slotUpdateStats();
}

void Server::slotStatsSetStartTime(int time) {
    stats_.startTime = time;
    slotUpdateStats();
}

void Server::slotStatsSetFinishTime(int time) {
    stats_.finishTime = time;
    slotUpdateStats();
}

void Server::slotUpdateStats() {
    mainWindow_->slotUpdateServerStats(stats_);
}
