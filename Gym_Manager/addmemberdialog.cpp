#include "addmemberdialog.h"
#include "ui_addmemberdialog.h"

#include "mainwindow.h"

addmemberdialog::addmemberdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addmemberdialog)
{
    ui->setupUi(this);

    cropBand = new QRubberBand(QRubberBand::Rectangle, this);
    startCheck = false;
    startPoint = endPoint = QPoint(0,0);

    QPalette pal;
    pal.setBrush(QPalette::Highlight, QBrush(Qt::blue, Qt::Dense4Pattern));
    cropBand->setPalette(pal);

    if (QCameraInfo::availableCameras().count() > 0)
        hasCam = true;
    else
        hasCam = false;

    if(!hasCam)
    {
        ui->btnInitCam->setEnabled(false);
        ui->btnCapture->setEnabled(false);
    }

    ui->btnCrop->setEnabled(false);

    QList<QCameraInfo> tempCam = QCameraInfo::availableCameras();
    for(int i = 0; i < tempCam.length(); i++)
        cameras.prepend(tempCam.at(i));

    for(int i = 0; i < cameras.length(); i++)
        ui->cbCamList->addItem(cameras.at(i).deviceName());

    on_cbCamList_currentIndexChanged(0);

    initGeometry();

    ui->cbAge->setCurrentText("21");
    ui->cbHeight->setCurrentText("184");
    ui->cbWeight->setCurrentText("72.5");

    ui->cbYear->setCurrentText(QDateTime::currentDateTime().toString("yyyy"));
    ui->cbMonth->setCurrentText(QDateTime::currentDateTime().toString("MM"));
    ui->cbDay->setCurrentText(QDateTime::currentDateTime().toString("dd"));

    memberID = -1;
}

addmemberdialog::~addmemberdialog()
{
    delete ui;
    delete viewFinder;
    delete camera;
    delete imageCapture;
    delete cropBand;
}

void addmemberdialog::initGeometry()
{
    QRect labelRect[9];
    QRect inputRect[9];
    int wWidth = this->width(), wHeight = this->height();

    QRect imageLabelRect(wWidth - 280, 10, 270, (270)*4/3);
    ui->imageLabel->setGeometry(imageLabelRect);

    QRect camInput[5];
    for(int i = 0; i < 5; i++)
    {
        camInput[i].setRect(wWidth - 280, (270)*4/3 + 30 + i * 40, 270, 30);
    }

    ui->btnBrowse->setGeometry(camInput[0]);
    ui->btnCrop->setGeometry(camInput[1]);
    ui->cbCamList->setGeometry(camInput[2]);
    ui->btnInitCam->setGeometry(camInput[3]);
    ui->btnCapture->setGeometry(camInput[4]);

    int labelBase = 10, inputBase = 30;
    for(int i = 0; i < 9; i++)
    {
        labelRect[i].setRect(10, labelBase + i*60, 290, 17);
        inputRect[i].setRect(10, inputBase + i*60, 300, 30);
    }

    QRect btnAddRect(10, inputBase - 20 + 9*60, 300, 30);
    ui->btnAdd->setGeometry(btnAddRect);

    ui->lFirstname->setGeometry(labelRect[0]);
    ui->tbFirstname->setGeometry(inputRect[0]);

    ui->lLastname->setGeometry(labelRect[1]);
    ui->tbLastname->setGeometry(inputRect[1]);

    ui->lAge->setGeometry(labelRect[2]);
    ui->cbAge->setGeometry(inputRect[2]);

    inputRect[2].setWidth(inputRect[2].width() - 5);
    labelRect[2].setWidth(labelRect[2].width() - 5);
    ui->lSex->setGeometry( QRect(labelRect[2].x(),
                             labelRect[2].y(),
                             labelRect[2].width()/2,
                             labelRect[2].height()) );
    ui->cbSex->setGeometry( QRect(inputRect[2].x(),
                             inputRect[2].y(),
                             inputRect[2].width()/2,
                             inputRect[2].height()) );

    ui->lAge->setGeometry( QRect(labelRect[2].x() + labelRect[2].width()/2 + 5,
                             labelRect[2].y(),
                             labelRect[2].width()/2,
                             labelRect[2].height()) );
    ui->cbAge->setGeometry( QRect(inputRect[2].x() + inputRect[2].width()/2 + 5,
                             inputRect[2].y(),
                             inputRect[2].width()/2,
                             inputRect[2].height()) );

    ui->lHeight->setGeometry(labelRect[3]);
    ui->cbHeight->setGeometry(inputRect[3]);

    ui->lWeight->setGeometry(labelRect[4]);
    ui->cbWeight->setGeometry(inputRect[4]);

    ui->lSport->setGeometry(labelRect[5]);
    ui->tbSport->setGeometry(inputRect[5]);

    ui->lIllness->setGeometry(labelRect[6]);
    ui->tbIllness->setGeometry(inputRect[6]);

    ui->lRFID->setGeometry(labelRect[7]);
    ui->tbRFID->setGeometry(inputRect[7]);

    ui->lExpire->setGeometry(labelRect[8]);

    inputRect[8].setWidth(inputRect[8].width() - 10);
    ui->cbYear->setGeometry( QRect(inputRect[8].x(),
                             inputRect[8].y(),
                             inputRect[8].width()/2,
                             inputRect[8].height()) );

    ui->cbMonth->setGeometry( QRect(inputRect[8].x() + inputRect[8].width()/2 + 5,
                             inputRect[8].y(),
                             inputRect[8].width()/4,
                             inputRect[8].height()) );

    ui->cbDay->setGeometry( QRect(inputRect[8].x() + inputRect[8].width()*0.75 + 10,
                             inputRect[8].y(),
                             inputRect[8].width()/4,
                             inputRect[8].height()) );

    ui->cbAge->clear();
    for(int i = 1; i <= 121; i++)
        ui->cbAge->addItem(QString::number(i));

    ui->cbHeight->clear();
    for(int i = 100; i <= 260; i++)
        ui->cbHeight->addItem(QString::number(i));

    ui->cbWeight->clear();
    for(double i = 10; i <= 150; i += 0.5)
        ui->cbWeight->addItem(QString::number(i));

    ui->cbYear->clear();
    for(int i = 2015; i <= 2099; i++)
        ui->cbYear->addItem(QString::number(i));

    ui->cbMonth->clear();
    for(int i = 1; i <= 12; i++)
        ui->cbMonth->addItem((i < 10 ?"0":"")+QString::number(i));

    ui->cbDay->clear();
    for(int i = 1; i <= 31; i++)
        ui->cbDay->addItem((i < 10 ?"0":"")+QString::number(i));

    ui->imageLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);
}

