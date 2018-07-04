#include "mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QWidget>
#include <QSizePolicy>
#include <iostream>
#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QTimer>
#include <qdatetime.h>
#include <workthread.h>
#include <unistd.h>
#include <gloable.h>
#include <opencv2/opencv.hpp>
#include <QPixmap>
#include <Meter.hpp>


using namespace std;
using namespace cv;
using namespace bcv;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)

{
    QDesktopWidget *desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    int g_nActScreenW = screenRect.width();
    int g_nActScreenH = screenRect.height();

    cout <<"width="<<g_nActScreenW<<endl;
    cout <<"height="<<g_nActScreenH<<endl;


    TimeLabel = new QLabel;
    int h0=g_nActScreenH*0.035;
    int w0=g_nActScreenW*0.98;

    TimeLabel->setFixedHeight(h0);
    TimeLabel->setFixedWidth(w0);
    TimeLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    //TimeLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    TimeLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    TimeLabel->setAlignment(Qt::AlignRight);

    TopLayout = new QHBoxLayout;
    TopLayout->addWidget(TimeLabel);
    int M_w0=g_nActScreenW*0.005;
    //int M_h0=g_nActScreenH*0.01;
    int M_h0=g_nActScreenH*0.02125;
    TopLayout->setContentsMargins(M_w0,M_h0,M_w0,M_h0);



    CameraLabel = new QLabel;
    CameraLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);

    int w1=g_nActScreenW*0.48;
    int h1=(w1/16)*9;
    CameraLabel->setFixedHeight(h1);
    CameraLabel->setFixedWidth(w1);

    CameraLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    CameraLabel->setScaledContents(true);
    //int wid=CameraLabel->width();
    /*CameraLayout = new QHBoxLayout;
    CameraLayout->addWidget(CameraLabel);

    int M_w1=M_w0*0.5;
    int M_h1=M_h0*0.5;
    CameraLayout->setContentsMargins(M_w1,M_h1,M_w1,M_h1);*/
    //CameraLayout->setMargin(3);

    ImgLabel = new QLabel;
    ImgLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);

    //int w2=g_nActScreenW*0.48;
    //int h2=(w2/16)*9;
    ImgLabel->setFixedHeight(h1);
    ImgLabel->setFixedWidth(w1);
    ImgLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ImgLabel->setScaledContents(true);
    /*ImgLayout = new QHBoxLayout;
    ImgLayout->addWidget(ImgLabel);
    ImgLayout->setContentsMargins(M_w1,M_h1,M_w1,M_h1);*/
    //ImgLayout->setMargin(3);

    MiddleLayout1 = new QHBoxLayout;

    //MiddleLayout1->addLayout(ImgLayout);
    //MiddleLayout1->addLayout(CameraLayout);

    MiddleLayout1->addWidget(ImgLabel);
    MiddleLayout1->addWidget(CameraLabel);

    MiddleLayout1->setContentsMargins(M_w0,M_h0,M_w0,M_h0);
    int Space=g_nActScreenW*0.02;
    MiddleLayout1->setSpacing(Space);
    //MiddleLayout1->setMargin(5);
    //MiddleLayout1->setSpacing(5);



    DataLabel = new QLabel;
    DataLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    int w2=g_nActScreenW*0.48;
    int h2=g_nActScreenH*0.035;
    DataLabel->setFixedHeight(h2);
    DataLabel->setFixedWidth(w2);
    DataLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    //DataLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    DataLabel1 = new QLabel;
    DataLabel1->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    int w3=g_nActScreenW*0.48;
    int h3=g_nActScreenH*0.035;
    DataLabel1->setFixedHeight(h3);
    DataLabel1->setFixedWidth(w3);
    //DataLabel1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    DataLabel1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    MiddleLayout2 = new QHBoxLayout;
    MiddleLayout2->addWidget(DataLabel);
    MiddleLayout2->addWidget(DataLabel1);
    MiddleLayout2->setContentsMargins(M_w0,M_h0,M_w0,M_h0);
    int Space1=g_nActScreenW*0.02;
    MiddleLayout2->setSpacing(Space1);



    int w4=g_nActScreenW*0.2;
    int h4=g_nActScreenH*0.2;
    CameraBtn = new QPushButton("Camera");
    CameraBtn->setFixedHeight(h4);
    CameraBtn->setFixedWidth(w4);
    //CameraBtn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    CameraBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    GrabBtn = new QPushButton("Grab");
    GrabBtn->setFixedHeight(h4);
    GrabBtn->setFixedWidth(w4);
    //GrabBtn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    GrabBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    ShutdownBtn = new QPushButton("Shutdown");
    ShutdownBtn->setFixedHeight(h4);
    ShutdownBtn->setFixedWidth(w4);
    //ShutdownBtn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ShutdownBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    RebootBtn = new QPushButton("Reboot");
    RebootBtn->setFixedHeight(h4);
    RebootBtn->setFixedWidth(w4);
    //RebootBtn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    RebootBtn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    BottomLayout= new QHBoxLayout;
    BottomLayout->addWidget(CameraBtn);
    BottomLayout->addWidget(GrabBtn);
    BottomLayout->addWidget(ShutdownBtn);
    BottomLayout->addWidget(RebootBtn);
    BottomLayout->setContentsMargins(M_w0,M_h0,M_w0,M_h0);
    int Space2=g_nActScreenW*0.06;
    BottomLayout->setSpacing(Space2);


    QWidget *centerWindow = new QWidget;
    this->setCentralWidget(centerWindow);
    //centerWindow->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    centerWindow->setLayout(mainLayout);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(TopLayout);
    mainLayout->addLayout(MiddleLayout1);
    mainLayout->addLayout(MiddleLayout2);
    mainLayout->addLayout(BottomLayout);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    /*centerWindow->addLayout(TopLayout);
    centerWindow->addLayout(MiddleLayout1);
    centerWindow->addLayout(MiddleLayout2);
    centerWindow->addLayout(BottomLayout);*/

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    timer->start(1000);
    connect(CameraBtn,SIGNAL(clicked()),this,SLOT(slotCamera()));
    connect(GrabBtn,SIGNAL(clicked()),this,SLOT(slotGrab()));
    connect(ShutdownBtn,SIGNAL(clicked()),this,SLOT(on_ShutdownBtn_clicked()));
    connect(RebootBtn,SIGNAL(clicked()),this,SLOT(on_RebootBtn_clicked()));




}

