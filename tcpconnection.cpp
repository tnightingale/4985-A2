#include "tcpconnection.h"

TCPConnection::TCPConnection(HWND hWnd) {
    int err = 0;
    int flags = FD_ACCEPT | FD_CLOSE;

    wVersionRequested_ = MAKEWORD(2,2);
    if ((err = WSAStartup(wVersionRequested_, &wsaData_)) < 0) {
        throw "TCPConnection::TCPConnection(): Missing WINSOCK2 DLL.";
    }

    if ((socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        throw "TCPConnection::TCPConnection(): Can't create socket.";
    }

    if ((err = WSAAsyncSelect(socket_, hWnd, WM_WSASYNC,
                              flags)) == SOCKET_ERROR) {
        throw "TCPConnection::TCPConnectio(): Error setting up async select.";
    }

    hWnd_ = hWnd;

}

TCPConnection::~TCPConnection() {
    closesocket(socket_);
    WSACleanup();
}

bool TCPConnection::startServer(int port) {
    int err = 0;
    std::ostringstream output;
    serverSockAddrIn_.sin_family = AF_INET;
    serverSockAddrIn_.sin_port = htons(port);
    serverSockAddrIn_.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((err = bind(socket_, (PSOCKADDR) &serverSockAddrIn_,
              sizeof(serverSockAddrIn_)) < 0)) {
        qDebug("TCPConnection::startServer(): Can't bind name to socket.");
        return false;
    }

    qDebug("TCPConnection::startServer(): Server bound port %d to TCP socket",
           port);
    output << "Server bound port " << port << " to TCP socket.";
    emit display(QString(output.str().c_str()));

    if ((err = ::listen(socket_, 5)) == SOCKET_ERROR) {
        err = WSAGetLastError();
        qDebug("TCPConnection::listen(); Error: %d.", err);
        return false;
    }

    return true;
}

bool TCPConnection::startClient(char* hostName, int port) {
    PHOSTENT host;

    serverSockAddrIn_.sin_family = AF_INET;
    serverSockAddrIn_.sin_port = htons(port);

    if ((host = gethostbyname(hostName)) == NULL) {
        qDebug("TCPConnection::startClient(): Unknown server address");
        return false;
    }

    memcpy((char*) &serverSockAddrIn_.sin_addr, host->h_addr, host->h_length);

    if ((::connect(socket_, (PSOCKADDR) &serverSockAddrIn_,
                   sizeof(SOCKADDR_IN))) < 0) {
        qDebug("TCPConnection::startClient(): Failed to connect to server.");
        return false;
    }

    return true;
}

bool TCPConnection::sendMessage(char* data, int data_length) {
    int err = 0;
    int numBytesSent = 0;

    if ((numBytesSent = send(socket_, data, data_length, 0)) == SOCKET_ERROR) {
        err = WSAGetLastError();
        qDebug("TCPConnection::sendMessage(); Error: %d", err);
        return false;
    }

    return true;
}

bool TCPConnection::slotProcessWSAEvent(MSG * msg) {
    if (WSAGETSELECTERROR(msg->lParam)) {
        qDebug("TCPConnection::slotProcessWSAEvent(): Socket failed with error %d.",
               WSAGETSELECTERROR(msg->lParam));
        return false;
    }

    switch (WSAGETSELECTEVENT(msg->lParam)) {
        case FD_ACCEPT:
            accept(msg);
            break;
        case FD_READ:
            read(msg);
            break;
        case FD_CLOSE:
            close(msg);
    }

    return true;
}

bool TCPConnection::accept(MSG * msg) {
    int flags = FD_READ | FD_WRITE | FD_CLOSE;
    std::ostringstream output;
    SOCKET clientSocket;
    SOCKADDR_IN client;
    int client_length = sizeof(SOCKADDR_IN);

    qDebug("TDConnection::accept().");

    if ((clientSocket = ::accept(msg->wParam, (SOCKADDR *) &client,
                               &client_length)) == INVALID_SOCKET) {
        qDebug("TCPConnection:listen(); Error:");
        return false;
    }

    output << "Remote address (" << inet_ntoa(client.sin_addr)
           << ") connected.";
    emit display(QString(output.str().c_str()));

    if (WSAAsyncSelect(clientSocket, hWnd_, WM_WSASYNC,
                       flags) == SOCKET_ERROR) {
        qDebug("TCPConnection::accept(): Error calling WSAAsyncSelect()");
        return false;
    }

    return true;
}

bool TCPConnection::read(MSG * msg) {
    qDebug("TDConnection::read().");

    QString output(": read.");
    emit display(output);

    return true;
}

bool TCPConnection::close(MSG * msg) {
    qDebug("TCPConnection::close().");

    QString output(": disconnected.");
    emit display(output);

    closesocket(msg->wParam);

    return true;
}
