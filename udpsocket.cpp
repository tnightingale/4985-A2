#include "udpsocket.h"

UDPSocket::UDPSocket(HWND hWnd) {
    int err = 0;
    WSADATA wsaData;

    WORD wVersionRequested_ = MAKEWORD(2,2);
    if ((err = WSAStartup(wVersionRequested_, &wsaData)) < 0) {
        throw "TCPConnection::TCPConnection(): Missing WINSOCK2 DLL.";
    }

    hWnd_ = hWnd;
    open(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

void UDPSocket::send(PMSG pMsg) {

}

void UDPSocket::receive(PMSG pMsg) {
    int err = 0;
    DWORD flags = 0;
    DWORD recvBytes;
    WSAOVERLAPPED* ol;

    PDATA data = (PDATA) calloc(1, sizeof(DATA));
    data->socket = this;
    data->winsockBuff.len = MAXUDPDGRAMSIZE;
    data->winsockBuff.buf = (char*) calloc(data->winsockBuff.len, sizeof(char));
    data->clientSD = pMsg->wParam;

    ol = (WSAOVERLAPPED*) calloc(1, sizeof(WSAOVERLAPPED));
    ol->hEvent = (HANDLE) data;

    if (WSARecvFrom(pMsg->wParam, &(data->winsockBuff), 1, &recvBytes, &flags,
                NULL, NULL, ol, UDPSocket::recvWorkerRoutine) == SOCKET_ERROR) {
        if ((err = WSAGetLastError()) != WSA_IO_PENDING) {
            qDebug("UDPSocket::receive(): WSARecv() failed with error %d",
                   err);
            return;
        }
    }
}

bool UDPSocket::slotProcessWSAEvent(PMSG pMsg) {
    if (WSAGETSELECTERROR(pMsg->lParam)) {
        qDebug("UDPSocket::slotProcessWSAEvent(): %d: Socket failed. Error: %d",
              (int) pMsg->wParam, WSAGETSELECTERROR(pMsg->lParam));
        return false;
    }

    // Filtering out messages for other sockets / protocols.
    if (pMsg->wParam != socket_) {
        return false;
    }

    switch (WSAGETSELECTEVENT(pMsg->lParam)) {
        case FD_READ:
            qDebug("UDPSocket::slotProcessWSAEvent: %d: FD_READ.",
                   (int) pMsg->wParam);
            receive(pMsg);
            break;

        case FD_WRITE:
            qDebug("UDPSocket::slotProcessWSAEvent: %d: FD_WRITE.",
                   (int) pMsg->wParam);
            send(pMsg);
            break;

        default:
            Socket::slotProcessWSAEvent(pMsg);
            break;
    }

    return true;
}
