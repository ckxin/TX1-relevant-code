
#ifndef POINTER_H_
#define POINTER_H_
#include "opencv2/core.hpp"



class pointer {
public:
	pointer();
	~pointer ();

	void  Dilation_Erosion(cv:: Mat &img, cv::Mat &outimg);// 图片预处理的腐蚀与膨胀处理
    void light(cv:: Mat &img, cv::Mat &outimg);//图片预处理的亮度与对比度
    float run0(cv::Mat&img);
    cv::Mat g_dstImage;//原始图和效果图
    cv::Mat img, resultImage;



    int g_nContrast, g_nBright;
    int g_nTrackbarNumer = 1;//0表示腐蚀erode, 1表示膨胀dilate
    int g_nStructElementSize = 1; //结构元素(内核矩阵)的尺寸
    const int g_nContrastMax = 300, g_nBright_Max = 300;


private:


};



#endif /* METER_H_ */
