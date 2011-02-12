#include "tcpconnection.h"

TCPConnection::TCPConnection()
{
    int err = 0;
    wVersionRequested_ = MAKEWORD(2,2);
    if ((err = WSAStartup(wVersionRequested_, &wsaData_)) < 0) {
        throw "TCPConnection::TCPConnection(): Missing WINSOCK2 DLL.";
    }

    if ((socket_ = socket(AF_INET, SOCK_STREAM, TCP)) < 0) {
        throw "TCPConnection::TCPConnection(): Can't create socket.";
    }
}

TCPConnection::~TCPConnection() {
    closesocket(socket_);
    WSACleanup();
}

bool TCPConnection::startServer(int port) {
    int err = 0;
    serverSockAddrIn_.sin_family = AF_INET;
    serverSockAddrIn_.sin_port = htons(port);
    serverSockAddrIn_.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((err = bind(socket_, (PSOCKADDR) &serverSockAddrIn_,
              sizeof(serverSockAddrIn_)) < 0)) {
        throw "TCPConnection::startServer(): Can't bind name to socket.";
    }

    qDebug("TCPConnection::startServer(): Server bound port %d to TCP socket",
           port);

    return true;
}

bool TCPConnection::startClient(char* hostName, int port) {
    PHOSTENT host;

    serverSockAddrIn_.sin_family = AF_INET;
    serverSockAddrIn_.sin_port = htons(port);

    if ((host = gethostbyname(hostName)) == NULL) {
        qDebug("TCPConnection()::sertClient(): Unknown server address");

        return false;
    }

    memcpy((char*) &serverSockAddrIn_.sin_addr, host->h_addr, host->h_length);

    if ((::connect(socket_, (PSOCKADDR) &serverSockAddrIn_,
                   sizeof(SOCKADDR_IN))) < 0) {

        return false;
    }

    return true;
}

bool TCPConnection::listen() {
    int err = 0;
    SOCKADDR_IN client;
    int client_length = sizeof(SOCKADDR_IN);
    int clientSocket;
    char msg[MSGSIZE];

    if ((err = ::listen(socket_, 5)) == SOCKET_ERROR) {
        err = WSAGetLastError();
        qDebug("TCPConnection::listen(); Error: %d.", err);
    }

    qDebug("TCPConnection::listen(): Server listening for TCP connections...");

    while (true) {
        qDebug("hello");
        if ((clientSocket = accept(socket_, (PSOCKADDR) &client,
                                   &client_length)) < 0) {
            throw "TCPConnection:listen(); Error:";
        }
        emit display(QString("TCPConnection:listen(): Client (") +
                     inet_ntoa(client.sin_addr) + QString(") connected\n"));

        if (!::recv(clientSocket, msg, MSGSIZE, 0)) {
            qDebug("TCPConnection:listen(); Error: Problem receiving message");
        }
        qDebug(msg);
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