void addmemberdialog::mousePressEvent(QMouseEvent *event)
{
    if(ui->imageLabel->underMouse())
    {
        ui->btnCrop->setEnabled(true);
        startPoint = event->pos();
        cropBand->setGeometry(QRect(startPoint,startPoint));
        cropBand->show();
        startCheck = true;
    }
}

void addmemberdialog::mouseMoveEvent(QMouseEvent *event)
{
    if(startCheck)
    {
        QPoint tempPoint, tempPoint2;
        tempPoint = event->pos();
        tempPoint2 = ui->imageLabel->mapFromParent(tempPoint);
        if(tempPoint2.x()<0)
            tempPoint.setX(ui->imageLabel->geometry().x());
        if(tempPoint2.y()<0)
            tempPoint.setY(ui->imageLabel->geometry().y());
        if(tempPoint2.x() > ui->imageLabel->geometry().width())
            tempPoint.setX(ui->imageLabel->geometry().x()+ui->imageLabel->geometry().width());
        if(tempPoint2.y() > ui->imageLabel->geometry().height())
            tempPoint.setY(ui->imageLabel->geometry().y()+ui->imageLabel->geometry().height());

        cropBand->setGeometry(QRect(startPoint,tempPoint).normalized());
    }
}

void addmemberdialog::mouseReleaseEvent(QMouseEvent *event)
{
    if(startCheck)
    {
        QPoint tempPoint;
        tempPoint = event->pos();
        if(tempPoint.x()<ui->imageLabel->geometry().x())
            tempPoint.setX(ui->imageLabel->geometry().x());
        if(tempPoint.y()<ui->imageLabel->geometry().y())
            tempPoint.setY(ui->imageLabel->geometry().y());
        if(tempPoint.x() > ui->imageLabel->geometry().x()+ui->imageLabel->geometry().width())
            tempPoint.setX(ui->imageLabel->geometry().x()+ui->imageLabel->geometry().width());
        if(tempPoint.y() > ui->imageLabel->geometry().y()+ui->imageLabel->geometry().height())
            tempPoint.setY(ui->imageLabel->geometry().y()+ui->imageLabel->geometry().height());
        endPoint = tempPoint;
        startCheck = false;
    }
}

void addmemberdialog::setRFID(QString RFID)
{
    RFIDStr = RFID;
    ui->tbRFID->setText(RFIDStr);
}

