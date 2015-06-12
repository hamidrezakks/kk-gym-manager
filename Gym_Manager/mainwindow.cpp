#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Gym Manager by KK");

    currentCol = -1;
    currentRow = -1;

    tableInProgress = false;
    listUpdated = false;

    RFIDDialog = new rfiddialog(this);
    connect(RFIDDialog, SIGNAL(manualRFIDReady(QString)), this, SLOT(manualRFIDReceived(QString)));

    netSettingDialog = new netsettingdialog(this);
    connect(netSettingDialog, SIGNAL(netSettingChanged()), this, SLOT(netSettingChanged()));

    RFIDSettingDialog = new rfidsettingdialog(this);
    connect(RFIDSettingDialog, SIGNAL(RFIDSettingChangd()), this, SLOT(RFIDSettingChanged()));

    aboutDialog = new about(this);

    SPTimer = new QTimer(this);
    connect(SPTimer, SIGNAL(timeout()), this, SLOT(SPHandleTimeout()));

    QSettings settings(settingAddress, QSettings::IniFormat);

    connect(&server1, SIGNAL(newConnection()), this, SLOT(acceptConnection1()));
    server1.listen(QHostAddress::Any, settings.value(CLIENT1_PORT_KEY).toUInt());

    connect(&server2, SIGNAL(newConnection()), this, SLOT(acceptConnection2()));
    server2.listen(QHostAddress::Any, settings.value(CLIENT2_PORT_KEY).toUInt());

    connect(&server3, SIGNAL(newConnection()), this, SLOT(acceptConnection3()));
    server3.listen(QHostAddress::Any, settings.value(CLIENT3_PORT_KEY).toUInt());

    ui->tableWidget->setStyleSheet("QTableWidget { background-image: url('image/bg.png'); background-position: center center; background-repeat: no-repeat;}");

    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName(QDir::currentPath()+"/data/db.db3");
    if (!myDB.open()) {
        QMessageBox::critical(0, tr("Cannot open database"),
            tr("Unable to establish a database connection.\n"
              "This example needs SQLite support. Please read "
              "the Qt SQL driver documentation for information how "
              "to build it.\n\n"
              "Click Cancel to exit."), QMessageBox::Cancel);
    }

    if(!myDB.tables().contains("memberlist"))
    {
        QSqlQuery squery;
        squery.exec("CREATE TABLE memberlist (id int PRIMARY KEY, RFID varchar(12), "\
                    "firstname varchar(64), lastname varchar(64), pic_dir varchar(128), "\
                    "m_age int, m_height int, m_weight varchar(8), m_sport varchar(256), "\
                    "m_illness varchar(256), expire_date varchar(32), date varchar(32), state int, m_sex varchar(2) )");
    }

    if(!myDB.tables().contains("memberrecord"))
    {
        QSqlQuery squery;
        squery.exec("CREATE TABLE memberrecord (id int PRIMARY KEY, m_id int, d_mode int, "\
                    "d_cal varchar(10), d_point varchar(10), d_date int(10), date varchar(32), state int )");
    }

    loadSQL();
    loadTable();

    taskTimer = new QTimer(this);
    connect( taskTimer, SIGNAL(timeout()), this, SLOT(taskTimerHandle()));

    taskTimer->start(500);
    taskSequence = 0;
}

void MainWindow::taskTimerHandle()
{
    checkTcpXML(taskSequence+1);
    taskSequence++;
    taskSequence = taskSequence % 3;
}

