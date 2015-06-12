#ifndef notifdialog_H
#define notifdialog_H

#include <QDialog>

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
private:
    Ui::notifdialog *ui;
};

#endif // notifdialog_H
