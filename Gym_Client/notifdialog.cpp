#include "notifdialog.h"
#include "ui_notifdialog.h"

notifdialog::notifdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::notifdialog)
{
    ui->setupUi(this);
    ui->label_6->setStyleSheet("QLabel {color: red; font-weight: bold;}");
    ui->label_7->setStyleSheet("QLabel {color: red; font-weight: bold;}");
}

notifdialog::~notifdialog()
{
    delete ui;
}

void notifdialog::setExist(bool tCheck)
{
    if(tCheck)
    {
        ui->imageLabel->setVisible(true);
        ui->label->setVisible(true);
        ui->label_2->setVisible(true);
        ui->label_3->setVisible(true);
        ui->label_4->setVisible(true);
        ui->label_5->setVisible(true);
        ui->label_8->setVisible(true);
        ui->label_6->setVisible(false);
        ui->label_7->setVisible(false);

        ui->pushButton->setVisible(true);
    }
    else
    {
        ui->imageLabel->setVisible(false);
        ui->label->setVisible(false);
        ui->label_2->setVisible(false);
        ui->label_3->setVisible(false);
        ui->label_4->setVisible(false);
        ui->label_5->setVisible(false);
        ui->label_8->setVisible(false);
        ui->label_6->setVisible(true);
        ui->label_7->setVisible(true);

        ui->pushButton->setVisible(false);
    }
}

void notifdialog::loadMember(kkMember &tMember)
{
    ui->label->setText("Name: "+tMember.firstname);
    ui->label_2->setText("Lastname: "+tMember.lastname);
    ui->label_3->setText("Past week points: "+QString::number(tMember.totalPoint));
    ui->label_4->setText("Today point: "+QString::number(tMember.todayPoint));
    ui->label_5->setText("Global rank: "+QString::number(tMember.rank));
    ui->label_8->setText("Calorie to burn: "+QString::number(tMember.eCal));
    loadImageIntoLabel(tMember.image);
}

void notifdialog::on_pushButton_clicked()
{
    emit userApplied();
    this->close();
}

void notifdialog::loadImageIntoLabel(QImage tImage)
{
    ui->imageLabel->setPixmap(QPixmap::fromImage(tImage).scaled( ui->imageLabel->width(),
                                                                 ui->imageLabel->height(),
                                                                 Qt::KeepAspectRatio));
}