void MainWindow::initGeometry()
{
    int wWidth = ui->centralWidget->width(),
        wHeight = ui->centralWidget->height();

    QRect tableWidgetRect(10, 10, wWidth - 20, wHeight - 50);
    ui->tableWidget->setGeometry(tableWidgetRect);

    QRect addBtnRect(wWidth - 110, wHeight - 35, 100, 30);
    ui->pushButtonAdd->setGeometry(addBtnRect);

    QRect editBtnRect(wWidth - 215, wHeight - 35, 100, 30);
    ui->pushButtonEdit->setGeometry(editBtnRect);

    QRect deleteBtnRect(wWidth - 320, wHeight - 35, 100, 30);
    ui->pushButtonDelete->setGeometry(deleteBtnRect);

    QRect applyBtnRect(wWidth - 445, wHeight - 35, 120, 30);
    ui->pushButtonApply->setGeometry(applyBtnRect);

    ui->pushButtonApply->setVisible(false);

    QRect rfidRect(10, wHeight - 35, 100, 30);
    ui->pushButtonRFID->setGeometry(rfidRect);

    for(int i = 0; i < 6; i++)
    {
        ui->tableWidget->setColumnWidth(i, ui->tableWidget->geometry().width()/6);
    }
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    initGeometry();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonRFID_clicked()
{
    RFIDDialog->resetAll();
    RFIDDialog->setWindowTitle("Manual RFID");
    RFIDDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    RFIDDialog->show();
}

void MainWindow::manualRFIDReceived(QString str)
{
    QSqlQuery squery("SELECT id FROM memberlist WHERE state = 1 AND RFID = '"+str+"' ORDER BY id DESC");
    if(squery.next())
    {
        /*QSqlQuery zquery;
        zquery.exec("UPDATE memberrecord SET d_date = 20150406, date = '06.04.2015 6:45:12 pm' WHERE id = 10023 ; ");
        zquery.exec("UPDATE memberrecord SET d_date = 20150407, date = '07.04.2015 6:36:2 pm' WHERE id = 10020 ; ");
        zquery.exec("UPDATE memberrecord SET d_date = 20150407, date = '07.04.2015 6:32:50 pm' WHERE id = 10018 ; ");
        QSqlQuery dquery("SELECT d_date, date, id FROM memberrecord WHERE state = 1 AND m_id = "+squery.value(0).toString()+" ORDER BY id DESC");
        while(dquery.next())
        {
            qDebug()<<dquery.value(0).toString()<<dquery.value(1).toString()<<dquery.value(2).toString();
        }*/
        openEditDilaog(str);
    }
    else
        openAddDialog(str);
}

void MainWindow::updateTable()
{
    loadSQL();;
    loadTable();
}

void MainWindow::loadTable()
{
    tableInProgress = true;
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount( tableRecords.length() );
    for(int i = 0; i < tableRecords.length(); i++)
    {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem( tableRecords.at(i).firstname ));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem( tableRecords.at(i).lastname ));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem( QString::number(tableRecords.at(i).age) ));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem( QString::number(tableRecords.at(i).height) ));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem( QString::number(tableRecords.at(i).weight) ));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem( tableRecords.at(i).expireDate ));
    }
    tableInProgress = false;
}

void MainWindow::loadSQL()
{
    tableRecords.clear();
    kkRecord tempRecord;
    QSqlQuery tempQuery("SELECT id, RFID, firstname, lastname, m_age, m_height, m_weight, expire_date FROM memberlist WHERE state = 1 ORDER BY id ASC");
    while(tempQuery.next())
    {
        tempRecord.id = tempQuery.value(0).toInt();
        tempRecord.RFID = tempQuery.value(1).toString();
        tempRecord.firstname = tempQuery.value(2).toString();
        tempRecord.lastname = tempQuery.value(3).toString();
        tempRecord.age = tempQuery.value(4).toInt();
        tempRecord.height = tempQuery.value(5).toInt();
        tempRecord.weight = tempQuery.value(6).toDouble();
        tempRecord.expireDate = tempQuery.value(7).toString();
        tempRecord.updated = false;
        tableRecords.append(tempRecord);
    }
}

void MainWindow::updateCell(int tRow)
{
    kkRecord tempRecord;
    tempRecord.updated = true;
    tempRecord.id = tableRecords.at(tRow).id;
    tempRecord.RFID = tableRecords.at(tRow).RFID;
    tempRecord.firstname = ui->tableWidget->item(tRow, 0)->text();
    tempRecord.lastname = ui->tableWidget->item(tRow, 1)->text();
    tempRecord.age = ui->tableWidget->item(tRow, 2)->text().toInt();
    tempRecord.height = ui->tableWidget->item(tRow, 3)->text().toInt();
    tempRecord.weight = ui->tableWidget->item(tRow, 4)->text().toDouble();
    tempRecord.expireDate = ui->tableWidget->item(tRow, 5)->text();

    tableRecords.replace(tRow, tempRecord);
    listUpdated = true;
    ui->pushButtonApply->setVisible(true);
}

