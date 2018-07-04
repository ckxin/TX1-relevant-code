#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv/cv.hpp>
#include"PreGraph.h"
#include<stdlib.h>
#include"angle.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"pointer.hpp"


using namespace cv;
using namespace std;
pointer ::pointer()
{

}
pointer::~pointer()
{

}
float  pointer ::run0(cv::Mat&img)
{


	Mat  dst;
	int resize_height =200;
	int resize_width=200;
	resize(img,img,Size(resize_height,resize_width),(0,0),(0,0),(INTER_LINEAR));

	light(img, dst);     //对比度与亮度调整函数   //dst指针类型的输入所以形参的改变会影响到输入的实参，所以此函数可以写成无返回值的函数
	Dilation_Erosion(dst, g_dstImage);  //对图像进行腐蚀膨胀操作
	Mat nor;
	normalize(g_dstImage, nor, 255.0, 0.0, NORM_MINMAX);
	nor.convertTo(nor, CV_8UC3, 1.0);


	PreGraph SpMat;                            //一定要实例化一个对象出来 才行 否则会提醒：：：：：非静态成员引用必须与特定对象相对
	Mat superpixels = SpMat.GeneSp(nor);      //产生超像素分割图 ，并且返回超像素分割的Mat标签矩阵
	Mat sal = SpMat.GenCenterSal(nor);             //产生中心先验显著性值 ，是归一化后的矩阵  sal是一维数组形式
												
	Mat salMap = SpMat.Sal2Img(superpixels, sal);   //产生显著性图！！！！//imshow("123", salMap);  //黑白明暗对比度不高，需要转换颜色空间

	Mat tmpsuperpixels;                          //归一化后的矩阵：0—255
	normalize(salMap, tmpsuperpixels, 255.0, 0.0, NORM_MINMAX);  // NORM_MINMAX=32 数组的数值被平移或缩放到一个指定的范围，线性归一化。
																 //cout << tmpsuperpixels << endl;   // 归一化就是要把需要处理的数据经过处理后（通过某种算法）限制在你需要的一定范围内。
																 //产生后是float型的数据，不可直接保存为图片

	tmpsuperpixels.convertTo(tmpsuperpixels, CV_8UC3, 1.0);      //对归一化矩阵图像转换到CV_8UC3格式     想要将标记图像显示出来必须转换其数据类型。、、//主要是变成整型
																 //convertTo函数是颜色数据的转换，但是不会改变原图像的通道数，所以后面即使用的类型为CV_8UC3，输出的也是单通道的。 想改变通道数要用cvtColor~~~~
		Mat dst1;
	//cout << "The return value of opencv threshold is: " << threshold(tmpsuperpixels, dst1, 60, 255, THRESH_BINARY)<<endl;  //CV_THRESH_OTSU   THRESH_BINARY
	int  yuzhi=threshold(tmpsuperpixels, dst1, 0, 255, CV_THRESH_OTSU) ;

	//**********************************************************************实现二值化后的图像对应的超像素标签*********************************************
	//cvtColor(dst, dst, CV_GRAY2BGR);
	Mat SortSal = SpMat.GeneSortSal(nor, dst1);
	Mat SortsalMap = SpMat.Sal2Img(superpixels, SortSal);
	Mat Sorttmpsuperpixels;
	normalize(SortsalMap, Sorttmpsuperpixels, 255.0, 0.0, NORM_MINMAX);
	Sorttmpsuperpixels.convertTo(Sorttmpsuperpixels, CV_8UC3, 1.0);
	Mat dst2;
	threshold(Sorttmpsuperpixels, dst2, 0, 255, CV_THRESH_OTSU);
	angle ang;    //一定要实例化一个对象出来 才行 否则会提醒：：：：：非静态成员引用必须与特定对象相对
	Mat sort3 = ang.convertTo3Channels(Sorttmpsuperpixels);
	Mat retImg = ang.nearestNeighRotate(sort3, 180); //输入三通道 输出为16 ,类型为8UC3

	//*****************************************************************************************************************************************************

	Mat sort4;

    Mat get=ang.Horizontal_angle(sort3);



	Mat dstout,grayget;
	cvtColor(get, grayget, CV_BGR2GRAY);
	threshold(grayget, dstout, 0, 255, CV_THRESH_OTSU);
	int jiaodu=ang.pointer_out(dstout);
	float picjiao;
	if (jiaodu < 270)
	{
		picjiao=ang.Fitting_line_270(jiaodu);
	}
	else
	{
		 picjiao=ang.Fitting_line_360(jiaodu);
	}


    return picjiao;
}


//**********************************************************************************************************************************************
//*******************************************************************************改进的亮度与对比度的自适应调整函数*****************************
void pointer::light(cv:: Mat &img, cv::Mat &outimg)
{

	int width = img.cols;        //列
	int height = img.rows;       //行
	int  Cx = width / 2;         //图像中心x坐标
	int  Cy = height / 2;        //图像中心Y坐标
	int  Center = sqrt(Cx*Cx + Cy*Cy);
	outimg = Mat::zeros(img.size(), img.type());
	float alpha = 4;         //对比度参数
	float beta = 100;          //亮度参数
	float alpha0;
	float beta0;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			if (img.channels() == 3)
			{
				float pow1=   pow(float(row - Cx), 2) ;
				float pow2=   pow(float(col - Cy), 2);
				alpha0 = (float)sqrt(pow1+ pow2)/Center;   //基于中心先验的对比度调整，越靠近中心对比度越高
				alpha = (1 - alpha0) ;
				float pow3=   pow(float(row - Cx), 2) ;
				float pow4=   pow(float(col - Cy), 2);
				beta0 = (float)sqrt(pow3+pow4)/Center;    //越边缘时亮度增加越大
				beta = beta0 * 250;
				float b = img.at<Vec3b>(row, col)[0];//blue
				float g = img.at<Vec3b>(row, col)[1];//green
				float r = img.at<Vec3b>(row, col)[2];//red

													 //output
				outimg.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(b*alpha + beta); //saturate_cast函数防止数据溢出，应该是根据前面的数据类型的取值范围来规定越界范围
				outimg.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(g*alpha + beta);
				outimg.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(r*alpha + beta);
			}
			else if (img.channels() == 1)
			{
				float gray = img.at<uchar>(row, col);
				outimg.at<uchar>(row, col) = saturate_cast<uchar>(gray*alpha + beta);
			}
		}
	}

	//char output_title[] = "对比度与亮度调整";
	//imshow(output_title, outimg);
}

void pointer::Dilation_Erosion(cv:: Mat &img, cv:: Mat &outimg)
{
	
	Mat out;
	Mat element = getStructuringElement(MORPH_RECT, Size(2 * g_nStructElementSize + 1, 2 * g_nStructElementSize + 1), Point(g_nStructElementSize, g_nStructElementSize));
	dilate(img, out, element);
	
	erode(out, outimg, element);

}



//end



