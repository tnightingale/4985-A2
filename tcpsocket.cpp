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

    Socket::init(clientSocket, hWnd_, FD_READ | FD_CLOSE);
}

void TCPSocket::send(PMSG pMsg) {

}

void TCPSocket::receive(PMSG pMsg) {

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


bool TCPSocket::slotProcessWSAEvent(PMSG pMsg) {
    if (WSAGETSELECTERROR(pMsg->lParam)) {
        qDebug("TCPSocket::slotProcessWSAEvent(): %d: Socket failed. Error: %d",
              (int) pMsg->wParam, WSAGETSELECTERROR(pMsg->lParam));
        return false;
    }

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

        default:
            Socket::slotProcessWSAEvent(pMsg);
            break;
    }

    return true;
}