void MainWindow::applyUpdate()
{
    if(!listUpdated)
        return;
    QString tempStr;
    QSqlQuery tempQuery;
    kkRecord tempRecord;
    bool tempCheck;
    for(int i = 0; i < tableRecords.length(); i++)
    {
        if(tableRecords.at(i).updated)
        {
            tempStr = "UPDATE memberlist SET firstname = '"+tableRecords.at(i).firstname+"', "\
                  "lastname = '"+tableRecords.at(i).lastname+"', m_age = "+QString::number(tableRecords.at(i).age)+", "\
                  "m_height = "+QString::number(tableRecords.at(i).height)+", m_weight = '"+QString::number(tableRecords.at(i).weight)+"', "\
                  "expire_date = '"+tableRecords.at(i).expireDate+"' WHERE id = "+QString::number(tableRecords.at(i).id)+" ;";
            tempCheck = tempQuery.exec(tempStr);

            if(tempCheck)
            {
                tempRecord.updated = false;
                tempRecord.id = tableRecords.at(i).id;
                tempRecord.RFID = tableRecords.at(i).RFID;
                tempRecord.firstname = tableRecords.at(i).firstname;
                tempRecord.lastname = tableRecords.at(i).lastname;
                tempRecord.age = tableRecords.at(i).age;
                tempRecord.height = tableRecords.at(i).height;
                tempRecord.weight = tableRecords.at(i).weight;
                tempRecord.expireDate = tableRecords.at(i).expireDate;

                tableRecords.replace(i, tempRecord);
            }
        }
    }
    ui->pushButtonApply->setVisible(false);
    loadTable();
    listUpdated = false;
}

void MainWindow::openAddDialog(QString _RFID)
{
    addMemberDialog = new addmemberdialog(this);
    connect(addMemberDialog, SIGNAL(sqlChanged()), this, SLOT(updateTable()));

    QRect tempRect = QApplication::desktop()->screenGeometry();
    addMemberDialog->move(tempRect.width()/2 - addMemberDialog->width()/2,
                           tempRect.height()/2 - addMemberDialog->height()/2);

    if(_RFID.length() == 10)
        addMemberDialog->setRFID(_RFID);

    addMemberDialog->setWindowTitle("Add User");
    addMemberDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    addMemberDialog->show();
}

void MainWindow::openEditDilaog(int tID)
{
    addMemberDialog = new addmemberdialog(this);
    connect(addMemberDialog, SIGNAL(sqlChanged()), this, SLOT(updateTable()));

    QRect tempRect = QApplication::desktop()->screenGeometry();
    addMemberDialog->move(tempRect.width()/2 - addMemberDialog->width()/2,
                           tempRect.height()/2 - addMemberDialog->height()/2);

    addMemberDialog->loadMemberData(tID);
    addMemberDialog->setWindowTitle("Modify User");
    addMemberDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    addMemberDialog->show();
}

void MainWindow::openEditDilaog(QString _RFID)
{
    addMemberDialog = new addmemberdialog(this);
    connect(addMemberDialog, SIGNAL(sqlChanged()), this, SLOT(updateTable()));

    QRect tempRect = QApplication::desktop()->screenGeometry();
    addMemberDialog->move(tempRect.width()/2 - addMemberDialog->width()/2,
                           tempRect.height()/2 - addMemberDialog->height()/2);

    addMemberDialog->loadMemberData(_RFID);
    addMemberDialog->setWindowTitle("Modify user");
    addMemberDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    addMemberDialog->show();
}


void MainWindow::on_pushButtonAdd_clicked()
{
    openAddDialog();
}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    currentCol = ui->tableWidget->currentColumn();
    currentRow = ui->tableWidget->currentRow();
}

void MainWindow::on_pushButtonEdit_clicked()
{
    currentCol = ui->tableWidget->currentColumn();
    currentRow = ui->tableWidget->currentRow();
    if(currentCol < 0 || currentRow < 0)
        return;
    openEditDilaog(tableRecords.at(currentRow).id);
}

void MainWindow::on_pushButtonApply_clicked()
{
    int tempReply = QMessageBox::question(this, "Confirm",
                              "Are you sure?",
                              "No",
                              "Yes",
                              "Cancel");
    if(tempReply == 2)
        return;
    if(tempReply == 0)
    {
        loadSQL();
        loadTable();
        ui->pushButtonApply->setVisible(false);
        listUpdated = false;
    }
    else if(tempReply == 1)
        applyUpdate();
}

void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    if(tableInProgress)
        return;
    int tempRow = ui->tableWidget->currentRow();
    if(tempRow >= 0)
        updateCell(tempRow);
}

