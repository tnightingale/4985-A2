#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>
#include <QDebug>
#include "socket.h"

class UDPSocket : public Socket
{
    Q_OBJECT
private:
    SOCKADDR_IN serverSockAddrIn_;

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

    /**
     *
     * @author Tom Nightingale.
     */
    void setDest(PHOSTENT host, size_t port) {
        memcpy((char*) &serverSockAddrIn_.sin_addr, host->h_addr, host->h_length);
        serverSockAddrIn_.sin_family = AF_INET;
        serverSockAddrIn_.sin_port = htons(port);
    }

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

        QDataStream * fileOutput = data->socket->getDataStream();
        fileOutput->writeRawData(data->winsockBuff.buf, data->winsockBuff.len);

        free(data);
        free(overlapped);
    }

    /**
     *
     * @param error
     *
     * @author Tom Nightingale
     */
    static void CALLBACK sendWorkerRoutine(DWORD error, DWORD bytesTransferred,
                                           LPWSAOVERLAPPED overlapped,
                                           DWORD inFlags) {

        //qDebug("UDPSocket::sendWorkerRoutine() Bytes sent: %d",
        //       bytesTransferred);

        if (error != 0) {
          qDebug("I/O operation failed with error %d\n", (int) error);
          return;
        }

        char* buff = (char*) overlapped->hEvent;
        free(buff);
        free(overlapped);
    }
};

#endif // UDPSOCKET_H
