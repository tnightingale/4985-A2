#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#define TCP 0
#define UDP 1
#define MSGSIZE 1024

#include <QObject>
#include <QString>
#include <winsock.h>

class TCPConnection : public QObject
{
    Q_OBJECT
private:
    WORD wVersionRequested_;
    WSADATA wsaData_;
    SOCKADDR_IN serverSockAddrIn_;
    int socket_;

public:
    explicit TCPConnection();
    ~TCPConnection();
    bool startServer(int port = 7000);
    bool startClient(char* hostName, int port);
    bool listen();
    bool sendMessage(char* data, int data_length);

signals:
    void display(QString);

public slots:

};

#endif // TCPCONNECTION_H