void MainWindow::on_pushButtonDelete_clicked()
{
    currentCol = ui->tableWidget->currentColumn();
    currentRow = ui->tableWidget->currentRow();
    if(currentCol < 0 || currentRow < 0)
        return;
    bool tempReply = QMessageBox::question(this, "Remove user",
                              "Are you sure?",
                              "No",
                              "Yes");
    if(tempReply)
    {
        QSqlQuery tempQuery;
        tempQuery.exec("UPDATE memberlist SET state = 0 WHERE id = "+QString::number(tableRecords.at(currentRow).id)+" ;");
        loadSQL();
        loadTable();
    }
}

void MainWindow::on_action_triggered()
{
    QRect tempRect = QApplication::desktop()->screenGeometry();
    netSettingDialog->move(tempRect.width()/2 - netSettingDialog->width()/2,
                           tempRect.height()/2 - netSettingDialog->height()/2);
    netSettingDialog->loadSettings();
    netSettingDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    netSettingDialog->setWindowTitle("Network Settings");
    netSettingDialog->show();
}

void MainWindow::netSettingChanged()
{
    qDebug()<<"YES";
}

void MainWindow::on_action_2_triggered()
{

    QRect tempRect = QApplication::desktop()->screenGeometry();
    RFIDSettingDialog->move(tempRect.width()/2 - RFIDSettingDialog->width()/2,
                           tempRect.height()/2 - RFIDSettingDialog->height()/2);
    RFIDSettingDialog->loadAvailablePorts();
    RFIDSettingDialog->loadSettings();
    RFIDSettingDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    RFIDSettingDialog->setWindowTitle("RFID Setting");
    RFIDSettingDialog->show();
}

void MainWindow::RFIDSettingChanged()
{
    QSettings settings(settingAddress, QSettings::IniFormat);

    currentSerialPortName = settings.value(RFID_PORT_KEY).toString();

    serialPort = new QSerialPort(this);
    qDebug()<<currentSerialPortName;
    serialPort->setPortName(currentSerialPortName);
    serialPort->setBaudRate(9600);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->open(QIODevice::ReadOnly);

    SPTimer->start(200);
    connect(serialPort, SIGNAL(readyRead()), SLOT(SPHandleReadyRead()));
    connect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(SPHandleError(QSerialPort::SerialPortError)));
}

void MainWindow::SPHandleReadyRead()
{
    SPData.append(serialPort->readAll());
    if (!SPTimer->isActive())
        SPTimer->start(200);
}

void MainWindow::SPHandleTimeout()
{
    if (SPData.isEmpty())
    {
        SPStandardOutput = "ERROR";
    }
    else
    {
        SPStandardOutput = QString::fromStdString(SPData.toStdString());
        ui->statusBar->showMessage(SPStandardOutput, 3000);
    }
    SPData.clear();
    if(SPStandardOutput != "ERROR")
    {
        if(SPStandardOutput.length() == 12)
            manualRFIDReceived(SPStandardOutput.mid(0, 10));
    }
}

void MainWindow::SPHandleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError)
    {
       qDebug()<<QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(serialPort->portName()).arg(serialPort->errorString());
    }
}

bool MainWindow::isXMLRight(QByteArray &tData)
{
    /*QDomDocument doc("streamdoc");

    doc.setContent(tData);
    QDomElement docElem = doc.documentElement();
    docElem*/

    QXmlStreamReader tempXML;
    int tempXMLCnt = 0;
    tempXML.addData(tData);
    while(!tempXML.atEnd() && !tempXML.hasError())
    {
        QXmlStreamReader::TokenType token = tempXML.readNext();
        //If token is just StartDocument - go to next
        if(token == QXmlStreamReader::StartDocument)
        {
            continue;
        }
        //If token is StartElement - read it
        if(token == QXmlStreamReader::StartElement)
        {
            if(tempXML.name() == "data") {
                continue;
            }
            if(tempXML.name() == "vld")
            {
                tempXMLCnt++;
            }
        }
    }
    if(tempXMLCnt == 2)
        return true;
    else
        return false;
}

