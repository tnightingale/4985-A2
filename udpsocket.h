#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>
#include <QDebug>
#include "socket.h"

class UDPSocket : public Socket
{
    Q_OBJECT
public:
    UDPSocket(HWND hWnd);
    ~UDPSocket() {}

    /**
     *
     * @param pMsg
     *
     * @author Tom Nightingale
     */
    void receive(PMSG pMsg);

    /**
     *
     * @param pMsg
     *
     * @author Tom Nightingale
     */
    void send(PMSG pMsg);

public slots:
    /**
     *
     * @param pMsg
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    bool slotProcessWSAEvent(PMSG pMsg);

public:
    /**
     *
     * @param error
     *
     * @author Tom Nightingale
     */
    static void CALLBACK recvWorkerRoutine(DWORD error, DWORD bytesTransferred,
                                           LPWSAOVERLAPPED overlapped,
                                           DWORD inFlags) {
        static int count = 0;
        static int totalRecv = 0;

        QString output;
        QTextStream log(&output, QIODevice::WriteOnly);
        PDATA data;

        if (error != 0) {
          qDebug("I/O operation failed with error %d\n", (int) error);
          return;
        }

        count++;
        totalRecv += bytesTransferred;

        data = (PDATA) overlapped->hEvent;

        log << "\t DGRAM: " << count << ") "
            << "Received: " << bytesTransferred << ", "
            << "Total: " << totalRecv;
        data->socket->outputStatus(output);

        qDebug("STATIC UDPSocket::recvWorkerRoutine()");
        qDebug("\tPacket no: %d", count);
        qDebug("\tTotal received (bytes): %d", totalRecv);
        qDebug() << "\tBytes received: " << bytesTransferred;

        //data->winsockBuff.buf[bytesTransferred] = '\0';
        //qDebug() << "\tData:\n" << data->winsockBuff.buf;

        free(data);
        free(overlapped);
    }
};

#endif // UDPSOCKET_H
