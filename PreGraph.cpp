/*
 * PreGraph.cpp
 *
 *  Created on: 2018-6-26
 *      Author: xsd
 */
/*
 * PreGraph.cpp
 *
 *  Created on: 2018-6-26
 *      Author: xsd
 */
/*************************************************

Copyright: Guangyu Zhong all rights reserved

Author: Guangyu Zhong

Date:2014-09-27

Description: codes for Manifold Ranking Saliency Detection//                                     对于流行排序的代码
Reference http://ice.dlut.edu.cn/lu/Project/CVPR13[yangchuan]/cvprsaliency.htm

**************************************************/
#include "PreGraph.h"
#include "SLIC.h"
//#include<cv.h>
//#include<highgui.h>
using namespace std;
typedef unsigned int UINT;

PreGraph::PreGraph()       //默认值的设置
{
	spNumMax = 200;
	compactness = 10.0;   //紧致性   欧氏距离与颜色空间距离的权重值 //值越大超像素分割后的超像素块的形状越相对规则
	alpha = 0.99f;         //平衡算法的平滑约束与拟合约束，设置为0.99即可
	theta = 0.1f;         // 相似性矩阵计算时的参数德尔塔的平方
	spNum = 0;            //最终产生的超像素个数
	U = 0.5;             //优化参数u
}
PreGraph::~PreGraph()
{
}
Mat PreGraph::GeneSp(const Mat &img)      //产生超像素图，并返回超像素标签图（Mat格式）

