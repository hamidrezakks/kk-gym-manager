#include "caldialog.h"
#include "ui_caldialog.h"

caldialog::caldialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::caldialog)
{
    ui->setupUi(this);
}

caldialog::~caldialog()
{
    delete ui;
}

void caldialog::clearAll()
{
    ui->lineEdit->clear();
}

void caldialog::on_pushButton_clicked()
{
    if(ui->lineEdit->text().toInt() > 0)
        emit calEntered(ui->lineEdit->text().toDouble());
    this->close();
}
