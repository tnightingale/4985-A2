#include "tcpconnection.h"
#include "mainwindow.h"

TCPConnection::TCPConnection(MainWindow* mainWindow, int extraFlags) {
    int err = 0;
    int flags = FD_CLOSE;
    if (extraFlags > 0) {
        flags |= extraFlags;
    }

    setData(NULL, 0);

    connect(mainWindow, SIGNAL(signalWMWSASyncRx(MSG*)),
            this, SLOT(slotProcessWSAEvent(MSG*)));

    wVersionRequested_ = MAKEWORD(2,2);
    if ((err = WSAStartup(wVersionRequested_, &wsaData_)) < 0) {
        throw "TCPConnection::TCPConnection(): Missing WINSOCK2 DLL.";
    }

    setWindowHandle(mainWindow->winId());

    openSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, flags);
}

TCPConnection::~TCPConnection() {
    qDebug("TCPConnection::~TCPConnection()");
    char * data = NULL;
    if ((getData(data)) > 0) {
        free(data);
    }
    WSACleanup();
}

bool TCPConnection::slotProcessWSAEvent(MSG * msg) {
    if (WSAGETSELECTERROR(msg->lParam)) {
        qDebug("TCPConnection::slotProcessWSAEvent(): %d: Socket failed with error %d.",
              (int) msg->wParam, WSAGETSELECTERROR(msg->lParam));
        return false;
    }

    switch (WSAGETSELECTEVENT(msg->lParam)) {
        case FD_ACCEPT:
            qDebug("TDConnection::slotProcessWSAEvent: %d: FD_ACCEPT.", (int) msg->wParam);
            accept(msg);
            break;

        case FD_READ:
            qDebug("TDConnection::slotProcessWSAEvent: %d: FD_READ.", (int) msg->wParam);
            read(msg);
            break;

        case FD_WRITE:
            qDebug("TDConnection::slotProcessWSAEvent: %d: FD_WRITE.", (int) msg->wParam);
            sendMessage("aaaa", 4);
            break;

        case FD_CLOSE:
            qDebug("TDConnection::slotProcessWSAEvent: %d: FD_CLOSE.", (int) msg->wParam);
            close(msg);
            break;
    }

    return true;
}

bool TCPConnection::accept(MSG * msg) {
    int flags = FD_READ | FD_CLOSE;
    std::ostringstream output;
    SOCKET clientSocket;
    SOCKADDR_IN client;
    int client_length = sizeof(SOCKADDR_IN);


    if ((clientSocket = ::accept(msg->wParam, (SOCKADDR *) &client,
                               &client_length)) == INVALID_SOCKET) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            qDebug("TCPConnection:accept(); Error: %d", WSAGetLastError());
            return false;
        }
    }

    qDebug("TCPConnection::accept(): Communicating with client on socket: %d.",
           (int) clientSocket);

    output << "Remote address (" << inet_ntoa(client.sin_addr)
           << ") connected. (" << (int) clientSocket << ")";
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

/**
 * TODO: It appears this isn't getting called by the client.
 */
bool TCPConnection::close(MSG * msg) {
    int err = 0;

    std::ostringstream output;
    output << "\tSocket: " << (int) msg->wParam << " disconnected.";
    emit status(output.str().c_str());

    if ((err = WSAAsyncSelect(msg->wParam, getWindowHandle(), WM_WSASYNC,
                              0)) == SOCKET_ERROR) {
        qDebug("TCPConnection::close(): Error setting up async select.");
        return false;
    }

    closesocket(msg->wParam);
    emit signalCloseConnection();

    return true;
}