{
	int width = img.cols;    //列
	int height = img.rows;   //行
	int sz = width*height;
	UINT *reimg = new UINT[sz * 3];
	for (int c = 0; c<3; c++)                //Mat格式转到SLIC格式
	{
		for (int i = 0; i<width; i++)
		{
			for (int j = 0; j<height; j++)

				reimg[c*(width*height) + i*height + j] = saturate_cast<unsigned int>(img.at<Vec3b>(j, i)[2 - c]);
		}
	}
	int* label = NULL;     //此处做了与windos不一样的 修改

	SLIC slic;
	slic.DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(reimg, height, width, label, spNum, spNumMax, compactness);
	Mat superpixels(img.size(), CV_16U);

	for (int i = 0; i<superpixels.rows; i++)             //将SLIC格式的超像素图转换为Mat格式，便于后续基于超像素的显著性图的计算
	{                                                    //将label（是一个一维的数组）的数据复制给superpixels矩阵（mat格式）
		for (int j = 0; j<superpixels.cols; j++)
		{
			superpixels.at<ushort>(i, j) = label[i + j*superpixels.rows];
		}
	}
	delete[] reimg;
	delete[] label;
	return superpixels;
}
//********************************************************************************************************
//判断各个超像素块是否相邻，也就是邻接，标记为1，否则为0
//********************************************************************************************************
Mat PreGraph::GeneAdjMat(const Mat &spmat) //输入超像素图的标签矩阵 ：每个像素属于哪一块超像素的标签值
{
	int width = spmat.cols;   //列数===图像宽度
	int height = spmat.rows; //行数 ===图像高度
	Mat adjmat(spNum,spNum, CV_16U, Scalar(0));
	//cout << "spmat:" << "  "<<spmat << endl;
	if (spmat.empty()) {
		cout << "矩阵为空！" << endl;
	}
	for (int i = 0; i < height - 1; ++i)
	{
		for (int j = 0; j < width - 1; ++j)
		{
			if (spmat.at<ushort>(i, j) != spmat.at<ushort>(i + 1, j))   //如果此点数据不等于右面相邻像素的标签值，两个像素不属于同一超像素块
			{
				adjmat.at<ushort>(spmat.at<ushort>(i, j), spmat.at<ushort>(i + 1, j)) = 1;  // 说明是在超像素块的分割边界。。说明邻接邻接值为1
				adjmat.at<ushort>(spmat.at<ushort>(i + 1, j), spmat.at<ushort>(i, j)) = 1;   //对称性
			}
			if (spmat.at<ushort>(i, j) != spmat.at<ushort>(i, j + 1))           //与上面类似上下关系 ，下面以此类推
			{
				adjmat.at<ushort>(spmat.at<ushort>(i, j), spmat.at<ushort>(i, j + 1)) = 1;
				adjmat.at<ushort>(spmat.at<ushort>(i, j + 1), spmat.at<ushort>(i, j)) = 1;
			}
			if (spmat.at<ushort>(i, j) != spmat.at<ushort>(i + 1, j + 1))
			{
				adjmat.at<ushort>(spmat.at<ushort>(i, j), spmat.at<ushort>(i + 1, j + 1)) = 1;
				adjmat.at<ushort>(spmat.at<ushort>(i + 1, j + 1), spmat.at<ushort>(i, j)) = 1;
			}
			if (spmat.at<ushort>(i + 1, j) != spmat.at<ushort>(i, j + 1))
			{
				adjmat.at<ushort>(spmat.at<ushort>(i + 1, j), spmat.at<ushort>(i, j + 1)) = 1;
				adjmat.at<ushort>(spmat.at<ushort>(i, j + 1), spmat.at<ushort>(i + 1, j)) = 1;
			}
		}
	}
	return adjmat;
}
vector<int> PreGraph::GeneBdQuery(const Mat &superpixels, const int type)    //type=5    Bd查询？？？ Bd= Background ???????????????
{                                                                           //环境背景的四条边的边界查询节点，然后进行排序并去除重复的值
	int height = superpixels.rows;
	int width = superpixels.cols;
	Mat transSuper = superpixels.t();  //产生转置矩阵！！！！！
	vector<int> bd;
	switch (5)
	{
	case 1:
		bd = superpixels.row(0);
		break;
	case 2:
		bd = superpixels.row(height - 1);
		break;
	case 3:
		bd = transSuper.row(0);
		break;
	case 4:
		bd = transSuper.row(width - 1);
		break;
	case 5:
		vector<int> bdTop = superpixels.row(0);                //提取第1行
		vector<int> bdDown = superpixels.row(height - 1);      //提取最后一行
		vector<int> bdRight = transSuper.row(0);               //提取原矩阵的第1列元素
		vector<int> bdLeft = transSuper.row(width - 1);        //提取原矩阵的最后一列元素
		bd.insert(bd.end(), bdTop.begin(), bdTop.end());       //把图像的边缘都存进bd数组中？？？？？？？？
		bd.insert(bd.end(), bdDown.begin(), bdDown.end());
		bd.insert(bd.end(), bdRight.begin(), bdRight.end());
		bd.insert(bd.end(), bdLeft.begin(), bdLeft.end());
		break;
	}
	sort(bd.begin(), bd.end());                    //升序排序
	bd.erase(unique(bd.begin(), bd.end()), bd.end());  //unique的作用是“去掉”容器中相邻元素的重复元素，......只剩下不重复的数据
	return bd;                                        //这里去掉要加一个引号，为什么呢，是因为它实质上是一个伪去除，它会把重复的元素添加到容器末尾，而返回值是去重之后的尾地址（是地址！！）
}                                                     //之后用erase清除重复的元素