void addmemberdialog::setMemberID(int _ID)
{
    memberID = _ID;
}

void addmemberdialog::cameraImageReady(int i, QImage tImage)
{
    currentImage = tImage;
    loadImageIntoLabel(currentImage);
    //on shutter button released
    camera->unlock();
}

void addmemberdialog::loadImageIntoLabel(QImage tImage)
{
    ui->imageLabel->setPixmap(QPixmap::fromImage(tImage).scaled( ui->imageLabel->width(),
                                                                 ui->imageLabel->height(),
                                                                 Qt::KeepAspectRatio));
}

void addmemberdialog::loadMemberData(int id)
{
    QSqlQuery tempQuery("SELECT * FROM memberlist WHERE id = "+QString::number(id)+" ORDER BY id ASC");
    loadFromQuery(tempQuery);
}

void addmemberdialog::loadMemberData(QString _RFID)
{
    QSqlQuery tempQuery("SELECT * FROM memberlist WHERE RFID = '"+_RFID+"' ORDER BY id ASC");
    loadFromQuery(tempQuery);
}

void addmemberdialog::loadFromQuery(QSqlQuery &tQuery)
{
    if(tQuery.next())
    {
        setMemberID( tQuery.value(0).toInt() );

        ui->tbFirstname->setText(tQuery.value(2).toString());
        ui->tbLastname->setText(tQuery.value(3).toString());
        if(tQuery.value(4).toString().length() > 2)
        {
            currentImage.load(QDir::currentPath()+tQuery.value(4).toString());
            loadImageIntoLabel(currentImage);
        }
        ui->cbAge->setCurrentText(tQuery.value(5).toString());
        ui->cbHeight->setCurrentText(tQuery.value(6).toString());
        ui->cbWeight->setCurrentText(tQuery.value(7).toString());
        ui->tbSport->setText(tQuery.value(8).toString());
        ui->tbIllness->setText(tQuery.value(9).toString());
        ui->cbYear->setCurrentText(tQuery.value(10).toString().split("-").at(0));
        ui->cbMonth->setCurrentText(tQuery.value(10).toString().split("-").at(1));
        ui->cbDay->setCurrentText(tQuery.value(10).toString().split("-").at(2));
        ui->cbSex->setCurrentIndex((tQuery.value(13).toString() == "f"? 0 : 1));

        setRFID( tQuery.value(1).toString() );
    }
    ui->btnAdd->setText("Apply");
}

void addmemberdialog::on_btnInitCam_clicked()
{
    QRect tempRect = QApplication::desktop()->screenGeometry();
    viewFinder->resize(400, 300);
    viewFinder->move(tempRect.width()/2 - 200, tempRect.height()/2 - 150);
    viewFinder->show();

    imageCapture = new QCameraImageCapture(camera);

    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(cameraImageReady(int,QImage)));
    camera->setCaptureMode(QCamera::CaptureStillImage);
    camera->start();
}

void addmemberdialog::on_cbCamList_currentIndexChanged(int index)
{
    if(cameras.length() > 0 && cameras.length() > index)
    {
        camera = new QCamera(cameras.at(index));
        viewFinder = new QCameraViewfinder();
        camera->setViewfinder(viewFinder);
    }
}

void addmemberdialog::on_btnBrowse_clicked()
{
    QSettings settings(settingAddress, QSettings::IniFormat);
    QString tempFileName = QFileDialog::getOpenFileName(this,
                        tr("Open Image"),
                        settings.value(DEFAULT_DIR_KEY).toString(),
                        tr("Image Files (*.png *.jpg *.bmp)"));
    if(!tempFileName.isEmpty())
    {
        currentImage.load(tempFileName);
        loadImageIntoLabel(currentImage);
        QDir CurrentDir;
        settings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(tempFileName));
    }
}

void addmemberdialog::on_btnCrop_clicked()
{
    if(startPoint == QPoint(0, 0) && endPoint == QPoint(0,0))
        return;
    QRect imageRect = currentImage.rect();
    QRect lableRect = ui->imageLabel->rect();
    QRect cropRect;
    double ratio;
    if(double(imageRect.width())/imageRect.height() > double(lableRect.height())/lableRect.height())
        ratio = double(imageRect.width())/lableRect.width();
    else
        ratio = double(imageRect.height())/lableRect.height();

    cropRect.setTopLeft((ui->imageLabel->mapFromParent(startPoint))*ratio);
    cropRect.setBottomRight((ui->imageLabel->mapFromParent(endPoint))*ratio);
    cropRect = cropRect.normalized();

    QImage tempImage;
    tempImage = currentImage.copy(cropRect);
    loadImageIntoLabel(tempImage);

    startCheck = false;
    startPoint = endPoint = QPoint(0,0);
    cropBand->hide();
    ui->btnCrop->setEnabled(false);

    bool check = QMessageBox::question(this, "Crop Image",
                                              "Are you sure?",
                                                "No",
                                                "Yes");
    if(check)
        currentImage = tempImage;
    else
        loadImageIntoLabel(currentImage);

}

