#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "client.h"
#include "connection.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->start, SIGNAL(clicked()), this, SLOT(start()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start() {
    if (ui->tab_server->isVisible()) {
        qDebug("MainWindow::start(): Starting tcp server...");

        Server* server = new Server(this);
        server->openTCPConnection();
        server->start(); // TODO: Send requested port.
    }

    else if (ui->tab_client->isVisible()) {
        qDebug("MainWindow::start(): Client starting");

        Client* client = new Client(this);
        client->openTCPConnection();
        client->start();
    }
}

bool MainWindow::winEvent(MSG * msg, long * result) {
    switch (msg->message) {
        case WM_WSASYNC:
            emit signalWMWSASyncRx(msg);
            return true;
    }

    return false;
}
