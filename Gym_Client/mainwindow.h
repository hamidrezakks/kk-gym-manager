#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QList>

#include <QDomDocument>
#include <QDomElement>

#include <QTcpServer>
#include <QTcpSocket>

#include <QTimer>
#include <QtSerialPort>

#include <QImage>
#include <QSettings>

#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QBrush>

#include "rfiddialog.h"
#include "notifdialog.h"
#include "settingdialog.h"
#include "caldialog.h"
#include "about.h"

const QString SERVER_IP_KEY("server_ip");
const QString SERVER_PORT_KEY("server_port");

const QString RFID_PORT_KEY("RFID_port");
const QString RFID_DATA_LENGTH_KEY("RFID_data_length");

const QString CLIENT_MODE_KEY("client_mode");

const QString settingAddress(QDir::currentPath()+"/setting.ini");

struct kkPlot {
    double points[7];
    QString dates[7];
    int todayRank;
    int globalRank;
    double totalVal;
    int mode;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void resizeEvent(QResizeEvent *);

private slots:

    void manualRFIDReceived(QString str);

    void readTcpData();

    void tcpConnected();

    void handleError(QAbstractSocket::SocketError);

    void taskTimerHandle();

    void RFIDSettingChanged();

    void SPHandleReadyRead();
    void SPHandleTimeout();
    void SPHandleError(QSerialPort::SerialPortError error);

    void settingChanged();

    void calEntered(double tCal);

    void loadMemberData();

    void on_btnRFID_clicked();

    void on_action_triggered();

    void on_btnAddRecord_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_action_2_triggered();

private:
    Ui::MainWindow *ui;

    rfiddialog *RFIDDialog;
    notifdialog *notifDialog;
    settingdialog *settingDialog;
    caldialog *calDialog;
    about *aboutDialog;

    QTimer *taskTimer;

    QSerialPort *serialPort;
    QByteArray  SPData;
    QString SPStandardOutput;
    QTimer      *SPTimer;

    QString currentSerialPortName;

    void checkTcpXml();

    void showNotif(bool tCheck);
    QImage getImageByData(QString tStr);

    kkPlot currentPlot;
    void drawPlot(bool tMode, int tPlot);
    double findMax(bool tMode);
    int currentPlotMode;
    int currentPlotCheck;
    //-------tcp---------//
    QByteArray tcpData;
    QTcpSocket *tcpSocket;
    QList<QByteArray> dataList;
    bool isXMLRight(QByteArray &tData);
    bool isConnected;
    //-----end of tcp------//

    void initGeometry();

    kkMember currentMember;


};

#endif // MAINWINDOW_H
