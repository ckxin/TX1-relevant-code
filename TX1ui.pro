#-------------------------------------------------
#
# Project created by QtCreator 2018-06-15T14:23:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TX1ui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    workthread.cpp \
    gloable.cpp \
    Meter.cpp

HEADERS += \
        mainwindow.h \
    workthread.h \
    gloable.h \
    Meter.hpp







 INCLUDEPATH += /usr/include/opencv/
 INCLUDEPATH += /usr/include/opencv2/

 LIBS +=    -L/usr/lib/    \
    -lopencv_core  \
    -lopencv_imgproc   \
    -lopencv_ml \
    -lopencv_photo  \
    -lopencv_tegra  \
    -lopencv_video  \
    -lopencv_vstab  \
    -lopencv_legacy \
    -lopencv_core   \
    -lopencv_flann  \
    -lopencv_calib3d    \
    -lopencv_contrib    \
    -lopencv_gpu    \
    -lopencv_detection_based_tracker    \
    -lopencv_highgui    \
    -lopencv_imgproc    \
    -lopencv_imuvstab   \
    -lopencv_superres   \
    -lopencv_objdetect  \
    -lopencv_facedetect \
    -lopencv_features2d \
    -lopencv_esm_panorama   \
    -lopencv_stitching  \
    -lopencv_videostab  \
    -lopencv_highgui

