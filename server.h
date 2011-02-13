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
    Connection* connection_;
public:
    explicit Server(QObject *parent = 0);

    bool openTCPConnection(MainWindow* mainWindow);
    void start();

signals:

public slots:

};

#endif // SERVER_H
