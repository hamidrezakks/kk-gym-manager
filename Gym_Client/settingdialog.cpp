#include "settingdialog.h"
#include "ui_settingdialog.h"

#include "mainwindow.h"

settingdialog::settingdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingdialog)
{
    ui->setupUi(this);
}

settingdialog::~settingdialog()
{
    delete ui;
}

void settingdialog::loadAvailablePorts()
{
    availablePorts.clear();
    availablePorts = QSerialPortInfo::availablePorts();

    ui->cbPorts->clear();
    for(int i = 0; i < availablePorts.length(); i++)
    {
        ui->cbPorts->addItem(availablePorts.at(i).portName());
    }

}

void settingdialog::loadSettings()
{
    QSettings settings(settingAddress, QSettings::IniFormat);

    ui->tbServerIP->setText(settings.value(SERVER_IP_KEY).toString());
    ui->tbServerPort->setText(settings.value(SERVER_PORT_KEY).toString());

    ui->cbPorts->setCurrentText(settings.value(RFID_PORT_KEY).toString());
    ui->cbDataLength->setCurrentText(settings.value(RFID_DATA_LENGTH_KEY).toString());

    ui->cbMode->setCurrentIndex(settings.value(CLIENT_MODE_KEY).toInt());
}

void settingdialog::on_btnApply_clicked()
{
    QSettings settings(settingAddress, QSettings::IniFormat);

    settings.setValue(SERVER_IP_KEY, ui->tbServerIP->text());
    settings.setValue(SERVER_PORT_KEY, ui->tbServerPort->text());

    settings.setValue(RFID_PORT_KEY, ui->cbPorts->currentText());
    settings.setValue(RFID_DATA_LENGTH_KEY, ui->cbDataLength->currentText());

    settings.setValue(CLIENT_MODE_KEY, ui->cbMode->currentIndex());

    emit settingsChanged();

    if(ui->cbPorts->currentText().length() > 2)
        emit RFIDChanged();

    this->close();
}
