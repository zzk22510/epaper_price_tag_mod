#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "command.h"

#include "uarthandler.h"

#include <QDebug>
#include <QtGlobal>
#include <QTimer>
#include <QDateTime>
#include <QLabel>

#include "forma7105pingtest.h"
#include "forma7105uploader.h"
#include "forma7105download.h"
#include "formtest.h"
#include "dialogtexttoimage.h"

#include "formbatchdownloader.h"
#include "batchdownloader.h"
#include "batchuploader.h"
#include "formbatchuploader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    labelStatusPortName(new QLabel("")),
    labelStatusPortError(new QLabel("")),
    labelStatusPortStatistic(new QLabel("")),
    formTraffic(new FormTraffic()),
    formLed(new FormLed()),
    formA7105PingTest(new FormA7105PingTest()),
    formA7105Upploader(new FormA7105Uploader()),
    formA7105Download(new FormA7105Download()),
    formTest(new FormTest()),
    dialogPortConfig(new DialogPortConfig(this)),
    a7105PingTest(new A7105PingTest()),
    a7105Upploader(new A7105Uploader()),
    a7105Downloader(new A7105Downloader()),
    formBatchDownloader(new FormBatchDownloader(nullptr)),
    batchDownloader(new BatchDownloader()),
    formBatchUploader(new FormBatchUploader(nullptr)),
    batchUploader(new BatchUploader())
{
    ui->setupUi(this);

    ui->tabWidget->addTab(formTraffic, "Traffic");
    ui->tabWidget->addTab(formLed, "Led");

    ui->tabWidget->addTab(formA7105PingTest, "A7105 Ping Test");
    formA7105PingTest->setA7105PingTest(a7105PingTest);

    ui->tabWidget->addTab(formA7105Upploader, "A7105 Uploader");
    formA7105Upploader->setUploader(a7105Upploader);

    ui->tabWidget->addTab(formA7105Download, "A7105 Download");
    formA7105Download->setDownloader(a7105Downloader);

    ui->tabWidget->addTab(formTest, "Test");

    batchDownloader->setA7105Downloader(a7105Downloader);
    formBatchDownloader->setBatchDownloader(batchDownloader);

    batchUploader->setUploader(a7105Upploader);
    formBatchUploader->setBatchUploader(batchUploader);

    formBatchUploader->setAttribute(Qt::WA_ShowModal, true);
    formBatchDownloader->setAttribute(Qt::WA_ShowModal, true);

    statusBar()->addWidget(labelStatusPortName);
    statusBar()->addWidget(labelStatusPortError);
    statusBar()->addWidget(labelStatusPortStatistic);

    uartHandler = new UartHandler(this);
    connect(uartHandler, &UartHandler::sgnStatisticChanged,
            this, &MainWindow::sltStatisticChanged);

    QTimer::singleShot(0, this, &MainWindow::on_actionPortConfig_triggered);

    init();
}

MainWindow::~MainWindow()
{
    uartHandler->unInit();
    delete ui;
}

void MainWindow::sltStatisticChanged(int tx, int rx)
{
    labelStatusPortStatistic->setText(QString("tx: %1  rx: %2")
                                      .arg(tx).arg(rx));
}

void MainWindow::on_actionPortConfig_triggered()
{
    int ret = dialogPortConfig->exec();
    if (QDialog::Accepted == ret) {
        // use new serial port
        portName = dialogPortConfig->getPortName();
        qDebug() << "got new port name: " << portName;

        labelStatusPortName->setText(portName);

        if (uartHandler->init(portName)) {
            labelStatusPortError->setText("");
        } else {
            labelStatusPortError->setText("error!");
        }

    }
}

void MainWindow::init()
{
    connect(uartHandler, &UartHandler::sgnFrameTransmitted,
            formTraffic, &FormTraffic::sltFrameTransmitted);
    connect(uartHandler, &UartHandler::sgnFrameReceived,
            formTraffic, &FormTraffic::sltFrameReceived);

    connect(uartHandler, &UartHandler::sgnFrameReceived,
            formLed, &FormLed::sltFrameReceived);
    connect(formLed, &FormLed::sgnSendFrame,
            uartHandler, &UartHandler::sltSendFrame);

    connect(uartHandler, &UartHandler::sgnFrameReceived,
            a7105PingTest, &A7105PingTest::sltFrameReceived);
    connect(a7105PingTest, &A7105PingTest::sgnSendFrame,
            uartHandler, &UartHandler::sltSendFrame);

    connect(uartHandler, &UartHandler::sgnFrameReceived,
            a7105Upploader, &A7105Uploader::sltFrameReceived);
    connect(a7105Upploader, &A7105Uploader::sgnSendFrame,
            uartHandler, &UartHandler::sltSendFrame);

    connect(uartHandler, &UartHandler::sgnFrameReceived,
            a7105Downloader, &A7105Downloader::sltFrameReceived);
    connect(a7105Downloader, &A7105Downloader::sgnSendFrame,
            uartHandler, &UartHandler::sltSendFrame);

    connect(uartHandler, &UartHandler::sgnFrameReceived,
            formTest, &FormTest::sltFrameReceived);
    connect(formTest, &FormTest::sgnSendFrame,
            uartHandler, &UartHandler::sltSendFrame);
}

void MainWindow::on_actionTextToBin_triggered()
{
    DialogTextToImage dlg;
    dlg.exec();
}

void MainWindow::on_actionBatchUpload_triggered()
{
    formBatchUploader->show();
}

void MainWindow::on_actionBatchDownload_triggered()
{
    formBatchDownloader->show();
}
