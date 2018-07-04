/*
 * Meter.h
 *
 *  Created on: Jun 26, 2018
 *      Author: bcli
 */

#ifndef METER_H_
#define METER_H_

#include "opencv2/core.hpp"

namespace bcv {


class Meter {
public:
	Meter (int maxWidth = 1920, int maxHeight = 1080, int it = 400, int offset = 5, int div = 4, int thres = 0);
	~Meter ();

	void run(cv::Mat& src, cv::Mat& rslt);
	bool find(cv::Mat& src);

	int centerX();
	int centerY();
	int radius();

	friend std::ostream& operator<<(std::ostream& os, const Meter& cd);

private:
	bool guided(cv::Mat& src, cv::Mat& rslt, int r=7, float eps=0.04, float s=0.3);
	void findPositivePoints(cv::Mat& src, const int thres);
	void estimateCircles();
	bool estimateCircle(const unsigned short& crId,
						const unsigned short& ptId1,
						const unsigned short& ptId2,
						const unsigned short& ptId3);
	unsigned short voteCircles();
	unsigned short voteCircle(const unsigned short& crId);

	unsigned short* x;
	unsigned short* y;
	unsigned short* a;
	unsigned short* b;
	unsigned short* r;
	unsigned short* v;

	unsigned short maxPts;
	unsigned short maxCrs;

	unsigned short ptCnt;
	unsigned short crCnt;

	unsigned short maxRadius;
	unsigned short maxVotedCrId;

	char offset;
	char thres;
	char div;

	int center_x;
	int center_y;
	int circle_radius;
};

} /* namespace bcv */

#endif /* METER_H_ */
