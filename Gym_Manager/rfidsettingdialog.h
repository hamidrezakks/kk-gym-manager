#ifndef RFIDSETTINGDIALOG_H
#define RFIDSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class rfidsettingdialog;
}

class rfidsettingdialog : public QDialog
{
    Q_OBJECT

public:
    explicit rfidsettingdialog(QWidget *parent = 0);
    ~rfidsettingdialog();

    void loadAvailablePorts();
    void loadSettings();
signals:
    void RFIDSettingChangd();
private slots:
    void on_pushButton_clicked();

private:
    Ui::rfidsettingdialog *ui;

    QList<QSerialPortInfo> availablePorts;
};

#endif // RFIDSETTINGDIALOG_H
