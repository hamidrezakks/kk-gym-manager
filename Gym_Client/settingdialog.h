#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class settingdialog;
}

class settingdialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingdialog(QWidget *parent = 0);
    ~settingdialog();

    void loadAvailablePorts();
    void loadSettings();

signals:
    void settingsChanged();
    void RFIDChanged();
private slots:
    void on_btnApply_clicked();

private:
    Ui::settingdialog *ui;

    QList<QSerialPortInfo> availablePorts;
};

#endif // SETTINGDIALOG_H
