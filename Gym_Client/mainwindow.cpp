#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    RFIDDialog = new rfiddialog(this);
    connect(RFIDDialog, SIGNAL(manualRFIDReady(QString)), this, SLOT(manualRFIDReceived(QString)));

    notifDialog = new notifdialog(this);
    connect(notifDialog, SIGNAL(userApplied()), this, SLOT(loadMemberData()));

    settingDialog = new settingdialog(this);
    connect(settingDialog, SIGNAL(settingsChanged()), this, SLOT(settingChanged()));

    calDialog = new caldialog(this);
    connect(calDialog, SIGNAL(calEntered(double)), this, SLOT(calEntered(double)));

    aboutDialog = new about(this);

    tcpSocket = new QTcpSocket( this );

    connect( tcpSocket, SIGNAL(readyRead()), SLOT(readTcpData()) );
    connect( tcpSocket, SIGNAL(connected()), SLOT(tcpConnected()) );
    connect( tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(handleError(QAbstractSocket::SocketError)) );

    QSettings settings(settingAddress, QSettings::IniFormat);

    tcpSocket->connectToHost(settings.value(SERVER_IP_KEY).toString(),
                             settings.value(SERVER_PORT_KEY).toUInt());


    SPTimer = new QTimer(this);
    connect(SPTimer, SIGNAL(timeout()), this, SLOT(SPHandleTimeout()));

    taskTimer = new QTimer(this);
    connect( taskTimer, SIGNAL(timeout()), this, SLOT(taskTimerHandle()));

    taskTimer->start(300);

    initGeometry();

    currentMember.id = -1;
    currentPlot.mode = -1;
    currentPlotCheck = false;
    isConnected = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    initGeometry();
    if(currentPlotMode != -1 && isConnected)
        drawPlot(currentPlotCheck, currentPlotMode);
}

void MainWindow::taskTimerHandle()
{
    checkTcpXml();
}

void MainWindow::initGeometry()
{
    int wWidth = ui->centralWidget->width(),
        wHeight = ui->centralWidget->height();

    QRect tabWidget(10, 10, wWidth - 20, wHeight - 50);
    ui->tabWidget->setGeometry(tabWidget);

    QRect addBtnRect(wWidth - 110, wHeight - 35, 100, 30);
    ui->btnAddRecord->setGeometry(addBtnRect);

    int tWidth = ui->tabWidget->width() - 4, tHeight = ui->tabWidget->height() - 25;
    QRect labelCalRect(5, 5, tWidth - 10, tHeight - 55);
    ui->lCal->setGeometry(labelCalRect);
    ui->lCal->setStyleSheet("QLabel {background-color: #eee;};");

    ui->lPoint->setGeometry(labelCalRect);
    ui->lPoint->setStyleSheet("QLabel {background-color: #eee;};");

    ui->lGloabalCal->setGeometry(labelCalRect);
    ui->lGloabalCal->setStyleSheet("QLabel {background-color: #eee;};");

    QRect labelProgRect(5, 5, tWidth - 10, tHeight - 20);
    ui->lProgCal->setGeometry(labelProgRect);
    ui->lProgCal->setStyleSheet("QLabel {background-color: #eee;};");

    ui->lProgPoint->setGeometry(labelProgRect);
    ui->lProgPoint->setStyleSheet("QLabel {background-color: #eee;};");

    int newTWidth = tWidth - 15;
    QRect lDailyRect(newTWidth/2 + 5, tHeight - 50, newTWidth/2, 45);
    QRect lGlobalRect(5, tHeight - 50, newTWidth/2, 45);

    ui->lDRank1->setGeometry(lDailyRect);
    ui->lDRank2->setGeometry(lDailyRect);
    ui->lDRank3->setGeometry(lDailyRect);

    ui->lGRank1->setGeometry(lGlobalRect);
    ui->lGRank2->setGeometry(lGlobalRect);
    ui->lGRank3->setGeometry(lGlobalRect);

    QRect rfidRect(10, wHeight - 35, 100, 30);
    ui->btnRFID->setGeometry(rfidRect);

}

void MainWindow::on_btnRFID_clicked()
{
    RFIDDialog->resetAll();
    RFIDDialog->setWindowTitle("Manual RFID");
    RFIDDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    RFIDDialog->show();
}