void MainWindow::checkRFID(QString tStr, int clientIndex)
{
    qDebug()<<tStr;
    QSqlQuery squery("SELECT id, firstname, lastname, pic_dir FROM memberlist WHERE state = 1 AND RFID = '"+tStr+"' ORDER BY id DESC");
    QByteArray tempBytes;
    if(squery.next())
    {
        double todayPoint;
        double totalPoint;
        double essenCal;
        int rank;

        getGlobalData(squery.value(0).toInt(), rank, totalPoint, todayPoint, essenCal);

        tempBytes.append("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
        tempBytes.append("<data><vld>1</vld><member><exist>1</exist>");
        tempBytes.append("<id>"+squery.value(0).toString()+"</id>");
        tempBytes.append("<rfid>"+tStr+"</rfid>");
        tempBytes.append("<fname>"+squery.value(1).toString()+"</fname>");
        tempBytes.append("<lname>"+squery.value(2).toString()+"</lname>");
        tempBytes.append("<tpoint>"+QString::number(todayPoint)+"</tpoint>");
        tempBytes.append("<totpoint>"+QString::number(totalPoint)+"</totpoint>");
        tempBytes.append("<rank>"+QString::number(rank)+"</rank>");
        tempBytes.append("<ecal>"+QString::number(essenCal)+"</ecal>");
        QImage tempImage(QDir::currentPath()+squery.value(3).toString());
        tempBytes.append("<image><![CDATA[");
        tempBytes.append(getImageData(tempImage));
        tempBytes.append("]]></image>");
        tempBytes.append("<end>1</end>");
        tempBytes.append("</member><vld>1</vld></data>");
    }
    else
    {
        tempBytes.append("<data><vld>1</vld><member><exist>0</exist></member><vld>1</vld></data>");
    }
    if(clientIndex == 1)
        client1->write(tempBytes);
    else if(clientIndex == 2)
        client2->write(tempBytes);
    else if(clientIndex == 3)
        client3->write(tempBytes);
}

void MainWindow::addRecord(int tID, double tCal, int tMode)
{
    QSqlQuery tempQuery;
    QString tempDate = QDateTime::currentDateTime().toString("dd.MM.yyyy h:m:s ap");
    QString tempDate2 = QDateTime::currentDateTime().toString("yyyyMMdd");

    int tempID = 10000;

    tempQuery.exec("SELECT id FROM memberrecord WHERE state = 1 OR state = 0 ORDER BY id DESC ;");
    if(tempQuery.next())
    {
        tempID = tempQuery.value(0).toInt();
        tempID++;
    }

    int tempAge, tempHeight;
    double tempWeight;
    QString tempSex;
    tempQuery.exec("SELECT m_age, m_height, m_weight, m_sex FROM memberlist WHERE id = "+QString::number(tID)+" ;");
    if(tempQuery.next())
    {
        tempAge = tempQuery.value(0).toInt();
        tempHeight = tempQuery.value(1).toInt();
        tempWeight = tempQuery.value(2).toDouble();
        tempSex = tempQuery.value(3).toString();
    }

    double tempPoint = 1000;

    if(tempSex == "f")
    {
        tempPoint = 655.0+(9.6*tempWeight)+(1.8*tempHeight) - (4.7*tempAge);
        tempPoint = (tCal/tempPoint)*100;
    }
    else
    {
        tempPoint = 66+(13.7*tempWeight)+(5*tempHeight) - (6.8*tempAge);
        tempPoint = (tCal/tempPoint)*100;
    }

    //f BMR = 655 + ( 9.6 x weight in kilos ) + ( 1.8 x height in cm ) - ( 4.7 x age in years)
    //Men: BMR = 66 + ( 13.7 x weight in kilos ) + ( 5 x height in cm ) - ( 6.8 x age in years )

    QString tempStr = "INSERT INTO memberrecord ( `id`, `m_id`, `d_mode`, "\
                      "d_cal, d_point, d_date, date, state ) VALUES ("\
                      ""+QString::number(tempID)+", "+QString::number(tID)+", "\
                      ""+QString::number(tMode)+", '"+QString::number(tCal)+"', "\
                      "'"+QString::number(tempPoint)+"',"+tempDate2+", '"+tempDate+"', 1 );";
    if(tempQuery.exec(tempStr))
        ui->statusBar->showMessage("Query Executed, Client: "+QString::number(tMode + 1), 3000);

}

void MainWindow::checkTcpXML(int clientIndex)
{
    QByteArray tempBytes;
    if(clientIndex == 1)
    {
        if(client1DataList.length() <= 0)
            return;
        tempBytes = client1DataList.first();
        client1DataList.removeFirst();
    }
    else if(clientIndex == 2)
    {
        if(client2DataList.length() <= 0)
            return;
        tempBytes = client2DataList.first();
        client2DataList.removeFirst();
    }
    else if(clientIndex == 3)
    {
        if(client3DataList.length() <= 0)
            return;
        tempBytes = client3DataList.first();
        client3DataList.removeFirst();
    }

    QDomDocument doc("streamdoc");
    doc.setContent(tempBytes);
    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();

    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName() == "rfid")
            {
                checkRFID(e.firstChild().nodeValue(), clientIndex);
            }
            if(e.tagName() == "cal")
            {
                int tempId = e.elementsByTagName("id").at(0).firstChild().nodeValue().toInt();
                double tempCal = e.elementsByTagName("val").at(0).firstChild().nodeValue().toDouble();
                int tempMode = e.elementsByTagName("mode").at(0).firstChild().nodeValue().toInt();
                addRecord(tempId, tempCal, tempMode);
            }
            if(e.tagName() == "plot")
            {
                int tempId = e.elementsByTagName("id").at(0).firstChild().nodeValue().toInt();
                double tempPlot = e.elementsByTagName("val").at(0).firstChild().nodeValue().toInt();
                int tempMode = e.elementsByTagName("mode").at(0).firstChild().nodeValue().toInt();
                loadPlot(tempId, tempPlot, tempMode, clientIndex);
            }

        }
        n = n.nextSibling();
    }
}

