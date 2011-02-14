#include "socket.h"

bool Socket::open(int addressFamily, int connectionType, int protocol) {
    qDebug("Socket::openSocket(): Opening a new socket.");
    if ((socket_ = WSASocket(addressFamily, connectionType, protocol, NULL, 0,
                             WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
        qDebug("Socket::openSocket(): Can't create socket. Error: %d",
               WSAGetLastError());
        return false;
    }
    qDebug("Socket::openSocket(): SocketCreated: %d", (int) socket_);

    return true;
}

bool Socket::listen(PSOCKADDR_IN pSockAddr) {
    int err = 0;

    if ((err = bind(socket_, (PSOCKADDR) pSockAddr, sizeof(*pSockAddr))
        == SOCKET_ERROR)) {
        qDebug("Connection::startServer(): Can't bind to socket. Error: %d",
               WSAGetLastError());
        return false;
    }

    // TODO: Might want to call linger here.

    return true;
}

void Socket::close(PMSG pMsg) {
    emit signalCloseSocket(pMsg->wParam);
    closesocket(pMsg->wParam);
}



bool Socket::slotProcessWSAEvent(PMSG pMsg) {
    if (WSAGETSELECTERROR(pMsg->lParam)) {
        qDebug("Socket::slotProcessWSAEvent(): %d: Socket failed with error %d",
              (int) pMsg->wParam, WSAGETSELECTERROR(pMsg->lParam));
        return false;
    }

    switch (WSAGETSELECTEVENT(pMsg->lParam)) {

        case FD_CLOSE:
            qDebug("Socket::slotProcessWSAEvent: %d: FD_CLOSE.", (int) pMsg->wParam);
            close(pMsg);
            break;
    }

    return true;
}
