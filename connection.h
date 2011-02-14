#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <winsock2.h>
#include <windowsx.h>
#include <sstream>

#define MSGSIZE 1024
#define WM_WSASYNC (WM_USER + 1)

#define DATA_BUFSIZE 8192

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[DATA_BUFSIZE];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;


class Connection : public QObject
{
    Q_OBJECT
private:
    SOCKET socket_;
    HWND hWnd_;
    int addressFamily_;
    char * data_;
    size_t data_len_;

public:
    bool openSocket(int addressFamily, int connectionType,
                    int protocol, int flags);

    SOCKET* getSocket() {
        return &socket_;
    }

    virtual bool startServer(int port = 7000);
    virtual bool startClient(char* hostName, int port = 7000);
    bool sendMessage(char* data, int data_length);

    void setData(char * data, size_t data_len) {
        data_ = data;
        data_len_ = data_len;
    }

    int getData(char * data) {
        data = data_;
        return data_len_;
    }

    void setWindowHandle(HWND hWnd) {
        hWnd_ = hWnd;
    }

    HWND getWindowHandle() {
        return hWnd_;
    }

    static void CALLBACK sendCompletionRoutine(DWORD error, DWORD bytesTransferred,
                                       LPWSAOVERLAPPED overlapped,
                                       DWORD inFlags) {
        qDebug("Connection::sendCompletionRoutine");
    }

signals:
    void status(QString);
    void signalCloseConnection();

public slots:

};

#endif // CONNECTION_H