void MainWindow::acceptConnection1()
{
    client1 = server1.nextPendingConnection();
    connect(client1, SIGNAL(readyRead()), this, SLOT(startRead1()));
}

void MainWindow::startRead1()
{
    tcpData1.append( client1->read(client1->bytesAvailable()) );
    if(isXMLRight(tcpData1))
    {
        client1DataList.append(tcpData1);
        tcpData1.clear();
    }
}

void MainWindow::acceptConnection2()
{
    client2 = server2.nextPendingConnection();
    connect(client2, SIGNAL(readyRead()), this, SLOT(startRead2()));
}

void MainWindow::startRead2()
{
    tcpData2.append( client2->read(client2->bytesAvailable()) );
    qDebug()<<tcpData2;
    if(isXMLRight(tcpData2))
    {
        client2DataList.append(tcpData2);
        tcpData2.clear();
    }
}

void MainWindow::acceptConnection3()
{
    client3 = server3.nextPendingConnection();
    connect(client3, SIGNAL(readyRead()), this, SLOT(startRead3()));
}

void MainWindow::startRead3()
{
    tcpData3.append( client3->read(client3->bytesAvailable()) );
    if(isXMLRight(tcpData3))
    {
        client3DataList.append(tcpData3);
        tcpData3.clear();
    }
}

QByteArray MainWindow::getImageData(QImage tImage)
{
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    tImage.save(&buffer, "PNG");
    buffer.close();
    return arr.toBase64();
}

void MainWindow::sortList(QList<kkIdAndVal> &tList)
{
    kkIdAndVal temp;
    for(int i = 0 ; i < tList.length(); i++)
    {
        for(int j = i; j < tList.length(); j++)
        {
            if(tList.at(j).val > tList.at(i).val)
            {
                temp.id = tList.at(i).id;
                temp.val = tList.at(i).val;
                tList.replace(i, tList.at(j));
                tList.replace(j, temp);
            }
        }
    }
}

int MainWindow::getRank(QList<kkIdAndVal> &tList, int tID)
{
    for(int i = 0 ; i < tList.length(); i++)
    {
        if(tList.at(i).id == tID)
            return i + 1;
    }
    return -1;
}

