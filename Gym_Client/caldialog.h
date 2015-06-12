#ifndef CALDIALOG_H
#define CALDIALOG_H

#include <QDialog>

namespace Ui {
class caldialog;
}

class caldialog : public QDialog
{
    Q_OBJECT

public:
    explicit caldialog(QWidget *parent = 0);
    ~caldialog();

    void clearAll();

signals:
    void calEntered(double tCal);

private slots:
    void on_pushButton_clicked();

private:
    Ui::caldialog *ui;
};

#endif // CALDIALOG_H
