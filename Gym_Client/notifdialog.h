#ifndef NOTIFDIALOG_H
#define NOTIFDIALOG_H

#include <QDialog>
#include <QRect>

struct kkMember {
    int id;
    QString RFID;
    QImage image;
    QString firstname;
    QString lastname;
    double todayPoint;
    double totalPoint;
    double eCal;
    int rank;
};

namespace Ui {
class notifdialog;
}

class notifdialog : public QDialog
{
    Q_OBJECT

public:
    explicit notifdialog(QWidget *parent = 0);
    ~notifdialog();

    void setExist(bool tCheck);
    void loadMember(kkMember &tMember);
signals:
    void userApplied();
private slots:
    void on_pushButton_clicked();

private:
    Ui::notifdialog *ui;
    void loadImageIntoLabel(QImage tImage);
};

#endif // NOTIFDIALOG_H
