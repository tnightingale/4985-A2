#include "connection.h"
#include <QDebug>


bool Connection::openSocket(int addressFamily, int connectionType,
                            int protocol, int flags) {
    int err = 0;
    addressFamily_ = addressFamily;

    socket_ = NULL;
    qDebug("Connection::openSocket(): Opening a new socket.");
    if ((socket_ = WSASocket(addressFamily_, connectionType, protocol, NULL, 0,
                             WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
        throw "Connection::openSocket(): Can't create socket.";
        return false;
    }
    qDebug("Connection::openSocket(): SocketCreated: %d", (int) socket_);

    //if ((err = WSAAsyncSelect(socket_, hWnd_, WM_WSASYNC,
    //                          flags)) == SOCKET_ERROR) {
    //    throw "Connection::startServer(): Error setting up async select.";
    //    return false;
    //}

    return true;
}


bool Connection::startServer(int port) {
    int err = 0;
    std::ostringstream output;
    SOCKADDR_IN serverSockAddrIn;
    int flags = FD_CLOSE | FD_ACCEPT | FD_READ;

    if ((err = WSAAsyncSelect(socket_, hWnd_, WM_WSASYNC,
                              flags)) == SOCKET_ERROR) {
        throw "Connection::startServer(): Error setting up async select.";
        return false;
    }


    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);
    serverSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((err = bind(socket_, (PSOCKADDR) &serverSockAddrIn,
              sizeof(serverSockAddrIn)) == SOCKET_ERROR)) {
        qDebug("Connection::startServer(): Can't bind to socket. Error: %d",
               WSAGetLastError());
        return false;
    }

    // TODO: Might want to call linger here.

    qDebug("Connection::startServer(): Server bound port %d to TCP socket %d",
           port, (int) socket_);
    output << "Server bound port " << port << " to TCP socket.";
    emit status(QString(output.str().c_str()));

    if ((err = ::listen(socket_, 5)) == SOCKET_ERROR) {
        err = WSAGetLastError();
        qDebug("Connection::listen(); Error: %d.", err);
        return false;
    }

    return true;
}

bool Connection::startClient(char* hostName, int port) {
    PHOSTENT host;
    SOCKADDR_IN serverSockAddrIn;
    int err = 0;

    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);

    if ((host = gethostbyname(hostName)) == NULL) {
        qDebug("Connection::startClient(): Unknown server address");
        return false;
    }

    qDebug() << "Connection::startClient(): Client connecting to host: "
             << host->h_name;
    qDebug() << "Port: " << port;
    memcpy((char*) &serverSockAddrIn.sin_addr, host->h_addr, host->h_length);

    // allow the TCP packets to linger for up to 10 seconds
    LINGER* linger = (LINGER*) malloc(sizeof(LINGER));
    linger->l_onoff = 1;
    linger->l_linger = 10;
    setsockopt(socket_, SOL_SOCKET, SO_LINGER, (char*) linger, sizeof(linger));

    if ((err = ::connect(socket_, (PSOCKADDR) &serverSockAddrIn,
                   sizeof(SOCKADDR_IN))) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            qDebug("Connection::startClient(): Failed to connect to server. Error: %d", WSAGetLastError());
            return false;
        }
    }

    return true;
}

bool Connection::sendMessage(char* data, int data_length) {
    int err = 0;
    DWORD numBytesSent = 0;
    WSAOVERLAPPED* ol;
    WSABUF winsockBuff;

    winsockBuff.len = data_len_;
    winsockBuff.buf = data_;

    memset(winsockBuff.buf, 'p', 1024);

    ol = (WSAOVERLAPPED*) calloc(1, sizeof(WSAOVERLAPPED));
    ol->hEvent = (HANDLE) winsockBuff.buf;

    if ((numBytesSent = WSASend(socket_, &winsockBuff, 1, &numBytesSent, 0, ol,
                                Connection::sendCompletionRoutine))
            == SOCKET_ERROR) {
        err = WSAGetLastError();
        qDebug("Connection::sendMessage(); Error: %d", err);
        return false;
    }

    shutdown(socket_, SD_BOTH);

    return true;
}
