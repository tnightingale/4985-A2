#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QString>
#include <windowsx.h>
#include <winsock2.h>
#include <sstream>

#define TCP 0
#define UDP 1
#define MSGSIZE 1024
#define WM_WSASYNC (WM_USER + 1)

#define DATA_BUFSIZE 8192

typedef struct _SOCKET_INFORMATION {
   BOOL RecvPosted;
   CHAR Buffer[DATA_BUFSIZE];
   WSABUF DataBuf;
   SOCKET Socket;
   DWORD BytesSEND;
   DWORD BytesRECV;
   _SOCKET_INFORMATION *Next;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

class TCPConnection : public QObject
{
    Q_OBJECT
private:
    WORD wVersionRequested_;
    WSADATA wsaData_;
    SOCKADDR_IN serverSockAddrIn_;
    SOCKET socket_;
    LPSOCKET_INFORMATION socketInfoList_;
    HWND hWnd_;

public:
    explicit TCPConnection(HWND hWnd);
    ~TCPConnection();
    bool startServer(int port = 7000);
    bool startClient(char* hostName, int port);
    bool sendMessage(char* data, int data_length);

    bool accept(MSG * msg);
    bool read(MSG * msg);
    bool close(MSG * msg);

    static void CreateSocketInformation(SOCKET_INFORMATION * socketInfoList,
                                        SOCKET s) {
        // Prepare SocketInfo structure for use.
        socketInfoList->Socket = s;
        socketInfoList->RecvPosted = FALSE;
        socketInfoList->BytesSEND = 0;
        socketInfoList->BytesRECV = 0;
        //socketInfoList->Next = socketInfoList;
    }

signals:
    void display(QString);

public slots:
    bool slotProcessWSAEvent(MSG * msg);
};

#endif // TCPCONNECTION_H
