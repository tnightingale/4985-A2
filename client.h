#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QMainWindow>
#include <winsock2.h>
#include <windowsx.h>

class MainWindow;
class Connection;
class Client : public QObject
{
    Q_OBJECT
private:
    MainWindow* mainWindow_;
    Connection* connection_;

public:
    explicit Client(MainWindow* mainWindow);

    //bool openTCPConnection();
    //void start();
    void writeTCP(char * hostName, int port, char * data, size_t data_len);

signals:

public slots:

};

#endif // CLIENT_H
