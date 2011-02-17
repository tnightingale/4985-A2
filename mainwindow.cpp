#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "client.h"
#include "connection.h"
#include "socket.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings_.mode = CLIENT;
    settings_.protocol = TCP;
    settings_.port = 7000;

    connect(ui->port_val, SIGNAL(textChanged(QString)),
            this, SLOT(slotUpdateSettings(void)));
    connect(ui->mode_tabs, SIGNAL(currentChanged(int)),
            this, SLOT(slotUpdateSettings(void)));
    connect(ui->protocol_tcp_radio, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateSettings(void)));
    connect(ui->protocol_udp_radio, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateSettings(void)));

    QValidator *validator = new QIntValidator(7000, 9000, this);
    ui->port_val->setValidator(validator);

    connect(ui->start, SIGNAL(clicked()), this, SLOT(start()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start() {
    Server* server;
    Client* client;
    char* address;
    char* data;

    switch (settings_.mode) {
        case SERVER:
            server = new Server(this);
            switch (settings_.protocol) {
                case TCP:
                    qDebug("MainWindow::start(): Starting tcp server...");
                    server->listenTCP(settings_.port);
                    break;

                case UDP:
                    qDebug("MainWindow::start(): Starting udp server...");
                    server->listenUDP(settings_.port);
                    break;

                default:
                    qDebug("MainWindow::start(): ERROR");
                    break;
            }
            break;

        case CLIENT:
            client = new Client(this);
            switch (settings_.protocol) {
                case TCP:
                    qDebug("MainWindow::start(): Client starting");
                    client->sendTCP();
                    break;

                case UDP:
                    qDebug("MainWindow::start(): UDP client starting.");
                    break;

                default:
                    qDebug("MainWindow::start(): ERROR");
                    break;
            }
            break;

        default:
            qDebug("MainWindow::start(): You shouldn't be able to get here");
            exit(1);
            break;
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

void MainWindow::slotUpdateSettings(void) {
    settings_.port = ui->port_val->text().toInt();
    if (ui->port_val->hasAcceptableInput()) {
        ui->start->setEnabled(TRUE);
    } else {
        ui->start->setEnabled(FALSE);
    }

    settings_.mode = (ui->mode_tabs->currentIndex() == 0) ? CLIENT : SERVER;
    settings_.protocol = (ui->protocol_tcp_radio->isChecked()) ? TCP : UDP;
}
