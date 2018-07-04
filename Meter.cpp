/*
 * Meter.cpp
 *
 *  Created on: Jun 26, 2018
 *      Author: bcli
 */

#include "Meter.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

namespace bcv {

Meter::Meter (int maxWidth, int maxHeight, int it, int offset, int div, int thres){

	if (it <= 0){
		cout << "Meter::Meter() 'it' has been reset to 400" << endl;
		it = 400;
	}

	if (thres < 0 || thres > 255){
		cout << "Meter::Meter() 'thres' has been reset to 0" << endl;
		thres = 0;
	}

	if (offset < 0){
		cout << "CircleDtector::Meter() 'offset' has been reset to 5" << endl;
		offset = 5;
	}

	maxPts = (maxWidth*maxHeight)/div;
	maxCrs = it;

	x = new unsigned short[maxPts];
	y = new unsigned short[maxPts];
	a = new unsigned short[maxCrs];
	b = new unsigned short[maxCrs];
	r = new unsigned short[maxCrs];
	v = new unsigned short[maxCrs];

	ptCnt = 0;
	crCnt = 0;

	maxRadius = maxWidth/2;
	maxVotedCrId = 0;

	center_x = 0;
	center_y = 0;
	circle_radius = 0;

	this->thres = thres;
	this->offset = offset;
	this->div = div;
}

Meter::~Meter () {

	delete [] x;
	delete [] y;
	delete [] a;
	delete [] b;
	delete [] r;
	delete [] v;
}

void Meter::run(Mat& src, Mat& rslt){
	Mat gdd,can;
	guided(src,gdd);
	Canny(gdd,can,30,60);
	find(can);
	rslt = src.clone();
	circle(rslt,Point(centerX(),centerY()),radius(),Scalar(0,255,255),5);
}

bool Meter::find(Mat& src){
	if(src.type() != CV_8UC1){
		cout << "Meter::find() requires a single channel binarized image (CV_8UC1)" << endl;
		return false;
	}
	findPositivePoints(src, thres);
	estimateCircles();
	voteCircles();
	center_x = a[maxVotedCrId];
	center_y = b[maxVotedCrId];
	circle_radius = r[maxVotedCrId];
	return true;
}

int Meter::centerX(){
	return center_x;
}

int Meter::centerY(){
	return center_y;
}

int Meter::radius(){
	return circle_radius;
}

void Meter::findPositivePoints(Mat& src, int thres){
	int cols = src.rows;
	int rows = src.cols;
	ptCnt = 0;
	for(int j=0;j<cols;j++){
		uchar* data = src.ptr<uchar>(j);
		for (int i=0;i<rows;i++){
			if (data[i] > thres){
					if (ptCnt >= maxPts) {break;}
					x[ptCnt] = i;
					y[ptCnt] = j;
					ptCnt++;
			}
		}
	}
}

void Meter::estimateCircles(){
	srand(time(NULL));		// random seed
	unsigned short max = ptCnt - 1;
	unsigned short rand1,rand2,rand3;
	crCnt = 0;

	for(unsigned short i = 0; i < maxCrs; i++){
		rand1 = rand()%max +1;
		rand2 = rand()%max +1;
		rand3 = rand()%max +1;
		if (estimateCircle(i,rand1,rand2,rand3)) crCnt++;
	}
}

bool Meter::estimateCircle(const unsigned short& crId,
									const unsigned short& ptId1,
									const unsigned short& ptId2,
									const unsigned short& ptId3){
	int x1 = x[ptId1];
	int y1 = y[ptId1];
	int x2 = x[ptId2];
	int y2 = y[ptId2];
	int x3 = x[ptId3];
	int y3 = y[ptId3];
	int a1,a2,a3;
	double a,b,r;
	a1 = x1*x1*y3 + x2*x2*y1 + x2*x2*y2 + x3*x3*y2 + y1*y2*y2 + y1*y1*y3 + y2*y3*y3;
	a2 = x1*x1*y2 + x2*x2*y2 + x2*x2*y3 + x3*x3*y1 + y1*y1*y2 + y1*y3*y3 + y2*y2*y3;
	a3 = 2*((x1-x2)*(y3-y2) + (x2-x3)*(y1-y2));
	if (a3 == 0){
		this->a[crId] = 0;
		this->b[crId] = 0;
		this->r[crId] = 0;
		return false;
	}
	a = (a1-a2)/a3;
	b = (x1*x1 - x2*x2 + y1*y1 - y2*y2 - 2*a*(x1-x2))/(2*(y1-y2));
	r = sqrt((x2-a)*(x2-a) + (y2-b)*(y2-b));
	this->a[crId] = round(a);
	this->b[crId] = round(b);
	this->r[crId] = round(r);
	return true;
}

unsigned short Meter::voteCircles (){
	int vote = 0;
	int maxVote = 0;


	for(unsigned short i = 0; i < maxCrs; i++){
		if (r[i] > maxRadius){
			v[i] = 0;
			continue;
		}
		v[i] = voteCircle(i);
		if (v[i] > maxVote){
			maxVote = v[i];
			maxVotedCrId = i;
		}
	}
	return maxVotedCrId;
}

unsigned short Meter::voteCircle(const unsigned short& crId){
	unsigned short a = this->a[crId];
	unsigned short b = this->b[crId];
	unsigned short r = this->r[crId];
	unsigned short x,y,dis,vote = 0;

	for(unsigned short i = 0; i < ptCnt; i++){
		x = this->x[i];
		y = this->y[i];
		dis = round(sqrt((a-x)*(a-x) + (b-y)*(b-y)));
		if (dis > (r-offset) && dis < (r+offset)){
			vote++;
		}
	}
	return vote;
}

ostream& operator<<(ostream& os, const Meter& cd){
	os << "it:" << cd.maxCrs << ", off:" << (int)cd.offset << ", div:" << (int)cd.div << ", thres:" << (int)cd.thres
	   <<", pts:" << cd.ptCnt << "/" << cd.maxPts << ", crs:" << cd.crCnt
	   << ", cr:(" << cd.center_x << "," << cd.center_y << ") " << cd.circle_radius << ", vt:" << cd.v[cd.maxVotedCrId]
	   << ", id:" << cd.maxVotedCrId;
	return os;
}

bool Meter::guided(Mat& src, Mat& rslt, int r, float eps, float s){
	if(src.type() != CV_8UC3){
		cout << "Filter::find() requires 3 channel BGR image (CV_8UC3)" << endl;
		return false;
	}
	Mat I = src.clone();
	Mat p = src.clone();
	cvtColor(I,I,CV_BGR2GRAY);
	cvtColor(p,p,CV_BGR2GRAY);
	I.convertTo(I,CV_64FC1,1.0/255);
	p.convertTo(p,CV_64FC1,1.0/255);

	int rows = I.rows;
	int cols = I.cols;

	Mat small_I, small_p;

	Size smallSize(int(round(s*cols)), int(round(s*rows)));

	resize(I,small_I,smallSize,0,0,CV_INTER_CUBIC);
	resize(p,small_p,smallSize,0,0,CV_INTER_CUBIC);

	int small_r = int(round(r*s));
	Size winSize(2*small_r + 1, 2*small_r + 1);

	Mat mean_small_I, mean_small_p;
	boxFilter(small_I, mean_small_I, CV_64FC1,winSize);
	boxFilter(small_p, mean_small_p, CV_64FC1,winSize);

	Mat small_Ip = small_I.mul(small_p);
	Mat mean_small_Ip;
	boxFilter(small_Ip, mean_small_Ip, CV_64FC1,winSize);

	Mat cov_small_Ip = mean_small_Ip - mean_small_I.mul(mean_small_p);

	Mat mean_small_II;
	boxFilter(small_I.mul(small_I), mean_small_II, CV_64FC1,winSize);

	Mat var_small_I = mean_small_II - mean_small_I.mul(mean_small_I);
	Mat small_a = cov_small_Ip/(var_small_I + eps);
	Mat small_b = mean_small_p - small_a.mul(mean_small_I);

	Mat mean_small_a, mean_small_b;
	boxFilter(small_a, mean_small_a, CV_64FC1, winSize);
	boxFilter(small_b, mean_small_b, CV_64FC1, winSize);

	Mat mean_a, mean_b;
	resize(mean_small_a, mean_a, I.size(), 0,0,CV_INTER_LINEAR);
	resize(mean_small_b, mean_b, I.size(), 0,0, CV_INTER_LINEAR);

	Mat q = mean_a.mul(I) + mean_b;
	q.convertTo(rslt,CV_8UC3,255);
	//Mat I_enhanced = (I-q)*5 + q;
	//normalize(I_enhanced, I_enhanced,1,0,NORM_MINMAX, CV_32FC1);

	//I_enhanced.convertTo(rslt,CV_8UC3,255);
	return true;
}


} /* namespace bcv */
