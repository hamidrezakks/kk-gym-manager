#ifndef NETSETTINGDIALOG_H
#define NETSETTINGDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class netsettingdialog;
}

class netsettingdialog : public QDialog
{
    Q_OBJECT

public:
    explicit netsettingdialog(QWidget *parent = 0);
    ~netsettingdialog();

    void loadSettings();

signals:
    void netSettingChanged();

private slots:
    void on_btnApply_clicked();

private:
    Ui::netsettingdialog *ui;
};

#endif // NETSETTINGDIALOG_H
