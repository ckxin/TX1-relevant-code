/*
 * angle.h
 *
 *  Created on: 2018-6-26
 *      Author: xsd
 */

#ifndef ANGLE_H_
#define ANGLE_H_
/*
 * angle.h
 *
 *  Created on: 2018-6-26
 *      Author: xsd
 */

#include <opencv2/opencv.hpp>
#include<math.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <valarray>

using namespace std;

using namespace cv;
class angle
{
public:
	float hor_angle;
	float hor_angleend;
	int flag;
	angle();
	~angle();
	Mat nearestNeighRotate(cv::Mat img, float angle);
	Mat convertTo3Channels(const Mat& binImg);
	Mat Horizontal_angle(const Mat& inpic);
	int pointer_out(const Mat& inbi);
	float Fitting_line_270(const int angle);
	float Fitting_line_360(const int angle);
  private:
	float rotate_angle;

};











#endif /* ANGLE_H_ */
