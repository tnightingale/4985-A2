#include "mainwindow.h"
#include "ui_mainwindow.h"

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
        TCPConnection* connection = new TCPConnection(this->winId());
        connect(connection, SIGNAL(display(QString)), ui->server_log_output,
                SLOT(append(QString)));
        connect(this, SIGNAL(signalWMWSASyncRx(MSG*)), connection,
                SLOT(slotProcessWSAEvent(MSG*)));

        connection->startServer();
    }

    else if (ui->tab_client->isVisible()) {
        qDebug("MainWindow::start(): tab_client = %d", ui->tab_client->isVisible());
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
