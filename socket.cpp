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
    QString output;
    QTextStream log(&output, QIODevice::WriteOnly);
    int err = 0;

    if ((err = bind(socket_, (PSOCKADDR) pSockAddr, sizeof(SOCKADDR))
        == SOCKET_ERROR)) {
        qDebug("Connection::startServer(): Can't bind to socket. Error: %d",
               WSAGetLastError());
        return false;
    }

    qDebug("Socket::listen(): Bound socket %d to port %d.",
           (int) socket_, (int) ntohs(pSockAddr->sin_port));

    log << "Bound socket " << (int) socket_
        << " to port " << (int) ntohs(pSockAddr->sin_port) << ".";
    emit status(output);
    // TODO: Might want to call linger here.
    // allow the TCP packets to linger for up to 10 seconds
    //LINGER* linger = (LINGER*) malloc(sizeof(LINGER));
    //linger->l_onoff = 1;
    //linger->l_linger = 2;
    //setsockopt(socket_, SOL_SOCKET, SO_LINGER, (char*) linger, sizeof(linger));

    return true;
}

void Socket::close(PMSG pMsg) {
    QString output;
    QTextStream log(&output, QIODevice::WriteOnly);
    log << "Socket: " << (int) pMsg->wParam << " disconnected.";
    emit status(output);

    //data_->device()->close();

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
