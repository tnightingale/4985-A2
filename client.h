#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
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

    bool openTCPConnection();
    void start();

signals:

public slots:

};

#endif // CLIENT_H
