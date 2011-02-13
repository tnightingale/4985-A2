#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <windowsx.h>
#include <winsock2.h>
#include <sstream>

#include "connection.h"

class MainWindow;
class TCPConnection : public Connection
{
    Q_OBJECT
private:
    WORD wVersionRequested_;
    WSADATA wsaData_;
    SOCKET socket_;

public:
    explicit TCPConnection(MainWindow* mainWindow);
    ~TCPConnection();

    bool accept(MSG * msg);
    bool read(MSG * msg);
    bool close(MSG * msg);


    static void CALLBACK WorkerRoutine(DWORD error, DWORD bytesTransferred,
                                       LPWSAOVERLAPPED overlapped,
                                       DWORD inFlags) {

        qDebug("STATIC TCPConnection::WorkerRoutine()");

        // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
        //LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) overlapped;

        if (error != 0) {
          qDebug("I/O operation failed with error %d\n", (int) error);
        }

        if (bytesTransferred == 0) {
           //qDebug("Closing socket %d\n", SI->Socket);
        }

        if (error != 0 || bytesTransferred == 0) {
           //closesocket(SI->Socket);
           //GlobalFree(SI);
           return;
        }

        char* buff = (char*) overlapped->hEvent;
        buff[bytesTransferred] = '\0';

        std::ostringstream output;
        qDebug() << "READ: " << buff;
        free(buff);
        free(overlapped);
    }


public slots:
    bool slotProcessWSAEvent(MSG * msg);
};

#endif // TCPCONNECTION_H
