#include "tcpconnection.h"
#include "mainwindow.h"

TCPConnection::TCPConnection(MainWindow* mainWindow) {
    int err = 0;

    connect(mainWindow, SIGNAL(signalWMWSASyncRx(MSG*)),
            this, SLOT(slotProcessWSAEvent(MSG*)));

    wVersionRequested_ = MAKEWORD(2,2);
    if ((err = WSAStartup(wVersionRequested_, &wsaData_)) < 0) {
        throw "TCPConnection::TCPConnection(): Missing WINSOCK2 DLL.";
    }

    setWindowHandle(mainWindow->winId());

    openSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TCPConnection::~TCPConnection() {
    closesocket(socket_);
    WSACleanup();
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
    emit status(QString(output.str().c_str()));

    if (WSAAsyncSelect(clientSocket, getWindowHandle(), WM_WSASYNC,
                       flags) == SOCKET_ERROR) {
        qDebug("TCPConnection::accept(): Error calling WSAAsyncSelect()");
        return false;
    }

    return true;
}

bool TCPConnection::read(MSG * msg) {
    DWORD flags = 0;
    DWORD recvBytes;
    int err = 0;
    WSAOVERLAPPED* ol;
    WSABUF winsockBuff;

    winsockBuff.len = 1024;
    winsockBuff.buf = (char*) calloc(1024, sizeof(char));
    ol = (WSAOVERLAPPED*) calloc(1, sizeof(WSAOVERLAPPED));
    ol->hEvent = (HANDLE) winsockBuff.buf;

    if (WSARecv(msg->wParam, &winsockBuff, 1, &recvBytes, &flags,
                ol, TCPConnection::WorkerRoutine) == SOCKET_ERROR) {
        if ((err = WSAGetLastError()) != WSA_IO_PENDING) {
            qDebug("TCPConnection::read(): WSARecv() failed with error %d",
                   err);
            return false;
        }
    }

    return true;
}

bool TCPConnection::close(MSG * msg) {
    qDebug("TCPConnection::close().");

    QString output(": disconnected.");
    emit status(output);

    closesocket(msg->wParam);

    return true;
}
