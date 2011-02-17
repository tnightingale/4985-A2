#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QMainWindow>
#include <winsock2.h>
#include <windowsx.h>

class MainWindow;
class Socket;
class Client : public QObject
{
    Q_OBJECT
private:
    MainWindow* mainWindow_;
    Socket* socket_;

public:
    explicit Client(MainWindow* mainWindow);

    /**
     *
     * @author Tom Nightingale.
     */
    void sendTCP();

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
     * @author Tom Nightingale.
     */
    void send();

signals:

public slots:

};

#endif // CLIENT_H
