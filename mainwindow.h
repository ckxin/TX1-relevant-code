#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <workthread.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QLabel *TimeLabel;
    QLabel *CameraLabel;
    QLabel *ImgLabel;
    QLabel *DataLabel;
    QLabel *DataLabel1;

    QPushButton *CameraBtn;
    QPushButton *GrabBtn;
    QPushButton *ShutdownBtn;
    QPushButton *RebootBtn;

    QHBoxLayout *TopLayout;
    QHBoxLayout *CameraLayout;
    QHBoxLayout *ImgLayout;
    QHBoxLayout *MiddleLayout1;
    QHBoxLayout *MiddleLayout2;
    QHBoxLayout *BottomLayout;
    WorkThread *workThread;

/*public slots:
    void timerUpdate(void);*/

public slots:
    void timerUpdate(void);
    void slotCamera();
    void slotGrab();
    void slotVideo();
    //void slotImgshow();
private slots:
    //void on_CamreaBtn_clicked();
    //void on_GrabButton_clicked();
    void on_ShutdownBtn_clicked();
    void on_RebootBtn_clicked();




};

#endif // MAINWINDOW_H
