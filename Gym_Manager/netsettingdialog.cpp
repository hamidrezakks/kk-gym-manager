#include "netsettingdialog.h"
#include "ui_netsettingdialog.h"

#include "mainwindow.h"

netsettingdialog::netsettingdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::netsettingdialog)
{
    ui->setupUi(this);
}

netsettingdialog::~netsettingdialog()
{
    delete ui;
}

void netsettingdialog::loadSettings()
{
    QSettings settings(settingAddress, QSettings::IniFormat);

    ui->tbIPC1->setText(settings.value(CLIENT1_IP_KEY).toString());
    ui->tbPORTC1->setText(settings.value(CLIENT1_PORT_KEY).toString());

    ui->tbIPC2->setText(settings.value(CLIENT2_IP_KEY).toString());
    ui->tbPORTC2->setText(settings.value(CLIENT2_PORT_KEY).toString());

    ui->tbIPC3->setText(settings.value(CLIENT3_IP_KEY).toString());
    ui->tbPORTC3->setText(settings.value(CLIENT3_PORT_KEY).toString());
}

void netsettingdialog::on_btnApply_clicked()
{
    QSettings settings(settingAddress, QSettings::IniFormat);

    settings.setValue(CLIENT1_IP_KEY, ui->tbIPC1->text());
    settings.setValue(CLIENT1_PORT_KEY, ui->tbPORTC1->text());

    settings.setValue(CLIENT2_IP_KEY, ui->tbIPC2->text());
    settings.setValue(CLIENT2_PORT_KEY, ui->tbPORTC2->text());

    settings.setValue(CLIENT3_IP_KEY, ui->tbIPC3->text());
    settings.setValue(CLIENT3_PORT_KEY, ui->tbPORTC3->text());

    emit netSettingChanged();

    this->close();
}
