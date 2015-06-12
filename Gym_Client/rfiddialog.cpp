#include "rfiddialog.h"
#include "ui_rfiddialog.h"

rfiddialog::rfiddialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rfiddialog)
{
    ui->setupUi(this);
}

rfiddialog::~rfiddialog()
{
    delete ui;
}

void rfiddialog::on_pushButton_clicked()
{
    QString tempStr;
    tempStr = ui->lineEdit->text();
    emit manualRFIDReady(tempStr);
    this->close();
}

void rfiddialog::resetAll()
{
    ui->lineEdit->setText("");
}
