#ifndef RFIDDIALOG_H
#define RFIDDIALOG_H

#include <QDialog>

namespace Ui {
class rfiddialog;
}

class rfiddialog : public QDialog
{
    Q_OBJECT

public:
    explicit rfiddialog(QWidget *parent = 0);
    ~rfiddialog();

    void resetAll();
signals:
    void manualRFIDReady(QString str);
private slots:
    void on_pushButton_clicked();

private:
    Ui::rfiddialog *ui;
};

#endif // RFIDDIALOG_H