int PreGraph::GeneFeature(const Mat &img, const Mat &superpixels, const int feaType, Mat &feaSpL, Mat &feaSpA, Mat &feaSpB, Mat &spNpix, Mat &spCnt)
{           //**********************
	Mat feaMap(img.size(), img.type());  //创建一个与img一样的图像数据空间
	img.copyTo(feaMap);  //把imgcopy到feamap
	switch (feaType)
	{
	case 1:
		cvtColor(img, feaMap, CV_BGR2Lab);         //RGB转 LAB颜色空间 。。。。输出到feaMap
		break;
	case 2:
		cvtColor(img, feaMap, CV_BGR2HSV);
		break;
	default:
		break;
	}
	/*vector<float> feaSpL(spNum, 0);
	vector<float> feaSpA(spNum, 0);
	vector<float> feaSpB(spNum, 0);
	vector<int> spNpix(spNum,0);*/
	//vector< vector<float> > spCnt(spNum, vector<float>(2, 0));
	for (int i = 0; i < superpixels.rows; ++i)                                     //看不懂在干嘛。。。。。。。。。大概懂了
	{
		for (int j = 0; j < superpixels.cols; ++j)
		{
			feaSpL.at<float>(superpixels.at<ushort>(i, j)) += feaMap.at<Vec3b>(i, j)[0];   //把每个超像素块里的像素的颜色值求和，后面算超像素节点的平均颜色值
			feaSpA.at<float>(superpixels.at<ushort>(i, j)) += feaMap.at<Vec3b>(i, j)[1];
			feaSpB.at<float>(superpixels.at<ushort>(i, j)) += feaMap.at<Vec3b>(i, j)[2];
			spCnt.at<float>(superpixels.at<ushort>(i, j), 0) += i;                  //计算超像素块内的像素的x坐标之和
			spCnt.at<float>(superpixels.at<ushort>(i, j), 1) += j;                  //计算超像素块内的像素的y坐标之和
			++spNpix.at<float>(superpixels.at<ushort>(i, j), 0);                    //每个超像素中的像素个数
		}
	}
	for (int i = 0; i < spNum; ++i)
	{
		feaSpL.at<float>(i) /= spNpix.at<float>(i);     //计算像素块的平均颜色空间值？
		feaSpA.at<float>(i) /= spNpix.at<float>(i);
		feaSpB.at<float>(i) /= spNpix.at<float>(i);
		spCnt.at<float>(i, 0) /= spNpix.at<float>(i);   //平均的欧式距离？->欧式空间中超像素块的平均坐标
		spCnt.at<float>(i, 1) /= spNpix.at<float>(i);
	}
	/*double minv = 0;
	double maxv = 0;
	minMaxIdx(feaSpL, &minv, &maxv);
	feaSpL = (feaSpL - minv) / (maxv - minv);
	minMaxIdx(feaSpA, &minv, &maxv);
	feaSpA = (feaSpA - minv) / (maxv - minv);
	minMaxIdx(feaSpB, &minv, &maxv);
	feaSpB = (feaSpB - minv) / (maxv - minv);*/
	//cout << "超像素节点的平均坐标:" << "  " << spCnt << endl;
	return 0;

}
//产生关联矩阵//
Mat PreGraph::GeneWeight(const vector<float> &feaSpL, const vector<float> &feaSpA, const vector<float> &feaSpB, const Mat &superpixels, const vector<int> &bd, const Mat &adj)
{
	Mat weightMat(Size(spNum, spNum), CV_32F, Scalar(-1));
	int dist = 0;
	float minv = (float)numeric_limits<float>::max();
	float maxv = (float)numeric_limits<float>::min();
	for (int i = 0; i < spNum; ++i)
	{
		for (int j = 0; j < spNum; ++j)
		{
			if (adj.at<ushort>(i, j) == 1)
			{
				dist = sqrt(pow(feaSpL[i] - feaSpL[j], 2)) + sqrt(pow(feaSpA[i] - feaSpA[j], 2)) + sqrt(pow(feaSpB[i] - feaSpB[j], 2));
				weightMat.at<float>(i, j) = dist;
				if (dist < minv)
					minv = dist;
				if (dist > maxv)
					maxv = dist;

				for (int k = 0; k < spNum; ++k)
				{
					if (adj.at<ushort>(j, k) == 1)
					{
						dist = sqrt(pow(feaSpL[k] - feaSpL[i], 2)) + sqrt(pow(feaSpA[k] - feaSpA[i], 2)) + sqrt(pow(feaSpB[k] - feaSpB[i], 2));
						weightMat.at<float>(i, k) = dist;
						if (dist < minv)
							minv = dist;
						if (dist > maxv)
							maxv = dist;
					}

				}
			}

		}
	}

	for (int i = 0; i < bd.size(); ++i)
	{
		for (int j = 0; j < bd.size(); ++j)
		{
			dist = sqrt(pow(feaSpL[bd[i]] - feaSpL[bd[j]], 2)) + sqrt(pow(feaSpA[bd[i]] - feaSpA[bd[j]], 2)) + sqrt(pow(feaSpB[bd[i]] - feaSpB[bd[j]], 2));
			weightMat.at<float>(bd[i], bd[j]) = dist;
			if (dist < minv)
				minv = dist;
			if (dist > maxv)
				maxv = dist;
		}
	}

	for (int i = 0; i < spNum; ++i)
	{
		for (int j = 0; j < spNum; ++j)
		{
			if (weightMat.at<float>(i, j)>-1)
			{
				weightMat.at<float>(i, j) = (weightMat.at<float>(i, j) - minv) / (maxv - minv);
				weightMat.at<float>(i, j) = exp(-weightMat.at<float>(i, j) / theta);               //theta = 相似度矩阵的公式中的德尔塔的平方 取值为0.1
			}
			else
				weightMat.at<float>(i, j) = 0;
		}
	}

	Mat tmpsuperpixels;
	normalize(weightMat, tmpsuperpixels, 255.0, 0.0, NORM_MINMAX);//将关联矩阵归一化到tmpsuperpixels 矩阵
	tmpsuperpixels.convertTo(tmpsuperpixels, CV_8UC3, 1.0);//  tmpsuperpixels 矩阵转换到Mat格式显示
														   /*imshow("sp", tmpsuperpixels);
														   waitKey();*/
	return weightMat;        //返回高斯相似性矩阵
}

