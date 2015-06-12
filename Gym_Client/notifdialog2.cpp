#include "notifdialog2.h"
#include "ui_notifdialog.h"

notifdialog::notifdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::notifdialog)
{
    ui->setupUi(this);

    ui->label_6->setVisible(false);
    ui->label_6->setStyleSheet("QLabel {color: red; font-weight: bold;}");
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
        ui->label_6->setVisible(false);
        ui->label->setVisible(false);
    }
    else
    {
        ui->imageLabel->setVisible(false);
        ui->label->setVisible(false);
        ui->label_2->setVisible(false);
        ui->label_3->setVisible(false);
        ui->label_4->setVisible(false);
        ui->label_5->setVisible(false);
        ui->label_6->setVisible(true);
        ui->label_7->setVisible(true);
    }
}
