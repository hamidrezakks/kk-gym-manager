#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRect>
#include <QDebug>
#include <QList>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTimer>
#include <QtSerialPort>
#include <QDomDocument>
#include <QDomElement>

#include <QTcpServer>
#include <QTcpSocket>

#include "rfiddialog.h"
#include "addmemberdialog.h"
#include "netsettingdialog.h"
#include "rfidsettingdialog.h"
#include "about.h"

const QString DEFAULT_DIR_KEY("default_dir");
const QString CLIENT1_IP_KEY("client_1_ip");
const QString CLIENT1_PORT_KEY("client_1_port");
const QString CLIENT2_IP_KEY("client_2_ip");
const QString CLIENT2_PORT_KEY("client_2_port");
const QString CLIENT3_IP_KEY("client_3_ip");
const QString CLIENT3_PORT_KEY("client_3_port");

const QString RFID_PORT_KEY("RFID_port");
const QString RFID_DATA_LENGTH_KEY("RFID_data_length");

const QString settingAddress(QDir::currentPath()+"/data/setting.ini");

struct kkRecord {
    int id;
    QString RFID;
    QString firstname;
    QString lastname;
    int age;
    int height;
    double weight;
    QString expireDate;
    bool updated;
};

struct kkIdAndVal {
    int id;
    double val;
};

struct kkDateAndVal {
    QString date;
    double val;
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

    void closeEvent(QCloseEvent *);

    QSqlDatabase myDB;



private slots:

    void manualRFIDReceived(QString str);

    void updateTable();

    void netSettingChanged();

    void RFIDSettingChanged();

    void SPHandleReadyRead();
    void SPHandleTimeout();
    void SPHandleError(QSerialPort::SerialPortError error);

    void taskTimerHandle();

    void on_pushButtonRFID_clicked();

    void on_pushButtonAdd_clicked();

    void on_tableWidget_itemSelectionChanged();

    void on_pushButtonEdit_clicked();

    void on_pushButtonApply_clicked();

    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_pushButtonDelete_clicked();

    void on_action_triggered();

    void on_action_2_triggered();

    //---------tcp----------//
    void acceptConnection1();
    void startRead1();

    void acceptConnection2();
    void startRead2();

    void acceptConnection3();
    void startRead3();
    //-----end of tcp-------//
    void on_action_3_triggered();

private:
    Ui::MainWindow *ui;

    rfiddialog *RFIDDialog;
    addmemberdialog *addMemberDialog;
    netsettingdialog *netSettingDialog;
    rfidsettingdialog *RFIDSettingDialog;
    about *aboutDialog;

    QList<kkRecord> tableRecords;
    int currentCol, currentRow;
    bool tableInProgress;
    bool listUpdated;

    QSerialPort *serialPort;
    QByteArray  SPData;
    QString SPStandardOutput;
    QTimer      *SPTimer;

    QString currentSerialPortName;

    QTimer *taskTimer;

    int taskSequence;

    void initGeometry();
    void loadTable();
    void loadSQL();
    void updateCell(int tRow);
    void applyUpdate();

    void openAddDialog(QString _RFID = "");
    void openEditDilaog(int tID);
    void openEditDilaog(QString _RFID);

    QByteArray getImageData(QImage tImage);

    void addRecord(int tID, double tCal, int tMode);
    void loadPlot(int tID, double tPlot, int tMode, int clientIndex);

    void sortList(QList<kkIdAndVal> &tList);
    int getRank(QList<kkIdAndVal> &tList, int tID);
    void sendPlotData(QList<kkDateAndVal> tList, double tTotal, int tDRank, int tGRank, int tMode, int clientIndex);

    void getGlobalData(int tID, int &rank, double &wPoint, double &tPoint, double &eCal);
    //----------tcp------------//
    QTcpServer server1;
    QTcpServer server2;
    QTcpServer server3;
    QTcpSocket *client1;
    QTcpSocket *client2;
    QTcpSocket *client3;

    QByteArray tcpData1;
    QByteArray tcpData2;
    QByteArray tcpData3;

    QList<QByteArray> client1DataList;
    QList<QByteArray> client2DataList;
    QList<QByteArray> client3DataList;
    //-------end of tcp---------//

    bool isXMLRight(QByteArray &tData);
    void checkTcpXML(int clientIndex = 1);
    void checkRFID(QString tStr, int clientIndex);



};

#endif // MAINWINDOW_H
