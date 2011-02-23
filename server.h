#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMainWindow>
#include <QFile>
#include <winsock2.h>
#include <windowsx.h>

class MainWindow;
class Socket;
class Server : public QObject
{
    Q_OBJECT
private:
    MainWindow* mainWindow_;
    QFile * file_;
    Socket * socket_;
public:
    explicit Server(MainWindow* mainWindow);
    virtual ~Server();

    bool listenTCP(int port = 7000);
    bool listenUDP(int port = 7000);

signals:

public slots:
    void slotUpdateStats();

};

#endif // SERVER_H
