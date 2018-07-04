/*
 * angle.cpp
 *
 *  Created on: 2018-6-26
 *      Author: xsd
 */
/*
 * angle.cpp
 *
 *  Created on: 2018-6-26
 *      Author: xsd
 */
#include "angle.h"
#include<vector>
#include<math.h>
using namespace cv;
using namespace std;
angle::angle()       //默认值的设置
{
	rotate_angle = 1;
	 flag=0;
	 hor_angle = 0;
	 hor_angleend = 0;
}
angle::~angle()
{
}
Mat angle::nearestNeighRotate(cv::Mat img, float angle)
{
	int len;
	float row1= img.rows;
	float col1=  img.cols;
	float pow1= pow(row1, 2) ;
	float pow2=pow(col1, 2);
	len = (int)(sqrt(pow1+pow2) + 0.5);
	Mat retMat = Mat::zeros(len, len, CV_8UC3);
	float anglePI = angle * CV_PI / 180;
	int xSm, ySm;

	for (int i = 0; i < retMat.rows; i++)
		for (int j = 0; j < retMat.cols; j++)
		{
			xSm = (int)((i - retMat.rows / 2)*cos(anglePI) - (j - retMat.cols / 2)*sin(anglePI) + 0.5);
			ySm = (int)((i - retMat.rows / 2)*sin(anglePI) + (j - retMat.cols / 2)*cos(anglePI) + 0.5);
			xSm += img.rows / 2;
			ySm += img.cols / 2;

			if (xSm >= img.rows || ySm >= img.cols || xSm <= 0 || ySm <= 0) {
				retMat.at<Vec3b>(i, j) = Vec3b(0, 0);
			}
			else {
				retMat.at<Vec3b>(i, j) = img.at<Vec3b>(xSm, ySm);
			}
		}

	return retMat;
}
Mat angle::convertTo3Channels(const Mat& binImg)    //颜色空间转换函数 从单通道的灰度图转换到3通道的RGB图
{
	Mat three_channel = Mat::zeros(binImg.rows, binImg.cols, CV_8UC3);
	vector<Mat> channels;
	for (int i = 0; i<3; i++)
	{
		channels.push_back(binImg);   //算法中里面的一个函数名，如c++中的vector头文件里面就有这个push_back函数，在vector类中作用为在vector尾部加入一个数据。
		//string中也有这个函数，作用是字符串之后插入一个字符。
	}
	merge(channels, three_channel);
	//cout <<"3通道图"<< three_channel << endl;
	return three_channel;
}
Mat angle::Horizontal_angle(const Mat& inpic)//对输入的指针的图片进行顺时针旋转，然后计算出指针与水平方向的夹角
{

	vector<Mat> rotated_pic; //每个角度旋转后的图片储存
	vector<Mat> rowsum;
	Mat diagr;
	rotated_pic.push_back(inpic.clone());
	int count = 0;
	for (int i = 1; (i*rotate_angle <= 180); i++) //得到若干张旋转后的图片矩阵
	{
		rotated_pic.push_back((nearestNeighRotate(rotated_pic[0], i*rotate_angle)).clone());
		count++;
	}
	Mat out;
	for (int j = 0; j < count; j++) //对矩阵求行和，并且求出每个矩阵的最大行和；（就是拿图片的灰度值向Y轴投影）
	{
		reduce(rotated_pic[j], out, 1, CV_REDUCE_SUM, CV_32S);//求每个矩阵的行和，并储存
		rowsum.push_back(out.clone());  //clone 是深拷贝，否则是浅拷贝，就是数据贡献，并没有数据真正储存在rowsum中
	}
	double minv = 0.0, maxv = 0.0;
	double MAX = 0;
	double max;
	int maxi;
	double* minp = &minv;
	double* maxp = &maxv;
	minMaxIdx(rowsum[0], minp, maxp);
	//cout << "原图的行和最大值" << maxv << endl;
	for (int i = 0; i < count; i++)
	{
		minMaxIdx(rowsum[i], minp, maxp);
		max = maxv;
		if (MAX < max)
		{
			MAX = max;
			maxi = i;
		}
	}
	//cout << "最大的行和" << MAX << endl;
	hor_angle = maxi*rotate_angle;
	//cout << "旋转到水平的角度：" << maxi*rotate_angle << endl;
	//imshow("水平状态的指针", rotated_pic[maxi]);
	Mat colssum;
	reduce(rotated_pic[maxi], colssum, 0, CV_REDUCE_SUM, CV_32S); //求水平状态图的列和，判断指针水平时的状态：大头在哪边。 参数0表示变成一行
	//cout << "colssum:" << colssum.t() << endl;            //colsssum为3通道的矩阵
	double minu = 0.0, maxu = 0.0;
	double* maxx = &maxu;
	double* minn = &minu;
	minMaxIdx(colssum, minn, maxx);
	vector<int> arry;
	vector<int> Partpointer;
	for (int i = 0; i < 3 * colssum.cols; i += 3)
	{
		arry.push_back(colssum.at<int>(0, i));

	}
	int counta = 0;
	for (int k = 0; k < colssum.cols; k++)
	{

		if((k>80)&&(k<140))
		{Partpointer.push_back(arry[k]);
		counta++;
		}
		if((k>270)&&(k<403))
		{Partpointer.push_back(arry[k]);
		counta++;

		}

	}

		return rotated_pic[maxi];

}
int angle::pointer_out(const Mat& inbi)
{
	int w = 0;
	int hangb = 0;
	int hange = 0;
	for (int i = 0; i < inbi.rows; i++)
	{
		for (int j = 0; j < inbi.cols; j++)
		{
			if (inbi.at<uchar>(i, j) != 0)
			{
				hangb = i; //开始的行
				break;
			}
		}
		if (hangb != 0)
		{
			break;
		}
	}
	//cout << "hangb:" << hangb << endl;
	int u = 0;
	for (int i = inbi.rows-1; i>0; i--)
	{
		for (int j = 0; j < inbi.cols; j++)
		{
			if (inbi.at<uchar>(i, j) != 0)
			{
				hange = i; //开始的行
				break;
			}
		}
		if (hange != 0)
		{
			break;
		}
	}
	//cout << "hange:" << hange << endl;
	Mat pointerauto = inbi(Range(hangb-3, hange+3), Range(0, inbi.cols));//感兴趣区域提取
	//imshow("pointerauto", pointerauto);
	Mat touying;
	reduce(pointerauto, touying, 0, CV_REDUCE_SUM, CV_32S);
	//cout << "touying" << touying << endl;
	double mint = 0.0, maxt = 0.0;
	double* maxtt = &maxt;
	double* mintt = &mint;
	double MAXX = 0;
	double zuida=0;
	int maxj;
	for (int i = 0; i < touying.cols; i++)
	{
		minMaxIdx(touying, mintt, maxtt);
		zuida = maxt;
		if (touying.at<int>(0,i)==zuida)   //colssum.at<int>(0, i)
		{
			MAXX = zuida;
			maxj = i;
		}
	}
	//cout << "maxj:" << maxj << endl;
	//cout << "水平投影的最大值：" << MAXX << endl;
	if (maxj > touying.cols / 2) //大头在右面  则指针与水平方向夹角大于180度
	{
		 flag = 1;
	}
	else
	{
		flag = 0;
	}
	//cout << "flag:" << flag << endl;
	hor_angleend = hor_angle + 180 * flag;
	cout << "最终的角度:" << hor_angleend << endl;
	return  hor_angleend;
}
float angle::Fitting_line_270(const int angle)
{
	double y[] = {0.0700, 0.0600, 0.0500, 0.0400,0.0200,0.0100,0.0000,-0.0100,-0.0200,-0.0300,-0.0400,-0.0500,-0.0600,-0.0700,-0.0800,-0.0900};
	double x[] = {0.00000, 11.8566, 23.5294, 35.9775, 59.6250,76.5500, 90.0000,100.8000,114.3000,132.9000,146.7813,161.5759,173.0137,189.1152,204.0273,222,3632 };

	//double x[] = { 1, 2, 3, 4, 5, 6 };
	//double y[] = { 3, 5.5, 6.8, 8.8, 11, 12 };
	valarray<double> data_x(x, 15);
	valarray<double> data_y(y, 15);

	float A = 0.0;
	float B = 0.0;
	float C = 0.0;
	float D = 0.0;
	A = (data_x*data_x).sum();
	B = data_x.sum();
	C = (data_x*data_y).sum();
	D = data_y.sum();
	float tmp = A*data_x.size() - B*B;
	float k, b;
	k = (C*data_x.size() - B*D) / tmp;
	b = (A*D - C*B) / tmp;
	//cout << "y=" << k << "x+" << b << endl;
	cout <<angle<<"度的读数为：" <<angle * k + b << endl;
	float jiao= angle * k + b ;
	return  jiao ;

}
float angle::Fitting_line_360(const int angle)
{
	double y[] = { 0.1000,0.0900,0.0800,0.0700 };
	double x[] = { 319.6500,332.6183,346.1367,360.0000 };
	valarray<double> data_x(x, 4);
	valarray<double> data_y(y, 4);

	float A = 0.0;
	float B = 0.0;
	float C = 0.0;
	float D = 0.0;
	A = (data_x*data_x).sum();
	B = data_x.sum();
	C = (data_x*data_y).sum();
	D = data_y.sum();
	float tmp = A*data_x.size() - B*B;
	float k, b;
	k = (C*data_x.size() - B*D) / tmp;
	b = (A*D - C*B) / tmp;
	//cout << "y=" << k << "x+" << b << endl;
	cout << "320度的读数为：" << angle * k + b << endl;
	float jiao= angle * k + b ;
	return  jiao ;
}