void MainWindow::manualRFIDReceived(QString str)
{
    QByteArray tempBytes;
    tempBytes.append("<data><vld>1</vld><rfid>"+str+"</rfid><vld>1</vld></data>");
    tcpSocket->write(tempBytes);
}

bool MainWindow::isXMLRight(QByteArray &tData)
{
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

void MainWindow::checkTcpXml()
{
    if(dataList.length() <= 0)
        return;
    QByteArray tempBytes;
    tempBytes = dataList.first();
    dataList.removeFirst();

    QDomDocument doc("streamdoc");

    doc.setContent(tempBytes);

    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    bool rfidCheck = false;
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull())
        {
            if(e.tagName() == "member")
            {
                if(e.elementsByTagName("exist").at(0).firstChild().nodeValue().toInt() == 1)
                {
                    rfidCheck = true;
                }
                else
                    showNotif(false);
                if(rfidCheck)
                {
                    currentMember.id = e.elementsByTagName("id").at(0).firstChild().nodeValue().toInt();
                    currentMember.RFID = e.elementsByTagName("rfid").at(0).firstChild().nodeValue();
                    currentMember.firstname = e.elementsByTagName("fname").at(0).firstChild().nodeValue();
                    currentMember.lastname = e.elementsByTagName("lname").at(0).firstChild().nodeValue();
                    currentMember.todayPoint = e.elementsByTagName("tpoint").at(0).firstChild().nodeValue().toDouble();
                    currentMember.totalPoint = e.elementsByTagName("totpoint").at(0).firstChild().nodeValue().toDouble();
                    currentMember.rank = e.elementsByTagName("rank").at(0).firstChild().nodeValue().toInt();
                    currentMember.eCal = e.elementsByTagName("ecal").at(0).firstChild().nodeValue().toDouble();
                    currentMember.image = getImageByData(e.elementsByTagName("image").at(0).firstChild().nodeValue());
                    showNotif(true);
                }
            }
            if(e.tagName() == "plot")
            {
                currentPlot.mode = e.elementsByTagName("mode").at(0).firstChild().nodeValue().toInt();
                currentPlot.todayRank = e.elementsByTagName("trank").at(0).firstChild().nodeValue().toInt();
                currentPlot.globalRank = e.elementsByTagName("grank").at(0).firstChild().nodeValue().toInt();
                currentPlot.totalVal = e.elementsByTagName("total").at(0).firstChild().nodeValue().toDouble();
                for(int i = 0; i < 7; i++)
                {
                    if(!e.elementsByTagName("d"+QString::number(i+1)).isEmpty())
                    {
                        currentPlot.points[i] = e.elementsByTagName("p"+QString::number(i+1)).at(0).firstChild().nodeValue().toDouble();
                        currentPlot.dates[i] = e.elementsByTagName("d"+QString::number(i+1)).at(0).firstChild().nodeValue();
                    }
                    else
                    {
                        currentPlot.points[i] = 0.0;
                        currentPlot.dates[i] = "";
                    }
                }
                if(currentPlot.mode == 0 || currentPlot.mode == 1 || currentPlot.mode == 4)
                {
                    drawPlot(true, currentPlot.mode);
                    currentPlotCheck = true;
                }
                else
                {
                    drawPlot(false, currentPlot.mode);
                    currentPlotCheck = false;
                }
                currentPlotMode = currentPlot.mode;

            }
        }
        n = n.nextSibling();
    }
}

void MainWindow::readTcpData()
{
    tcpData.append(tcpSocket->readAll());
    qDebug()<<tcpData;
    if(isXMLRight(tcpData))
    {
        dataList.append(tcpData);
        tcpData.clear();
    }

    //checkTcpXml();
}

void MainWindow::tcpConnected()
{
    ui->statusBar->showMessage("Connected", 3000);
    isConnected = true;
}

void MainWindow::handleError(QAbstractSocket::SocketError error)
{
    //->statusBar->showMessage(, 3000);
    qDebug()<<error;
}

void MainWindow::showNotif(bool tCheck)
{
    /*QRect tempRect = QApplication::desktop()->screenGeometry();
    notifDialog->move(tempRect.width()/2 - notifDialog->width()/2,
                           tempRect.height()/2 - notifDialog->height()/2);*/
    notifDialog->setExist(tCheck);
    notifDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    notifDialog->setWindowTitle("Welcome");
    if(tCheck)
    {
        notifDialog->loadMember(currentMember);
    }
    notifDialog->show();
}

