#include "connection.h"
#include <QDebug>


bool Connection::openSocket(int addressFamily, int connectionType,
                            int protocol) {
    int flags = FD_ACCEPT | FD_CLOSE;
    int err = 0;
    addressFamily_ = addressFamily;

    if ((socket_ = WSASocket(addressFamily_, connectionType, protocol, NULL, 0,
                             WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
        throw "Connection::openSocket(): Can't create socket.";
        return false;
    }

    if ((err = WSAAsyncSelect(socket_, hWnd_, WM_WSASYNC,
                              flags)) == SOCKET_ERROR) {
        throw "Connection::startServer(): Error setting up async select.";
        return false;
    }

    return true;
}


bool Connection::startServer(int port) {
    int err = 0;
    std::ostringstream output;
    SOCKADDR_IN serverSockAddrIn;


    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);
    serverSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((err = bind(socket_, (PSOCKADDR) &serverSockAddrIn,
              sizeof(serverSockAddrIn)) == SOCKET_ERROR)) {
        qDebug("Connection::startServer(): Can't bind name to socket.");
        return false;
    }

    qDebug("Connection::startServer(): Server bound port %d to TCP socket",
           port);
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

    serverSockAddrIn.sin_family = AF_INET;
    serverSockAddrIn.sin_port = htons(port);

    if ((host = gethostbyname(hostName)) == NULL) {
        qDebug("Connection::startClient(): Unknown server address");
        return false;
    }

    qDebug() << "Connection::startClient(): Client connecting to host: "
             << host->h_name;
    memcpy((char*) &serverSockAddrIn.sin_addr, host->h_name, host->h_length);

    if ((::connect(socket_, (PSOCKADDR) &serverSockAddrIn,
                   sizeof(SOCKADDR_IN))) < 0) {
        qDebug("Connection::startClient(): Failed to connect to server.");
        return false;
    }

    return true;
}

bool Connection::sendMessage(char* data, int data_length) {
    int err = 0;
    DWORD numBytesSent = 0;
    WSAOVERLAPPED* ol;
    WSABUF winsockBuff;

    winsockBuff.len = 1024;
    winsockBuff.buf = (char*) malloc(1024 * sizeof(char));

    memset(winsockBuff.buf, 'p', 1024);

    ol = (WSAOVERLAPPED*) calloc(1, sizeof(WSAOVERLAPPED));
    ol->hEvent = (HANDLE) winsockBuff.buf;


    if ((numBytesSent = WSASend(socket_, &winsockBuff, 1, &numBytesSent, 0,
                                ol, Connection::sendCompletionRoutine))
            == SOCKET_ERROR) {
        err = WSAGetLastError();
        qDebug("TCPConnection::sendMessage(); Error: %d", err);
        return false;
    }

    return true;
}
