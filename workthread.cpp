#include "workthread.h"
#include <QtDebug>
#include "mainwindow.h"
//#include "ui_mainwindow.h"

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <gloable.h>
#include <Meter.hpp>



using namespace std;
using namespace cv;
using namespace bcv;

WorkThread::WorkThread()
{

}

void WorkThread::run()

{
    VideoCapture cap("rtsp://admin:12345678xin@192.168.1.64/h264/ch1/main/av_stream");


        for(;;)
        {
            bool OK = cap.grab();

                if (OK == false)
               {
                cout << "cannot grab" << endl;
               }
                else
                {

                    writingImg = true;
                    cap >> image;




                    writingImg = false;
                    emit show("show");
                    //emit pause(1);
                }
        }

}
