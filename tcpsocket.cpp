#include "tcpsocket.h"

TCPSocket::TCPSocket(HWND hWnd) {
    int err = 0;
    WSADATA wsaData;

    WORD wVersionRequested_ = MAKEWORD(2,2);
    if ((err = WSAStartup(wVersionRequested_, &wsaData)) < 0) {
        throw "TCPConnection::TCPConnection(): Missing WINSOCK2 DLL.";
    }

    hWnd_ = hWnd;
    open(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void TCPSocket::accept(PMSG pMsg) {
    QString output;
    QTextStream log(&output, QIODevice::WriteOnly);
    SOCKET clientSocket;
    SOCKADDR_IN client;
    int client_length = sizeof(SOCKADDR_IN);

    if ((clientSocket = ::accept(pMsg->wParam, (PSOCKADDR) &client,
                                 &client_length)) == INVALID_SOCKET) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            qDebug("TCPConnection:accept(); Error: %d", WSAGetLastError());
            return;
        }
    }

    log << "Remote address (" << inet_ntoa(client.sin_addr)
        << ") connected. (" << (int) clientSocket << ")";
    outputStatus(output);

    Socket::init(clientSocket, hWnd_, FD_READ | FD_CLOSE);
}

void TCPSocket::send(PMSG pMsg) {
    int err = 0;
    int result = 0;
    int num = 0;
    size_t bytesRead = 0;
    size_t totalSent = 0;

    DWORD numBytesSent = 0;
    WSAOVERLAPPED* ol;
    WSABUF winsockBuff;

    winsockBuff.len = getPacketSize();
    bytesRead = winsockBuff.len;


    while (data_->status() == QDataStream::Ok) {
        // These are free'd within TCPSocket::sendWorkerRoutine.
        winsockBuff.buf = (char *) malloc(PACKETSIZE * sizeof(char));
        ol = (WSAOVERLAPPED*) calloc(1, sizeof(WSAOVERLAPPED));

        if ((num = data_->readRawData(winsockBuff.buf, bytesRead)) <= 0) {
            qDebug("ABOUT TO TERM: %d", num);
            break;
        }
        totalSent += num;
        qDebug("Total sent: %d", totalSent);
        ol->hEvent = (HANDLE) winsockBuff.buf;

        result = WSASend(pMsg->wParam, &winsockBuff, 1, &numBytesSent, 0, ol,
                          TCPSocket::sendWorkerRoutine);
        if ((err = WSAGetLastError()) > 0 && err != ERROR_IO_PENDING) {
            qDebug("TCPSocket::send(); Error: %d", err);
            return;
        }

        if (result == WSAEWOULDBLOCK) {
            return;
        }

    }
    qDebug("TCPSocket::send(): Total bytes sent: %d", totalSent);
    if (data_->status() == QDataStream::Ok) {
        shutdown();
    }
}

void TCPSocket::receive(PMSG pMsg) {
    int err = 0;
    DWORD flags = 0;
    DWORD recvBytes;
    WSAOVERLAPPED* ol;
    WSABUF winsockBuff;

    PDATA data = (PDATA) calloc(1, sizeof(DATA));
    data->socket = this;
    data->winsockBuff.len = MAXUDPDGRAMSIZE;
    data->winsockBuff.buf = (char*) calloc(data->winsockBuff.len, sizeof(char));
    data->clientSD = pMsg->wParam;

    ol = (WSAOVERLAPPED*) calloc(1, sizeof(WSAOVERLAPPED));
    ol->hEvent = (HANDLE) data;

    if (WSARecv(pMsg->wParam, &(data->winsockBuff), 1, &recvBytes, &flags,
                ol, TCPSocket::recvWorkerRoutine) == SOCKET_ERROR) {
        if ((err = WSAGetLastError()) != WSA_IO_PENDING) {
            qDebug("TCPSocket::receive(): WSARecv() failed with error %d",
                   err);
            return;
        }
    }

}


bool TCPSocket::listen(PSOCKADDR_IN pSockAddr) {
    int err = 0;

    if (!Socket::listen(pSockAddr)) {
        return false;
    }

    if ((::listen(socket_, 5)) == SOCKET_ERROR) {
        err = WSAGetLastError();
        qDebug("Connection::listen(); Error: %d.", err);
        return false;
    }

    return true;
}

bool TCPSocket::connectRemote(PSOCKADDR_IN pSockAddr) {
    int err = 0;

    if ((err = ::connect(socket_, (PSOCKADDR) pSockAddr,
                   sizeof(SOCKADDR_IN))) == SOCKET_ERROR) {
        if ((err = WSAGetLastError()) != WSAEWOULDBLOCK) {
            qDebug("TCPSocket::connectRemote(): Connect failed. Error: %d",
                   WSAGetLastError());
            return false;
        }
    }

    return true;
}

void TCPSocket::shutdown() {
    qDebug("TCPSocket::shutdown()");
    ::shutdown(socket_, SD_BOTH);
}

bool TCPSocket::slotProcessWSAEvent(PMSG pMsg) {
    if (WSAGETSELECTERROR(pMsg->lParam)) {
        qDebug("TCPSocket::slotProcessWSAEvent(): %d: Socket failed. Error: %d",
              (int) pMsg->wParam, WSAGETSELECTERROR(pMsg->lParam));
        return false;
    }

    // TODO: If want to receive TCP & UDP simultaneously, need to be able to
    //		 filter messages against a list of open TCP client sockets here.
    //		 Currently that list doesn't exist.
    //if (pMsg->wParam != socket_) {
    //    return false;
    //}

    switch (WSAGETSELECTEVENT(pMsg->lParam)) {
        case FD_ACCEPT:
            qDebug("TCPSocket::slotProcessWSAEvent: %d: FD_ACCEPT.",
                   (int) pMsg->wParam);
            accept(pMsg);
            break;

        case FD_READ:
            qDebug("TCPSocket::slotProcessWSAEvent: %d: FD_READ.",
                   (int) pMsg->wParam);
            receive(pMsg);
            break;

        case FD_WRITE:
            qDebug("TCPSocket::slotProcessWSAEvent: %d: FD_WRITE.",
                   (int) pMsg->wParam);
            send(pMsg);
            break;

        case FD_CONNECT:
            qDebug("TCPSocket::slotProcessWSAEvent: %d: FD_CONNECT.",
                   (int) pMsg->wParam);
            break;

        default:
            Socket::slotProcessWSAEvent(pMsg);
            break;
    }

    return true;
}