void addmemberdialog::on_btnCapture_pressed()
{
    //on half pressed shutter button
    camera->searchAndLock();
}

void addmemberdialog::on_btnCapture_released()
{
    //on shutter button pressed
    imageCapture->capture();

}

void addmemberdialog::on_btnAdd_clicked()
{
    if(ui->tbFirstname->text().length() < 2 ||
       ui->tbLastname->text().length() < 2 ||
       ui->tbRFID->text().length() != 10 )
    {
        QMessageBox::critical(this,"Alert",
                              "Fulfill the required fields");
        return;
    }

    int tempID = 10000;

    if(memberID == -1)
    {
        QSqlQuery squery("SELECT id FROM memberlist WHERE state = 1 OR state = 0 ORDER BY id DESC");
        if(squery.next())
        {
            tempID = squery.value(0).toInt();
            tempID++;
        }
    }
    else
        tempID = memberID;

    QString tempImageDir = "";
    if(currentImage.height() > 5)
    {
        tempImageDir = "/data/image/"+QString::number(tempID)+"p.png";
        currentImage.save(QDir::currentPath()+tempImageDir);
    }

    QString tempStr;

    QString tempDate = QDateTime::currentDateTime().toString("dd.MM.yyyy h:m:s ap");

    QString tempExpireDate = ui->cbYear->currentText()+"-"+
            ""+ui->cbMonth->currentText()+"-"+ui->cbDay->currentText();

    if(memberID == -1)
    {
        tempStr = "INSERT INTO `memberlist`(`id`, `RFID`, `firstname`, `lastname`, "\
              "`pic_dir`, `m_age`, `m_height`, `m_weight`, `m_sport`, `m_illness`, "\
              "`expire_date`, `date`, `state`, `m_sex`) "\
              "VALUES ("+QString::number(tempID)+", '"+ui->tbRFID->text()+"', "\
              "'"+ui->tbFirstname->text()+"', '"+ui->tbLastname->text()+"', "\
              "'"+tempImageDir+"', "+ui->cbAge->currentText()+", "+ui->cbHeight->currentText()+", "\
              "'"+ui->cbWeight->currentText()+"', '"+ui->tbSport->text()+"', "\
              "'"+ui->tbIllness->text()+"', '"+tempExpireDate+"', '"+tempDate+"', 1, "\
              "'"+(ui->cbSex->currentIndex() == 0? "f":"m")+"');";
    }
    else
    {
        tempStr = "UPDATE memberlist SET RFID = '"+ui->tbRFID->text()+"', firstname = '"+ui->tbFirstname->text()+"', pic_dir = '"+tempImageDir+"', "\
              "lastname = '"+ui->tbLastname->text()+"', m_age = "+ui->cbAge->currentText()+", "\
              "m_height = "+ui->cbHeight->currentText()+", m_weight = '"+ui->cbWeight->currentText()+"', "\
              "m_sport = '"+ui->tbSport->text()+"', m_illness = '"+ui->tbIllness->text()+"', "\
              "expire_date = '"+tempExpireDate+"', m_sex = '"+(ui->cbSex->currentIndex() == 0? "f":"m")+"' WHERE id = "+QString::number(tempID)+" ;";

    }


    QSqlQuery tempQuery;
    bool tempResult = tempQuery.exec(tempStr);
    //qDebug()<<query
    if(memberID == -1)
    {
        if(tempResult)
        {
            emit sqlChanged();
            this->close();
        }
        else
            QMessageBox::critical(0, "Alert",
                    "Problem in adding member"
                                , "OK");
    }
    else
    {
        if(tempResult)
            QMessageBox::information(0, "",
                    "Successfully modified"
                                , "OK");
        else
            QMessageBox::critical(0, "Alert",
                    "Successfully added"
                                , "OK");
        emit sqlChanged();
    }


}

void addmemberdialog::closeEvent(QCloseEvent *)
{
    try
    {
       viewFinder->close();
    }
    catch(int e)
    {
        qDebug()<<e;
    }

}