QImage MainWindow::getImageByData(QString tStr)
{
    QByteArray ba;
    ba.append(tStr);
    QByteArray by = ba.fromBase64(ba);
    QImage image = QImage::fromData(by, "PNG");
    return image;
}

void MainWindow::on_action_triggered()
{
    /*QDesktopWidget widget;
    QRect tempRect = widget.availableGeometry(widget.primaryScreen());
    settingDialog->move(tempRect.width()/2 - settingDialog->width()/2,
                           tempRect.height()/2 - settingDialog->height()/2);*/
    settingDialog->loadSettings();
    settingDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    settingDialog->setWindowTitle("Settings");
    settingDialog->show();
}

void MainWindow::settingChanged()
{
    qDebug()<<"yes";
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

void MainWindow::on_btnAddRecord_clicked()
{
    if(currentMember.id == -1)
        return;
    calDialog->clearAll();
    calDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    calDialog->setWindowTitle("Enter the burnt calorie");
    calDialog->show();
}

void MainWindow::calEntered(double tCal)
{
    QSettings settings(settingAddress, QSettings::IniFormat);
    QByteArray tempBytes;
    tempBytes.append("<data><vld>1</vld><cal><id>"+QString::number(currentMember.id)+"</id>"\
                     "<val>"+QString::number(tCal)+"</val><mode>"+settings.value(CLIENT_MODE_KEY).toString()+"</mode></cal><vld>1</vld></data>");
    tcpSocket->write(tempBytes);
    QMessageBox::information(this, "", "Record has been added successfully");
}

void MainWindow::loadMemberData()
{
    int currentTabIndex = ui->tabWidget->currentIndex();
    QSettings settings(settingAddress, QSettings::IniFormat);
    QByteArray tempBytes;
    tempBytes.append("<data><vld>1</vld><plot><id>"+QString::number(currentMember.id)+"</id>"\
                 "<val>"+QString::number(currentTabIndex)+"</val><mode>"+settings.value(CLIENT_MODE_KEY).toString()+"</mode></plot><vld>1</vld></data>");
    tcpSocket->write(tempBytes);

}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(currentMember.id == -1)
        return;
    loadMemberData();
}

double MainWindow::findMax(bool tMode)
{
    double tempData[8];
    for(int i = 0; i < 7; i++)
    {
        tempData[i] = currentPlot.points[i];
    }
    if(tMode)
        tempData[7] = currentPlot.totalVal;

    int tempMax = 0;
    for(int i = 0; i < (tMode ? 8 : 7); i++)
    {
        if(tempData[i] > tempMax)
            tempMax = tempData[i];
    }

    return tempMax;
}

