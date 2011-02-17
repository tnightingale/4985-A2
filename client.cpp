#include "client.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"
#include "udpsocket.h"

Client::Client(MainWindow* mainWindow) : mainWindow_(mainWindow) {}

void Client::sendTCP() {
    HWND hWnd = mainWindow_->winId();
    char * data = (char *) malloc(DATABUFSIZE * sizeof(char));
    memset(data, 'p', DATABUFSIZE);

    socket_ = new TCPSocket(hWnd);
    socket_->setDataStream(data);
    writeTCP("192.168.0.21", 7000);
}

void Client::writeTCP(char* hostName, int port) {
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

    qDebug() << "Client::writeTCP(): Client connecting to:" << endl
             << "\tHost: " << host->h_name << endl
             << "\tPort: " << port;

    memcpy((char*) &serverSockAddrIn.sin_addr, host->h_addr, host->h_length);
    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);

    if (!tcpsocket->connectRemote(&serverSockAddrIn)) {
        return;
    }
}
