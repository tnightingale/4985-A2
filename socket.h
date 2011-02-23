#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <windowsx.h>
#include <winsock2.h>
#include <QTextStream>
#include <QDataStream>
#include <QFile>
#include <QDebug>

#define MSGSIZE 1024
#define WM_WSASYNC (WM_USER + 1)
#define DATABUFSIZE 2097152
#define MAXUDPDGRAMSIZE 65507
#define PACKETSIZE 4096

class Socket;

typedef struct _DATA_ {
    WSABUF winsockBuff;
    Socket* socket;
    SOCKET clientSD;
} DATA, *PDATA;

typedef struct _STATS_ {
    int totalBytes;
    int totalPackets;
    DWORD startTime;
    DWORD finishTime;
} STATS, *PSTATS;

class Socket : public QObject
{
    Q_OBJECT
protected:
    /**
     *
     */
    SOCKET socket_;

    /**
     *
     */
    QDataStream * data_;

    /**
     *
     */
    size_t packetSize_;

    /**
     *
     */
    STATS stats_;

    /**
     *
     */
    HWND hWnd_;

public:
    virtual ~Socket() {
        closesocket(socket_);

        if (data_ != NULL) {
            delete data_;
        }
    }

    /**
     *
     * @author Tom Nightingale.
     */
    SOCKET getSocket() { return socket_; }

    /**
     *
     * @param dataSource
     *
     * @author Tom Nightingale.
     */
    void setDataStream(char * data) {
        QByteArray * qba = new QByteArray(data);
        data_ = new QDataStream(qba, QIODevice::ReadOnly);
    }

    /**
     *
     * @author Tom Nightingale.
     */
    void setDataStream(QFile * file) {
        data_ = new QDataStream(file);
    }

    /**
     *
     * @author Tom Nightingale.
     */
    QDataStream * getDataStream() {
        return data_;
    }

    /**
     *
     * @param packetSize
     *
     * @author Tom Nightingale
     */
    void setPacketSize(size_t packetSize) {
        packetSize_ = packetSize;
    }

    /**
     *
     * @author Tom Nightingale.
     */
    size_t getPacketSize() { return packetSize_; }

    /**
     *
     * @param addressFamily
     * @param connectionType
     * @param protocol
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    bool open(int addressFamily, int connectionType, int protocol);

    /**
     *
     * @param pSockAddr
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    virtual bool listen(PSOCKADDR_IN pSockAddr);

    /**
     *
     * @param pMsg
     *
     * @author Tom Nightingale.
     */
    void close(PMSG pMsg);

    void outputStatus(QString& output) {
        qDebug() << output;
        emit status(output);
        output.clear();
    }

    /**
     *
     * @author Tom Nightingale.
     */
    void initStats() {
        stats_.startTime = 0;
        stats_.finishTime = 0;
        stats_.totalBytes = 0;
        stats_.totalPackets = 0;
        emit signalStatsChanged(stats_);
    }

    STATS getStats() { return stats_; }

    /**
     *
     * @param socket
     * @param hWnd
     * @param flags
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    static bool init(SOCKET socket, HWND hWnd, int flags) {
        int err = 0;

        if ((err = WSAAsyncSelect(socket, hWnd, WM_WSASYNC,
                                  flags)) == SOCKET_ERROR) {
            throw "Connection::startServer(): Error setting up async select.";
            return false;
        }

        return true;
    }

signals:
    void signalCloseSocket(SOCKET socket);
    void status(QString);
    void signalStatsChanged(STATS);

public slots:
    /**
     *
     * @param pMsg
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    virtual bool slotProcessWSAEvent(PMSG pMsg);

};

#endif // SOCKET_H
