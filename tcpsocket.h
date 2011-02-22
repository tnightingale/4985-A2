#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QDebug>
#include "socket.h"

class TCPSocket : public Socket
{
    Q_OBJECT
public:
    TCPSocket(HWND hWnd);
    ~TCPSocket() {}

    /**
     *
     * @param pMsg
     *
     * @author Tom Nightingale
     */
    void accept(PMSG pMsg);

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
     * @param pSockAddr
     *
     * @author Tom Nightingale
     */
    bool listen(PSOCKADDR_IN pSockAddr);

    /**
     *
     * @param pSockAddr
     *
     * @author Tom Nightingale
     */
    bool connectRemote(PSOCKADDR_IN pSockAddr);

    /**
     *
     * @author Tom Nightingale
     */
    void shutdown();

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

        log << "\t" << (int) data->clientSD << ") "
            << "PKT: " << count << ") "
            << "Received: " << bytesTransferred << ", "
            << "Total: " << totalRecv;
        data->socket->outputStatus(output);

        qDebug("STATIC TCPSocket::recvWorkerRoutine()");
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

        qDebug("TCPSocket::sendWorkerRoutine() Bytes sent: %d",
               bytesTransferred);

        if (error != 0) {
          qDebug("I/O operation failed with error %d\n", (int) error);
          return;
        }

        char* buff = (char*) overlapped->hEvent;
        free(buff);
        free(overlapped);
        //buff[bytesTransferred] = '\0';

    }
};

#endif // TCPSOCKET_H
