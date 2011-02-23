#include "client.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"
#include "udpsocket.h"

Client::Client(MainWindow* mainWindow, SETTINGS * settings)
: mainWindow_(mainWindow) {
    settings_ = settings;
}

Client::~Client() {
    delete socket_;
}

void Client::sendTCP(QString address, int port, size_t packetSize,
                     size_t numPackets) {
    HWND hWnd = mainWindow_->winId();
    char * data = (char *) malloc(packetSize * numPackets * sizeof(char));
    memset(data, 'p', packetSize * numPackets);

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

    if (!Socket::init(tcpsocket->getSocket(), mainWindow_->winId(), FD_WRITE | FD_CLOSE)) {
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
    char * data = (char *) malloc(packetSize * numPackets * sizeof(char));
    memset(data, 'p', packetSize * numPackets);

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
    QTimer * statUpdater = mainWindow_->getTimer();
    connect(socket_, SIGNAL(status(QString)),
            mainWindow_->getUi()->server_log_output, SLOT(append(QString)));

    connect(socket_, SIGNAL(signalStatsChanged(STATS)),
            mainWindow_, SLOT(slotUpdateClientStats(STATS)));
    connect(statUpdater, SIGNAL(timeout()),
            this, SLOT(slotUpdateStats()));
    connect(socket_, SIGNAL(signalSocketClosed()),
            this, SLOT(deleteLater()));
}

void Client::slotUpdateStats() {
    STATS stats = socket_->getStats();
    mainWindow_->slotUpdateClientStats(stats);
}
