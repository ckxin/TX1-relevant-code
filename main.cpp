#include "mainwindow.h"
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <iostream>
#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <gloable.h>
#include <opencv2/opencv.hpp>

using namespace std;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.showFullScreen();


    return a.exec();
}
