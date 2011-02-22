#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "client.h"
#include "socket.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initGui();

    settings_.mode = CLIENT;
    settings_.protocol = TCP;
    settings_.port = 7000;

    slotUpdateSettings();

    settings_.packet_count = 100;
    settings_.packet_size = 4096;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initGui() {
    // Port
    connect(ui->port_val, SIGNAL(textChanged(QString)),
            this, SLOT(slotUpdateSettings(void)));
    // Tabs
    connect(ui->mode_tabs, SIGNAL(currentChanged(int)),
            this, SLOT(slotUpdateSettings(void)));
    // Protocol radios
    connect(ui->protocol_tcp_radio, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateSettings(void)));
    connect(ui->protocol_udp_radio, SIGNAL(toggled(bool)),
            this, SLOT(slotUpdateSettings(void)));
    // Num packets
    connect(ui->packet_num, SIGNAL(valueChanged(int)),
            this, SLOT(slotUpdateSettings()));
    connect(ui->packet_num_val, SIGNAL(textChanged(QString)),
            this, SLOT(slotUpdateSettings(void)));
    // Packet size
    connect(ui->packet_size, SIGNAL(valueChanged(int)),
            this, SLOT(slotUpdateSettings()));
    connect(ui->packet_size_val, SIGNAL(textChanged(QString)),
            this, SLOT(slotUpdateSettings(void)));
    // File browse buttons
    connect(ui->file_browse, SIGNAL(clicked()),
            this, SLOT(slotBrowseFileSrc()));
    connect(ui->file_dest_browse, SIGNAL(clicked()),
            this, SLOT(slotBrowseFileDst()));
    // File path
    connect(ui->file_val, SIGNAL(textChanged(QString)),
            this, SLOT(slotUpdateSettings()));
    connect(ui->file_dest_val, SIGNAL(textChanged(QString)),
            this, SLOT(slotUpdateSettings()));
    // Destination address
    connect(ui->dest_address, SIGNAL(textChanged(QString)),
            this, SLOT(slotUpdateSettings(void)));
    // Start button
    connect(ui->start, SIGNAL(clicked()),
            this, SLOT(start()));

    QValidator * portValidator = new QIntValidator(7000, 65535, this);
    ui->port_val->setValidator(portValidator);

    //QRegExp rx("(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    //QRegExpValidator * addrValidator = new QRegExpValidator(rx, 0);
    //ui->dest_address->setValidator(addrValidator);
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
            client = new Client(this, &settings_);

            switch (settings_.protocol) {
                case TCP:
                    if (ui->file_val->text() == NULL) {
                        qDebug("MainWindow::start(): TCP Client starting");
                        client->sendTCP(settings_.address,
                                        settings_.port,
                                        settings_.packet_size,
                                        settings_.packet_count);
                    } else {
                        qDebug("MainWindow::start(): TCP Client starting -- file");
                        client->sendFileTCP(settings_.address,
                                            settings_.port,
                                            settings_.packet_size,
                                            settings_.srcFilePath);
                    }
                    break;

                case UDP:
                    if (ui->file_val->text() == NULL) {
                        qDebug("MainWindow::start(): UDP client starting.");
                        client->sendUDP(settings_.address,
                                        settings_.port,
                                        settings_.packet_size,
                                        settings_.packet_count);
                    } else {
                        qDebug("MainWindow::start(): UDP Client starting -- file");
                        client->sendFileUDP(settings_.address,
                                            settings_.port,
                                            settings_.packet_size,
                                            settings_.srcFilePath);
                    }
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

void MainWindow::slotBrowseFileSrc() {
    settings_.srcFilePath = QFileDialog::getOpenFileName(this,
        tr("Select file"), ".", tr("Any File(*.*)"));
    ui->file_val->setText(settings_.srcFilePath);
}

void MainWindow::slotBrowseFileDst() {
    settings_.srcFilePath = QFileDialog::getSaveFileName(this,
        tr("Select file"), ".", tr("Any File(*.*)"));
    ui->file_dest_val->setText(settings_.srcFilePath);
}

void MainWindow::slotUpdateSettings(void) {
    bool enabled = true;
    QHostAddress address;

    settings_.mode = (ui->mode_tabs->currentIndex() == 0) ? CLIENT : SERVER;

    switch (settings_.mode) {
        case CLIENT:
            if (!address.setAddress(ui->dest_address->text())) {
                enabled = false;
            }

            ui->packet_num_val->setText(QString().setNum(ui->packet_num->value()));
            settings_.packet_count = ui->packet_num_val->text().toInt();
            ui->packet_size_val->setText(QString().setNum(ui->packet_size->value()));
            settings_.packet_size = ui->packet_size_val->text().toInt();

            break;

        case SERVER:
            if (ui->file_dest_val->text() == NULL) {
                enabled = false;
            }
            break;

        default:
            return;
    }

    settings_.port = ui->port_val->text().toInt();
    if (!ui->port_val->hasAcceptableInput()) {
        enabled = false;
    }

    settings_.protocol = (ui->protocol_tcp_radio->isChecked()) ? TCP : UDP;
    settings_.srcFilePath = ui->file_val->text();
    settings_.dstFilePath = ui->file_dest_val->text();

    if (!enabled) {
        ui->start->setEnabled(false);
        return;
    } else {
        ui->start->setEnabled(true);
    }

    settings_.address = ui->dest_address->text();
}

void MainWindow::log(QString output) {
    ui->server_log_output->append(output);
}