Mat PreGraph::GeneY(const vector<int> &bd)
{
	Mat Y(Size(spNum, 1), CV_32F, Scalar(0));
	for (int i = 0; i < bd.size(); ++i)
	{
		Y.at<float>(bd[i]) = 1;                  //做初始化的标签？？？？？？对，把在边缘的超像素的标签初始化为1
												 //做中心先验性预处理时主要在此处下手，把标签数组改成中心先验显著图之后的二值化标签，然后进行带有两个约束的谱聚类算法
	}
	return Y;
}

Mat PreGraph::inveMat(const Mat &weight, const Mat &Y)
{
	Mat D(Size(weight.size()), CV_32F, Scalar(0));   //D为度矩阵
	Mat diagD(Size(weight.rows, 1), CV_32F);
	reduce(weight, diagD, 1, CV_REDUCE_SUM);  //求高斯相似度矩阵的列和
	D = Mat::diag(diagD);                     //列和变为对角矩阵          。。。。建立度矩阵D。。。。https://blog.csdn.net/v_july_v/article/details/40738211
	Mat LapMat(Size(weight.size()), CV_32F);
	LapMat = D - (1 / (1 + U))*weight;      //产生拉普拉斯矩阵，即基尔霍夫矩阵  L=D-alpha*W(考虑便签数组的两个约束,加入alpha与贝塔)
	Mat E = Mat::eye(weight.size(), CV_32F);    //产生单位矩阵
	Mat inverMat(Size(weight.size()), CV_32F);
	solve(LapMat, E, inverMat, CV_SVD);     //函数 cvSolve 解决线性系统或者最小二乘法问题   CV_SVD - 奇异值分解法 (SVD)  求解LapMat * inverMat= E _————就是求LapMat矩阵的逆矩阵
	Mat tmpMat = Mat::ones(inverMat.size(), CV_32F) - Mat::eye(inverMat.size(), CV_32F);       //ones（）为全1 矩阵     得到一个主对角线为0 ，其他元素全为1的矩阵    主对角线为0，是为了排除自身的相似度
	inverMat = inverMat.mul(tmpMat);  //mul函数为两矩阵的对应元素相乘，当新元素。 invertMat =invertMat* tmpMat(主对角线元素全为0)—>消除LapMat的转置矩阵的主对角线元素
	Mat fData = (U / (1 + U))*inverMat*Y;   //拉普拉斯矩阵的逆矩阵，乘上标签数组的转置   //得到最终的排序函数F*
	return fData;
}

Mat PreGraph::Sal2Img(const Mat &superpixels, const Mat &Sal)
{
	Mat salMap(Size(superpixels.size()), CV_32F);
	for (int i = 0; i < superpixels.rows; ++i)
	{
		for (int j = 0; j < superpixels.cols; ++j)
		{
			salMap.at<float>(i, j) = Sal.at<float>(superpixels.at<ushort>(i, j));
		}
	}
	return salMap;
}
Mat PreGraph::GeneSal(const Mat &img)           //产生显著性图？

