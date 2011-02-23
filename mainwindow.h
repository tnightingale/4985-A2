#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <windowsx.h>
#include <winsock2.h>
#include <cstdlib>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QRegExp>
#include <QHostAddress>
#include <QTimer>
#include "socket.h"

enum IPTEST_MODE {
    CLIENT,
    SERVER
};

enum PROTOCOL {
    TCP,
    UDP
};

typedef struct _SETTINGS_ {
    IPTEST_MODE mode;
    PROTOCOL protocol;
    int port;
    QString address;
    QString srcFilePath;
    QString dstFilePath;
    size_t packet_size;
    size_t packet_count;
} SETTINGS;

namespace Ui {
    class MainWindow;
}


class Client;
class Server;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initGui();
    bool winEvent(MSG * msg, long * result);

    Ui::MainWindow* getUi() {
        return ui;
    }

    QTimer * getTimer() { return statUpdater_; }

    void log(QString output);

private:
    Ui::MainWindow *ui;
    SETTINGS settings_;
    QTimer * statUpdater_;
    Client * client_;
    Server * server_;

signals:
    void signalWMWSASyncRx(PMSG pMsg);

public slots:
    void start();
    void stop();
    void slotBrowseFileSrc();
    void slotBrowseFileDst();
    void slotUpdateSettings(void);
    void slotUpdateClientStats(STATS);
    void slotUpdateServerStats(STATS);

};

#endif // MAINWINDOW_H