MainWindow::~MainWindow()
{

}


void MainWindow::timerUpdate(void)
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    TimeLabel->setText(str);
}

/*void MainWindow::CameraBtn_clicked()
{
    slotCamera();

}

void MainWindow::GrabBtn_clicked()
{
    slotGrab();

}*/

void MainWindow::on_ShutdownBtn_clicked()
{
    system("shutdown -h now");

}

void MainWindow::on_RebootBtn_clicked()
{
    system("shutdown -r -t 0");

}

void MainWindow::slotCamera()
{

        workThread=new WorkThread();
        connect(workThread,SIGNAL(show(const QString)),this,SLOT(slotVideo()));

        workThread ->start();

        CameraBtn ->setEnabled(false);
    //ui ->stopBtn ->setEnabled(true);

}

void MainWindow::slotVideo()
{
    //Meter m;
    //Mat rslt;
    Mat Rgb;
    QImage Img;
    //m.run(image,rslt);
    cv::cvtColor(image, Rgb, CV_BGR2RGB);//颜色空间转换
    Img = QImage((const uchar*)(Rgb.data), Rgb.cols, Rgb.rows, Rgb.cols * Rgb.channels(), QImage::Format_RGB888);

    CameraLabel->setPixmap(QPixmap::fromImage(Img));
    CameraLabel->show();

}

void MainWindow::slotGrab()
{
    //Mat rslt;
    Meter m;
    Mat rslt;
        if (writingImg){
            for(;;){
                if(!writingImg){
                    break;
                }
            }
        }
        cout << image.cols << "x" << image.rows << endl;
        m.run(image,rslt);
        imwrite("/home/xin/Qt/frame.jpg",rslt);
        //imwrite("/home/xin/Qt/frame.jpg",image);
        QPixmap pixmap("/home/xin/Qt/frame.jpg");
        ImgLabel->setPixmap(pixmap);
        ImgLabel->show();
        QString str="12345";
        DataLabel->setText(str);
        //w1=pixmap.size();
}


