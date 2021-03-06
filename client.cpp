#include "client.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"
#include "udpsocket.h"

Client::Client(MainWindow* mainWindow, SETTINGS * settings)
: mainWindow_(mainWindow) {
    settings_ = settings;
    Socket::initStats(stats_);
}

Client::~Client() {
    delete socket_;
}

void Client::sendTCP(QString address, int port, size_t packetSize,
                     size_t numPackets) {
    HWND hWnd = mainWindow_->winId();

    QByteArray * data = new QByteArray(packetSize * numPackets, 'P');
    socket_ = new TCPSocket(hWnd);
    socket_->setDataStream(data);
    socket_->setPacketSize(packetSize);

    initGui();

    writeTCP(address.toAscii().data(), port);
}

void Client::sendFileTCP(QString address, int port, size_t packetSize,
                         QString filepath) {
    HWND hWnd = mainWindow_->winId();
    QFile * file = new QFile(filepath);
    file->open(QIODevice::ReadOnly);

    socket_ = new TCPSocket(hWnd);
    socket_->setDataStream(file);
    socket_->setPacketSize(packetSize);

    initGui();

    writeTCP(address.toAscii().data(), port);
}

void Client::writeTCP(char* hostName, int port) {
    QString output;
    QTextStream log(&output, QIODevice::WriteOnly);

    int err = 0;
    PHOSTENT host;
    SOCKADDR_IN serverSockAddrIn;
    TCPSocket * tcpsocket = (TCPSocket*) socket_;

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            tcpsocket, SLOT(slotProcessWSAEvent(PMSG)));

    if (!Socket::init(tcpsocket->getSocket(), mainWindow_->winId(),
                      FD_CONNECT | FD_WRITE | FD_CLOSE)) {
        return;
    }

    if ((host = gethostbyname(hostName)) == NULL) {
        err = GetLastError();
        qDebug("Client::writeTCP(): Unknown server address. Error: %d.", err);
        return;
    }

    log << "Client::writeTCP(): Client connecting to: "
        << host->h_name << ":" << port;
    tcpsocket->outputStatus(output);

    memcpy((char*) &serverSockAddrIn.sin_addr, host->h_addr, host->h_length);
    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);

    if (!tcpsocket->connectRemote(&serverSockAddrIn)) {
        return;
    }
}

void Client::sendUDP(QString address, int port, size_t packetSize,
                     size_t numPackets) {
    HWND hWnd = mainWindow_->winId();

    QByteArray * data = new QByteArray(packetSize * numPackets, 'U');
    socket_ = new UDPSocket(hWnd);
    socket_->setDataStream(data);
    socket_->setPacketSize(packetSize);

    initGui();

    writeUDP(address.toAscii().data(), port);
}

void Client::sendFileUDP(QString address, int port, size_t packetSize,
                         QString filepath) {
    HWND hWnd = mainWindow_->winId();
    QFile * file = new QFile(filepath);
    file->open(QIODevice::ReadOnly);

    socket_ = new UDPSocket(hWnd);
    socket_->setDataStream(file);
    socket_->setPacketSize(packetSize);

    initGui();

    writeUDP(address.toAscii().data(), port);
}

void Client::writeUDP(char* hostName, int port) {
    QString output;
    QTextStream log(&output, QIODevice::WriteOnly);

    int err = 0;
    PHOSTENT host;
    UDPSocket * udpsocket = (UDPSocket*) socket_;

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            udpsocket, SLOT(slotProcessWSAEvent(PMSG)));

    if ((host = gethostbyname(hostName)) == NULL) {
        err = GetLastError();
        qDebug("Client::writeUDP(): Unknown server address. Error: %d.", err);
        return;
    }

    log << "Client::writeUDP(): Client sending to: "
        << host->h_name << ":" << port;
    udpsocket->outputStatus(output);

    udpsocket->setDest(host, port);

    if (!Socket::init(udpsocket->getSocket(), mainWindow_->winId(), FD_WRITE | FD_CLOSE)) {
        return;
    }
}

void Client::initGui() {
    connect(socket_, SIGNAL(status(QString)),
            mainWindow_->getUi()->server_log_output, SLOT(append(QString)));

    connect(socket_, SIGNAL(signalSocketClosed()),
            this, SLOT(deleteLater()));

    connect(socket_, SIGNAL(signalStatsSetBytes(int)),
            this, SLOT(slotStatsSetBytes(int)));
    connect(socket_, SIGNAL(signalStatsSetPackets(int)),
            this, SLOT(slotStatsSetPackets(int)));
    connect(socket_, SIGNAL(signalStatsSetStartTime(int)),
            this, SLOT(slotStatsSetStartTime(int)));
    connect(socket_, SIGNAL(signalStatsSetFinishTime(int)),
            this, SLOT(slotStatsSetFinishTime(int)));
}

void Client::slotStatsSetBytes(int bytes) {
    stats_.totalBytes += bytes;
    slotUpdateStats();
}

void Client::slotStatsSetPackets(int packets) {
    stats_.totalPackets += packets;
    slotUpdateStats();
}

void Client::slotStatsSetStartTime(int time) {
    stats_.startTime = time;
    slotUpdateStats();
}

void Client::slotStatsSetFinishTime(int time) {
    stats_.finishTime = time;
    slotUpdateStats();
}

void Client::slotUpdateStats() {
    mainWindow_->slotUpdateClientStats(stats_);
}