void MainWindow::drawPlot(bool tMode, int tPlot)
{
    //int scale = 128;
    int height, width;
    if(tMode)
    {
        height = ui->lGloabalCal->height();
        width = ui->lGloabalCal->width();
    }
    else
    {
        height = ui->lProgCal->height();
        width = ui->lProgCal->width();
    }

    int tempMargin = 40;
    double tempMax = findMax(tMode);
    double tempScale = (1.0*height - tempMargin*2)/tempMax;

    QPen fPen;
    fPen.setColor(QColor(30, 30, 30));
    fPen.setWidth(2);
    QTextOption tempFont;
    tempFont.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QPixmap pix(width,height);

    pix.fill(QColor(227, 222, 219));

    QPainter painter(&pix);

    painter.setPen(QColor(255,255,255,255));

    for(int i = 20 ; i < width; i += 40)
    {
        painter.drawLine(QPoint(i,0),QPoint(i,height));
    }
    for(int i= 20; i < height; i += 40)
    {
        painter.setPen(QColor(255,255,255,255));
        painter.drawLine(QPoint(0,i),QPoint(width,i));
        painter.setPen(fPen);
        painter.setFont(QFont("Arial", 9, QFont::Normal));
        painter.drawText(QRect(5, i - 20, 50, 20), QString::number( (height - i + tempMargin)*(tempMax/height) ) );
    }


    QPolygon polyX1,polyX2;
    if(tMode)
    {
        polyX1<<QPoint(tempMargin + 50, (height - tempMargin) - currentPlot.totalVal*tempScale);
        polyX2<<QPoint(tempMargin + 50, (height - tempMargin) - currentPlot.totalVal*tempScale);
    }

    if(tMode)
    {
        for(int i = 0; i < 7; i++)
        {
            if(currentPlot.dates[i].length() > 1)
            {
                polyX1<<QPoint((i+1) * (width - tempMargin*2 - 50)/7 + tempMargin + 50, (height - tempMargin) - currentPlot.points[i]*tempScale);
                polyX2<<QPoint((i+1) * (width - tempMargin*2 - 50)/7 + tempMargin + 50, (height - tempMargin) - currentPlot.points[i]*tempScale);
            }
        }
    }
    else
    {
        for(int i = 0; i < 7; i++)
        {
            if(currentPlot.dates[i].length() > 1)
            {
                polyX1<<QPoint(i * (width - tempMargin*2 - 25)/6 + tempMargin + 25, (height - tempMargin) - currentPlot.points[i]*tempScale);
                polyX2<<QPoint(i * (width - tempMargin*2 - 25)/6 + tempMargin + 25, (height - tempMargin) - currentPlot.points[i]*tempScale);
            }
        }
    }

    if(!tMode)
    {
        QPainterPath path;
        path.addPolygon(polyX1);
        QPen penX1(QBrush(QColor(0, 95, 241, 128),Qt::SolidPattern), 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(penX1);
        painter.drawPath(path);

        path = QPainterPath();
        path.addPolygon(polyX2);
        QPen penX2(QBrush(QColor(0, 95, 241, 196),Qt::SolidPattern), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(penX2);
        painter.drawPath(path);
    }
    else
    {
        QPen penX1(QBrush(QColor(0, 95, 241, 128),Qt::SolidPattern), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        QBrush rectBrush;
        rectBrush.setColor(QColor(0, 95, 241, 64));
        rectBrush.setStyle(Qt::Dense1Pattern);

        painter.setPen(penX1);
        painter.setBrush(rectBrush);
        for(int i = 0; i < polyX1.length(); i++)
        {
            painter.drawRect(QRect(polyX1.at(i).x() - 15, polyX1.at(i).y(), 30, height - tempMargin));
        }
    }

    painter.setPen(fPen);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    if(tMode)
    {
        fPen.setColor(QColor(10, 10, 10));
        fPen.setWidth(2);
        painter.setPen(fPen);
        for(int i = 0; i < polyX1.length(); i++)
        {
            if(i == 0)
                painter.drawText(QRect(polyX1.at(i).x()-25, height-20, 50, 20), "Total", tempFont);
            else
                painter.drawText(QRect(polyX1.at(i).x()-25, height-20, 50, 20), currentPlot.dates[i-1].split(".").join("/"), tempFont);
        }
    }
    else
    {
        for(int i = 0; i < polyX1.length(); i++)
        {
            painter.drawText(QRect(polyX1.at(i).x()-25, height-20, 50, 20), currentPlot.dates[i].split(".").join("/"), tempFont);
        }
    }

    if(tPlot == 0)
    {
        ui->lCal->setPixmap(pix);
        ui->lDRank1->setText("Today rank: "+QString::number(currentPlot.todayRank));
        ui->lGRank1->setText("Global rank: "+QString::number(currentPlot.globalRank));
    }
    else if(tPlot == 1)
    {
        ui->lPoint->setPixmap(pix);
        ui->lDRank2->setText("Today rank: "+QString::number(currentPlot.todayRank));
        ui->lGRank2->setText("Global rank: "+QString::number(currentPlot.globalRank));
    }
    else if(tPlot == 2)
    {
        ui->lProgCal->setPixmap(pix);
    }
    else if(tPlot == 3)
    {
        ui->lProgPoint->setPixmap(pix);
    }
    else if(tPlot == 4)
    {
        ui->lGloabalCal->setPixmap(pix);
        ui->lDRank3->setText("Today rank: "+QString::number(currentPlot.todayRank));
        ui->lGRank3->setText("Global rank: "+QString::number(currentPlot.globalRank));
    }


    //ui->label_2->setPixmap(tPix->scaled(QSize(ui->label_2->width(),ui->label_2->height()),Qt::IgnoreAspectRatio));

}

void MainWindow::on_action_2_triggered()
{
    /*QRect tempRect = QApplication::desktop()->screenGeometry();
    aboutDialog->move(tempRect.width()/2 - aboutDialog->width()/2,
                           tempRect.height()/2 - aboutDialog->height()/2);*/
    aboutDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    aboutDialog->setWindowTitle("About");
    aboutDialog->show();
}
