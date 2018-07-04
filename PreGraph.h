#if !defined(_PREGRAPH_H_INCLUDE_)
#define _PREGRAPH_H_INCLUDE_
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include "SLIC.h"
using namespace cv;
class PreGraph
{
public:
	PreGraph();
	~PreGraph();
	Mat GeneSal(const Mat &img);
	Mat Sal2Img(const Mat &superpixels, const Mat &Sal);
	Mat GeneSp(const Mat &img);
	int GeneFeature(const Mat &img, const Mat &superpixels, const int feaType, Mat &feaSpL, Mat &feaSpA, Mat &feaSpB, Mat &spNpix, Mat &spCnt);
	Mat GenCenterSal(const Mat &img); //分文件类外定义
	Mat GeneSortSal(const Mat &img, const Mat &bi);
private:
	int spNumMax;
	double compactness;
	float alpha;
	float theta;
	int spNum;
	float U;//无向图排序优化参数
	Mat GeneAdjMat(const Mat &spmat);
	//int GeneFeature(const Mat &img, const Mat &superpixels, const int feaType, Mat &feaSpL, Mat &feaSpA, Mat &feaSpB, Mat &spNpix, Mat &spCnt);
	Mat GeneWeight(const vector<float> &feaSpL, const vector<float> &feaSpA, const vector<float> &feaSpB, const Mat &superpixels, const vector<int> &bd, const Mat &adj);
	Mat GeneSortWeight(const vector<float> &feaSpL, const vector<float> &feaSpA, const vector<float> &feaSpB, const Mat &superpixels, const Mat &adj);
	vector<int> GeneBdQuery(const Mat &superpixels, const int type);
	Mat GeneY(const vector<int> &bd);
	Mat inveMat(const Mat &weight, const Mat &Y);
	//Mat GeneFeature(const Mat &img);
};

#endif