void MainWindow::loadPlot(int tID, double tPlot, int tMode, int clientIndex)
{
    QSqlQuery tempQuery;

    long tempDateInt = QDateTime::currentDateTime().toString("yyyyMMdd").toLong();

    tempQuery.exec("SELECT id FROM memberlist WHERE state = 1 ORDER BY id ;");
    QList<int> tempMIdList;
    while(tempQuery.next())
    {
        tempMIdList.append(tempQuery.value(0).toInt());
    }

    QString qMode = "";

    if(tPlot != 4)
    {
        if(tMode == 0)
        {
            qMode = " AND d_mode = 0 ";
        }
        else if(tMode == 1)
        {
            qMode = " AND d_mode = 1 ";
        }
        else if(tMode == 2)
        {
            qMode = " AND d_mode = 2 ";
        }
    }

    QList<kkIdAndVal> tempGPList;
    QList<kkIdAndVal> tempTPList;
    QList<kkIdAndVal> tempGCList;
    QList<kkIdAndVal> tempTCList;

    kkIdAndVal tempP, tempC;
    double totalPoint = 0;
    double totalCal = 0;
    for(int i = 0; i < tempMIdList.length(); i++)
    {
        totalPoint = totalCal = 0;
        tempQuery.exec("SELECT d_cal, d_point FROM memberrecord WHERE m_id = "+QString::number(tempMIdList.at(i))+" "+qMode+" ORDER BY id ASC;");
        while(tempQuery.next())
        {
            totalCal += tempQuery.value(0).toDouble();
            totalPoint += tempQuery.value(1).toDouble();
        }
        tempC.id = tempMIdList.at(i);
        tempC.val = totalCal;
        tempGCList.append(tempC);

        tempP.id = tempMIdList.at(i);
        tempP.val = totalPoint;
        tempGPList.append(tempP);
    }


    for(int i = 0; i < tempMIdList.length(); i++)
    {
        totalPoint = totalCal = 0;
        tempQuery.exec("SELECT d_cal, d_point FROM memberrecord WHERE m_id = "+QString::number(tempMIdList.at(i))+" AND d_date = "+QString::number(tempDateInt)+" "+qMode+" ORDER BY d_date ASC;");
        while(tempQuery.next())
        {
            totalCal += tempQuery.value(0).toDouble();
            totalPoint += tempQuery.value(1).toDouble();
        }
        tempC.id = tempMIdList.at(i);
        tempC.val = totalCal;
        tempTCList.append(tempC);

        tempP.id = tempMIdList.at(i);
        tempP.val = totalPoint;
        tempTPList.append(tempP);
    }

    QList<kkDateAndVal> dataCList;
    QList<kkDateAndVal> dataPList;
    kkDateAndVal tempDV;

    int tempTR = -1, tempGR = -1;

    totalPoint = totalCal = 0;
    tempQuery.exec("SELECT d_cal, d_point, d_date, date FROM memberrecord WHERE m_id = "+QString::number(tID)+" AND d_date > "+QString::number(tempDateInt - 7)+" "+qMode+" ORDER BY d_date ASC;");
    QString lastDate = "", tempDD;
    double lastTCal = 0, lastTPoint = 0;
    while(tempQuery.next())
    {
        if(lastDate != tempQuery.value(2).toString())
        {
            tempDV.date = tempDD;
            tempDV.val = lastTCal;
            dataCList.append(tempDV);

            tempDV.date = tempDD;
            tempDV.val = lastTPoint;
            dataPList.append(tempDV);

            lastTCal = lastTPoint = 0;
            lastDate = tempQuery.value(2).toString();
        }
        totalCal += tempQuery.value(0).toDouble();
        lastTCal += tempQuery.value(0).toDouble();

        totalPoint += tempQuery.value(1).toDouble();
        lastTPoint += tempQuery.value(1).toDouble();
        tempDD = tempQuery.value(3).toString().mid(0,5);
    }

    tempDV.date = tempDD;
    tempDV.val = lastTCal;
    dataCList.append(tempDV);

    tempDV.date = tempDD;
    tempDV.val = lastTPoint;
    dataPList.append(tempDV);

    dataCList.removeFirst();
    dataPList.removeFirst();

    if(tPlot == 0 || tPlot == 4)
    {
        sortList(tempTCList);
        tempTR = getRank(tempTCList, tID);
        sortList(tempGCList);
        tempGR = getRank(tempGCList, tID);
    }
    else if(tPlot == 1)
    {
        sortList(tempTPList);
        tempTR = getRank(tempTPList, tID);
        sortList(tempGPList);
        tempGR = getRank(tempGPList, tID);
    }

    if(tPlot == 0 || tPlot == 2 || tPlot == 4)
    {
        sendPlotData(dataCList, totalCal, tempTR, tempGR, tPlot, clientIndex);
    }
    else
    {
        sendPlotData(dataPList, totalPoint, tempTR, tempGR, tPlot, clientIndex);
    }
}

