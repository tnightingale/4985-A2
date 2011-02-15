#include "client.h"
//#include "tcpconnection.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"
#include "udpsocket.h"

Client::Client(MainWindow* mainWindow) : mainWindow_(mainWindow) {}
/*
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
*/

void Client::writeTCP(char* hostName, int port, char * data, size_t data_len) {
    PHOSTENT host;
    SOCKADDR_IN serverSockAddrIn;
    TCPSocket* tcpSocket;
    int err = 0;
    HWND hWnd = mainWindow_->winId();

    tcpSocket = new TCPSocket(hWnd);

    if (!Socket::init(tcpSocket->getSocket(), hWnd, FD_WRITE | FD_CLOSE)) {
        return;
    }

    connect(mainWindow_, SIGNAL(signalWMWSASyncRx(PMSG)),
            tcpSocket, SLOT(slotProcessWSAEvent(PMSG)));

    if ((host = gethostbyname(hostName)) == NULL) {
        err = GetLastError();
        qDebug("Client::writeTCP(): Unknown server address. Error: %d.", err);
        return;
    }

    qDebug() << "Client::writeTCP(): Client connecting to:" << endl
             << "\tHost: " << host->h_name << endl
             << "\tPort: " << port;

    memcpy((char*) &serverSockAddrIn.sin_addr, host->h_addr, host->h_length);
    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);

    tcpSocket->setDataSource(data, data_len);

    if (!tcpSocket->connectRemote(&serverSockAddrIn)) {
        return;
    }

}
