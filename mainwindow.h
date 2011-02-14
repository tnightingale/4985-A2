#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windowsx.h>
#include <winsock2.h>

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

signals:
    void signalWMWSASyncRx(PMSG pMsg);

public slots:
    void start();
};

#endif // MAINWINDOW_H