{
	Mat superpixels = GeneSp(img);                    //产生超像素图
	Mat feaSpL(Size(1, spNum), CV_32F, Scalar(0));     //定义各个颜色通道的图？？
	Mat feaSpA(Size(1, spNum), CV_32F, Scalar(0));
	Mat feaSpB(Size(1, spNum), CV_32F, Scalar(0));
	Mat spNpix(Size(1, spNum), CV_32F, Scalar(0));   //每个超像素中的像素个数
	Mat spCnt(Size(2, spNum), CV_32F, Scalar(0));      //两行的数组？      有可能是欧式空间中超像素块的坐标
	GeneFeature(img, superpixels, 1, feaSpL, feaSpA, feaSpB, spNpix, spCnt);    //RGB转 LAB颜色空间    //并且得到超像素块的平均颜色空间值与平均的欧式距离？
	Mat adjmat = GeneAdjMat(superpixels);                       //得到一个0，1的矩阵？？ 不知是干嘛的
	vector<int> bd = GeneBdQuery(superpixels, 5);              //得到bd内容为superpixels得图像边缘数据，只有非重复的数值。。。。不知是干嘛
															   //Bd查询？？？ Bd = Background ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?
															   //bd数组，大概是表示在图像的边缘的超像素的序号 //是边界背景的种子点，然后向内传播，但不知为何要排序并去除重复->只记录在图像边界的超像素标号
	Mat affmat = GeneWeight(feaSpL, feaSpA, feaSpB, superpixels, bd, adjmat); //产生高斯相似性矩阵
	Mat Y = GeneY(bd);
	Mat Sal = inveMat(affmat, Y);   //affmat为高斯相似度矩阵 ，返回值为fData...就是谱聚类算法的求解结果F* （就是本文的算法，乘上贝塔即可）
	double minv = 0;
	double maxv = 0;
	minMaxIdx(Sal, &minv, &maxv);  //求显著性图中的最大值与最小值
	Sal = (Sal - minv) / (maxv - minv);     //Sal图的归一化
	Sal = 1 - Sal;
	return Sal;
}
Mat PreGraph::GenCenterSal(const Mat &img)  //分文件类外定义
{

	Mat superpixels = GeneSp(img);                    //产生超像素图
	Mat feaSpL(Size(1, spNum), CV_32F, Scalar(0));     //定义各个颜色通道的图？？ 对
	Mat feaSpA(Size(1, spNum), CV_32F, Scalar(0));
	Mat feaSpB(Size(1, spNum), CV_32F, Scalar(0));
	Mat spNpix(Size(1, spNum), CV_32F, Scalar(0));   //每个超像素中的像素个数
	Mat spCnt(Size(2, spNum), CV_32F, Scalar(0));      //两行的数组？      有可能是欧式空间中超像素块的坐标
	GeneFeature(img, superpixels, 1, feaSpL, feaSpA, feaSpB, spNpix, spCnt);    //RGB转 LAB颜色空间    //并且得到超像素块的平均颜色空间值与平均的欧式距离？
																				//cout << spCnt << endl;
																				//cout << "feaspl" << feaSpL << endl;
	Mat saliency(spNum, 1, CV_32F);
	vector<float> sali(spNum);
	vector<float> Sali(spNum);
	int width = img.cols;        //列
	int height = img.rows;       //行
	int  Cx = width / 2;
	int  Cy = height / 2;
	vector<float> Dc(spNum);   //各个超像素节点距离图像中心的距离
	vector<float> Dp(spNum);   //超像素节点间的欧氏距离
	vector<float> ColorDis(spNum);  //两个超像素节点的颜色距离
	vector<float> Dr(spNum);   //中心先验性的公式右半部分
	for (int i = 0; i < spNum; i++)
	{
		Dc[i] = sqrt(pow(spCnt.at<float>(i, 0) - Cx, 2) + pow(spCnt.at<float>(i, 1) - Cy, 2));
		for (int j = 0; j < spNum; j++)
		{
			Dp[i] = (pow(spCnt.at<float>(i, 0) - spCnt.at<float>(j, 0), 2) + pow(spCnt.at<float>(i, 1) - spCnt.at<float>(j, 1), 2));//可去掉开方 sqrt
			ColorDis[i] = sqrt(pow(feaSpL.at<float>(i) - feaSpL.at<float>(j), 2) + pow(feaSpA.at<float>(i) - feaSpA.at<float>(j), 2) + pow(feaSpB.at<float>(i) - feaSpB.at<float>(j), 2));
			Dr[i] += ColorDis[i] / (1 + Dp[i]);
		}
		sali[i] = (1 / (1 + Dc[i]))*Dr[i] * 5;
		Sali[i] = 1 - exp(-sali[i]);
	}

	/*
	for (int i = 0; i < spNum; i++)
	{
		cout << Sali[i] << endl;
	}
	*/
	for (int i = 0; i < spNum; i++)
	{
		saliency.at<float>(i, 0) = Sali[i];
	}
	//cout << saliency << endl;
	//system("pause");
	return saliency;
}
//********************************************************************无向图排序算法产生显著性图*********************************************************
//需要修改的是初始化的标签值Y和关联矩阵W。
//*******************************************************************************************************************************************************

