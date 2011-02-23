#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMainWindow>
#include <QFile>
#include <winsock2.h>
#include <windowsx.h>
#include "socket.h"

class MainWindow;
class Socket;
class Server : public QObject
{
    Q_OBJECT
private:
    MainWindow* mainWindow_;
    QFile * file_;
    Socket * socket_;

    /**
     *
     */
    STATS stats_;

public:
    explicit Server(MainWindow* mainWindow);
    virtual ~Server();

    bool listenTCP(int port = 7000);
    bool listenUDP(int port = 7000);

    /**
     *
     * @author Tom Nightingale.
     */
    STATS getStats() { return stats_; }

    /**
     *
     * @author Tom Nightingale.
     */
    void initGui();

signals:

public slots:
    void slotStatsSetBytes(int);
    void slotStatsSetPackets(int);
    void slotStatsSetStartTime(int);
    void slotStatsSetFinishTime(int);
    void slotUpdateStats();

};

#endif // SERVER_H
