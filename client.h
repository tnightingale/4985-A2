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
    Connection* connection_;

public:
    explicit Client(QObject *parent = 0);

    bool openTCPConnection(MainWindow* mainWindow);
    void start();

signals:

public slots:

};

#endif // CLIENT_H
