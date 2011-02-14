#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMainWindow>
#include <winsock2.h>
#include <windowsx.h>

class MainWindow;
class Connection;
class Server : public QObject
{
    Q_OBJECT
private:
    MainWindow* mainWindow_;
    Connection* connection_;
public:
    explicit Server(MainWindow* mainWindow);

    bool openTCPConnection();
    void start();

signals:

public slots:

};

#endif // SERVER_H