void MainWindow::sendPlotData(QList<kkDateAndVal> tList, double tTotal, int tDRank, int tGRank, int tMode, int clientIndex)
{
    QByteArray tempBytes;
    tempBytes.append("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
    tempBytes.append("<data><vld>1</vld><plot>");
    tempBytes.append("<mode>"+QString::number(tMode)+"</mode>");
    tempBytes.append("<trank>"+QString::number(tDRank)+"</trank>");
    tempBytes.append("<grank>"+QString::number(tGRank)+"</grank>");
    tempBytes.append("<total>"+QString::number(tTotal)+"</total>");

    for(int i = 0; i < tList.length() && i < 7; i++)
    {
        tempBytes.append("<p"+QString::number(i+1)+">"+QString::number(tList.at(i).val)+"</p"+QString::number(i+1)+">");
        tempBytes.append("<d"+QString::number(i+1)+">"+tList.at(i).date+"</d"+QString::number(i+1)+">");
    }

    tempBytes.append("</plot><vld>1</vld></data>");

    if(clientIndex == 1)
        client1->write(tempBytes);
    else if(clientIndex == 2)
        client2->write(tempBytes);
    else if(clientIndex == 3)
        client3->write(tempBytes);
}

void MainWindow::getGlobalData(int tID, int &rank, double &wPoint, double &tPoint, double &eCal)
{
    QSqlQuery tempQuery;

    long tempDateInt = QDateTime::currentDateTime().toString("yyyyMMdd").toLong();

    tempQuery.exec("SELECT id FROM memberlist WHERE state = 1 ORDER BY id;");
    QList<int> tempMIdList;
    while(tempQuery.next())
    {
        tempMIdList.append(tempQuery.value(0).toInt());
    }

    QString qMode = "";

    QList<kkIdAndVal> tempGPList;

    kkIdAndVal tempP, tempC;
    double totalPoint = 0;
    for(int i = 0; i < tempMIdList.length(); i++)
    {
        totalPoint = 0;
        tempQuery.exec("SELECT d_cal, d_point FROM memberrecord WHERE m_id = "+QString::number(tempMIdList.at(i))+" "+qMode+" ORDER BY id ASC;");
        while(tempQuery.next())
        {
            totalPoint += tempQuery.value(1).toDouble();
        }

        tempP.id = tempMIdList.at(i);
        tempP.val = totalPoint;
        tempGPList.append(tempP);
    }

    sortList(tempGPList);
    rank = getRank(tempGPList, tID);

    totalPoint = 0;
    tempQuery.exec("SELECT d_point FROM memberrecord WHERE m_id = "+QString::number(tID)+" AND d_date > "+QString::number(tempDateInt - 7)+" "+qMode+" ORDER BY id ASC;");
    while(tempQuery.next())
    {
        totalPoint += tempQuery.value(0).toDouble();
    }

    wPoint = totalPoint;


    totalPoint = 0;
    tempQuery.exec("SELECT d_point FROM memberrecord WHERE m_id = "+QString::number(tID)+" AND d_date = "+QString::number(tempDateInt)+" "+qMode+" ORDER BY id ASC;");
    while(tempQuery.next())
    {
        totalPoint += tempQuery.value(0).toDouble();
    }

    tPoint = totalPoint;

    int tempAge, tempHeight;
    double tempWeight, tempPoint;
    QString tempSex;

    tempQuery.exec("SELECT m_age, m_height, m_weight, m_sex FROM memberlist WHERE id = "+QString::number(tID)+" ;");
    if(tempQuery.next())
    {
        tempAge = tempQuery.value(0).toInt();
        tempHeight = tempQuery.value(1).toInt();
        tempWeight = tempQuery.value(2).toDouble();
        tempSex = tempQuery.value(3).toString();
    }

    if(tempSex == "f")
    {
        tempPoint = 655.0+(9.6*tempWeight)+(1.8*tempHeight) - (4.7*tempAge);
    }
    else
    {
        tempPoint = 66+(13.7*tempWeight)+(5*tempHeight) - (6.8*tempAge);
    }

    eCal = tempPoint;
}




void MainWindow::on_action_3_triggered()
{
    QRect tempRect = QApplication::desktop()->screenGeometry();
    aboutDialog->move(tempRect.width()/2 - aboutDialog->width()/2,
                           tempRect.height()/2 - aboutDialog->height()/2);
    aboutDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    aboutDialog->setWindowTitle("About");
    aboutDialog->show();
}

void MainWindow::closeEvent(QCloseEvent *)
{
}
