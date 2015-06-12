#include "rfidsettingdialog.h"
#include "ui_rfidsettingdialog.h"

#include "mainwindow.h"

rfidsettingdialog::rfidsettingdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rfidsettingdialog)
{
    ui->setupUi(this);


}

void rfidsettingdialog::loadAvailablePorts()
{
    availablePorts.clear();
    availablePorts = QSerialPortInfo::availablePorts();

    ui->cbPorts->clear();
    for(int i = 0; i < availablePorts.length(); i++)
    {
        ui->cbPorts->addItem(availablePorts.at(i).portName());
    }

}

void rfidsettingdialog::loadSettings()
{
    QSettings settings(settingAddress, QSettings::IniFormat);

    ui->cbPorts->setCurrentText(settings.value(RFID_PORT_KEY).toString());
    ui->cbDataLength->setCurrentText(settings.value(RFID_DATA_LENGTH_KEY).toString());
}

rfidsettingdialog::~rfidsettingdialog()
{
    delete ui;
}

void rfidsettingdialog::on_pushButton_clicked()
{
    QSettings settings(settingAddress, QSettings::IniFormat);

    settings.setValue(RFID_PORT_KEY, ui->cbPorts->currentText());
    settings.setValue(RFID_DATA_LENGTH_KEY, ui->cbDataLength->currentText());

    if(ui->cbPorts->currentText().length() > 2)
        emit RFIDSettingChangd();

    this->close();
}
