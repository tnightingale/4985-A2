#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QFile>
#include <QMainWindow>
#include <winsock2.h>
#include <windowsx.h>

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

public:
    Client(MainWindow* mainWindow, _SETTINGS_ * settings);

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

signals:

public slots:

};

#endif // CLIENT_H
