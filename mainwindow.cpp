#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpconnection.h"

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
        TCPConnection* connection = new TCPConnection();
        connect(connection, SIGNAL(display(QString)), ui->server_log_output,
                SLOT(append(QString)));
        connection->startServer();
        connection->listen();
    }

    else if (ui->tab_client->isVisible()) {
        qDebug("MainWindow::start(): tab_client = %d", ui->tab_client->isVisible());
    }
}
