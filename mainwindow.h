#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windowsx.h>
#include <winsock2.h>
#include <cstdlib>
#include <QIntValidator>

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
} SETTINGS;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool winEvent(MSG * msg, long * result);

    Ui::MainWindow* getUi() {
        return ui;
    }

private:
    Ui::MainWindow *ui;
    SETTINGS settings_;

signals:
    void signalWMWSASyncRx(PMSG pMsg);

public slots:
    void start();
    void slotUpdateSettings(void);

};

#endif // MAINWINDOW_H
