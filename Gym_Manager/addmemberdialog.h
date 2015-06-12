#ifndef ADDMEMBERDIALOG_H
#define ADDMEMBERDIALOG_H

#include <QDialog>
#include <QtMultimedia>
#include <QMessageBox>
#include <QtMultimediaWidgets>
#include <QDebug>
#include <QSettings>
#include <QRubberBand>
#include <QDateTime>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

namespace Ui {
class addmemberdialog;
}

class addmemberdialog : public QDialog
{
    Q_OBJECT

public:
    explicit addmemberdialog(QWidget *parent = 0);
    ~addmemberdialog();

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void closeEvent(QCloseEvent *);

    void setRFID(QString RFID);

    void setMemberID(int _ID);

    void loadMemberData(int id);
    void loadMemberData(QString _RFID);

signals:
    void sqlChanged();

private slots:
    void cameraImageReady(int i, QImage tImage);

    void on_btnInitCam_clicked();

    void on_cbCamList_currentIndexChanged(int index);

    void on_btnBrowse_clicked();

    void on_btnCrop_clicked();

    void on_btnCapture_pressed();

    void on_btnCapture_released();

    void on_btnAdd_clicked();

private:
    Ui::addmemberdialog *ui;
    bool hasCam;

    QList<QCameraInfo> cameras;
    QCameraViewfinder *viewFinder;
    QCamera *camera;
    QCameraImageCapture *imageCapture;

    QImage currentImage;

    QString RFIDStr;

    int memberID;

    //
    bool startCheck;
    QPoint startPoint;
    QPoint endPoint;
    QRubberBand *cropBand;
    //
    void initGeometry();

    void loadImageIntoLabel(const QImage tImage);

    void loadFromQuery(QSqlQuery &tQuery);

};

#endif // ADDMEMBERDIALOG_H