Mat PreGraph::GeneSortSal(const Mat &img, const Mat &bi)        //产生基于无向图排序即谱聚类算法的优化算法的显著性图
																//img为原图，bi为二值化后的中心先验图
{
	int width = img.cols;        //列数
	int height = img.rows;       //行
	Mat superpixels = GeneSp(img);                    //产生超像素标签矩阵
	int typ123 = superpixels.type();
	//cout << " ~超像素标签矩阵：" << " " << typ123 << endl;//2 ,16位无符号整型单通道整型  ,类型没有问题
	Mat feaSpL(Size(1, spNum), CV_32F, Scalar(0));    //定义各个颜色通道的图？？
	Mat feaSpA(Size(1, spNum), CV_32F, Scalar(0));
	Mat feaSpB(Size(1, spNum), CV_32F, Scalar(0));
	Mat spNpix(Size(1, spNum), CV_32F, Scalar(0));    //每个超像素中的像素个数
	Mat spCnt(Size(2, spNum), CV_32F, Scalar(0));      //两行的数组？      有可能是欧式空间中超像素块的坐标
	GeneFeature(img, superpixels, 1, feaSpL, feaSpA, feaSpB, spNpix, spCnt);    //RGB转 LAB颜色空间    //并且得到超像素块的平均颜色空间值与平均的欧式距离？
	Mat adjmat = GeneAdjMat(superpixels);                       //得到一个0，1的矩阵？？ 不知是干嘛的  1就是超像素块相邻，0就是不相邻，不相邻的就不计算其关联矩阵
	Mat W = GeneSortWeight(feaSpL, feaSpA, feaSpB, superpixels, adjmat); //产生高斯相似性矩阵
																		 //cout << "超像素分割标签矩阵" << " " << superpixels << endl;
	Mat Y(Size(1, spNum), CV_32F, Scalar(0));
	Mat K(Size(1, spNum), CV_32F, Scalar(0));
	//cout << "二值化的矩阵：" << "  " << bi << endl;
	int typbi = bi.type();
	int truecount = 0;
	for (int i = 0; i < bi.rows; i++)
	{
		for (int j = 0; j < bi.cols; j++)
		{
			if (bi.at<uchar>(i, j) >0)      //为什么不能用 ==255？？？？？？？？？？？？？？？？？？？？？？？？？？？？
			{                              //此处有一个中断
				truecount = truecount + 1;
			}
		}
	}
	//cout << "对二值化图直接计数：" << truecount << endl;
	//cout << "typbi:" << " " << typbi << endl; //输入的二值化图bi为8位的单通道矩阵
											  //cout << "显著性函数里的二值化:" <<  bi  << endl;
											  //利用超像素的标签矩阵产生显著性的标签数组************************************************************************************
	int count = 0;    //计算多少个白色的像素点
	int counter = 0;  //在K中统计到的白色个数
	for (int i = 0; i < height; i++)  //从第一行开始遍历
	{
		for (int j = 0; j < width; j++)
		{
			if (bi.at<uchar>(i, j)>0)
			{
				count++;
				K.at<float>(superpixels.at<ushort>(i, j), 0) += 1;    //K的数据类型 float没问题   superpixels.at<ushort>(i, j) 有问题
				if (K.at<float>(superpixels.at<ushort>(i, j),0) > 10)
				{
					Y.at<float>( superpixels.at<ushort>(i, j),0) = 1;
				}
			}

		}
	}
	//cout << "K:" << " " << K.t() << endl;
	//cout << "白色像素点个数：" << count << endl;
	for (int i = 0; i < spNum; i++)
	{
		counter += K.at<float>( i,0);

	}
	//cout << "统计K中的白色点个数：" << counter << endl;
	Mat PY(height, width, CV_8UC1, Scalar(0));
	for (int i = 0; i < spNum; i++)
	{

		if (Y.at<float>(i, 0) == 1)
		{
			PY.at<uchar>(int(spCnt.at<float>(i, 0)), int(spCnt.at<float>(i, 1))) = 255;
		}
	}
	//imshow("标签坐标图", PY);
	//cout << "标签数组：" << " " << Y.t() << endl;
	Mat Sal = inveMat(W, Y);   //W为高斯相似度矩阵 ，返回值为fData...就是谱聚类算法的求解结果F* （就是本文的算法，乘上贝塔即可）

							   //double minv = 0;
							   //double maxv = 0;
							   //minMaxIdx(Sal, &minv, &maxv);  //求显著性图中的最大值与最小值
							   //Sal = (Sal - minv) / (maxv - minv);     //Sal图的归一化
							   //Sal =1- Sal;
	double thr = mean(Sal)[0];  //计算显著性图的平均值
	Mat fgy;
	threshold(Sal, fgy, thr, 1, THRESH_BINARY);     //灰度值的平均值作为二值化的阈值，一般的二值化

	Mat salf;
	salf = inveMat(W, fgy);
	//salf = 1 - salf;
	//cout << "SortSal:" << " " << salf.t() << endl;

	return salf;
}
Mat PreGraph::GeneSortWeight(const vector<float> &feaSpL, const vector<float> &feaSpA, const vector<float> &feaSpB, const Mat &superpixels, const Mat &adj)
{
	Mat weightMat(Size(spNum, spNum), CV_32F, Scalar(-1));
	int dist = 0;
	float minv = (float)numeric_limits<float>::max();
	float maxv = (float)numeric_limits<float>::min();

	for (int i = 0; i < spNum; ++i)
	{
		for (int j = 0; j < spNum; ++j)
		{
			if (adj.at<ushort>(i, j) == 1)
			{
				dist = sqrt(pow(feaSpL[i] - feaSpL[j], 2)) + sqrt(pow(feaSpA[i] - feaSpA[j], 2)) + sqrt(pow(feaSpB[i] - feaSpB[j], 2));
				weightMat.at<float>(i, j) = dist;
				if (dist < minv)
					minv = dist;
				if (dist > maxv)
					maxv = dist;

				for (int k = 0; k < spNum; ++k)
				{
					if (adj.at<ushort>(j, k) == 1 && k != j)
					{
						dist = sqrt(pow(feaSpL[k] - feaSpL[i], 2)) + sqrt(pow(feaSpA[k] - feaSpA[i], 2)) + sqrt(pow(feaSpB[k] - feaSpB[i], 2));
						weightMat.at<float>(i, k) = dist;
						if (dist < minv)
							minv = dist;
						if (dist > maxv)
							maxv = dist;
					}

				}
			}

		}
	}

	for (int i = 0; i < spNum; ++i)
	{
		for (int j = 0; j < spNum; ++j)
		{
			if (weightMat.at<float>(i, j)>-1)
			{
				weightMat.at<float>(i, j) = (weightMat.at<float>(i, j) - minv) / (maxv - minv);
				weightMat.at<float>(i, j) = exp(-weightMat.at<float>(i, j) / theta);               //theta = 相似度矩阵的公式中的德尔塔的平方 取值为0.1
			}
			else
				weightMat.at<float>(i, j) = 0;
		}
	}


	return weightMat;        //返回高斯相似性矩阵
}







