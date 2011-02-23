#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QFile>
#include <QMainWindow>
#include <winsock2.h>
#include <windowsx.h>
#include "socket.h"

struct _SETTINGS_;
class MainWindow;
class Socket;
class Client : public QObject
{
    Q_OBJECT
private:
    MainWindow* mainWindow_;
    Socket* socket_;
    _SETTINGS_ * settings_;

    /**
     *
     */
    STATS stats_;

public:
    Client(MainWindow* mainWindow, _SETTINGS_ * settings);
    ~Client();

    /**
     * @param address
     * @param port
     *
     * @author Tom Nightingale.
     */
    void sendTCP(QString address, int port, size_t packetSize,
                     size_t numPackets);
    void sendFileTCP(QString address, int port, size_t packetSize,
                     QString filepath);

    /**
     *
     * @param hostName
     * @param port
     *
     * @author Tom Nightingale.
     */
    void writeTCP(char * hostName, int port);

    /**
     *
     */
    void sendUDP(QString address, int port, size_t packetSize,
                 size_t numPackets);
    void sendFileUDP(QString address, int port, size_t packetSize,
                     QString filepath);

    /**
     *
     */
    void writeUDP(char* hostName, int port);

    /**
     *
     * @author Tom Nightingale.
     */
    void send();

    /**
     *
     * @author Tom Nightingale.
     */
    void initGui();

    /**
     *
     * @author Tom Nightingale.
     */
    STATS getStats() { return stats_; }

signals:

public slots:
    void slotStatsSetBytes(int);
    void slotStatsSetPackets(int);
    void slotStatsSetStartTime(int);
    void slotStatsSetFinishTime(int);
    void slotUpdateStats();

};

#endif // CLIENT_H
