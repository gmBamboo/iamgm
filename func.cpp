/*
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

//给RGB添加透明度(alpha通道)
int addAlpha(Mat& src, cv::Mat& dst, cv::Mat& alpha)
{
	if (src.channels() == 4)
	{
		return -1;
	}
	else if (src.channels() == 1)
	{
		cv::cvtColor(src, src, cv::COLOR_GRAY2RGB);
	}

	dst = cv::Mat(src.rows, src.cols, CV_8UC4);

	std::vector<cv::Mat> srcChannels;
	std::vector<cv::Mat> dstChannels;
	//分离通道
	cv::split(src, srcChannels);

	dstChannels.push_back(srcChannels[0]);
	dstChannels.push_back(srcChannels[1]);
	dstChannels.push_back(srcChannels[2]);
	//添加透明度通道
	dstChannels.push_back(alpha);
	//合并通道
	cv::merge(dstChannels, dst);

	return 0;
}

//创建透明度
cv::Mat createAlpha(cv::Mat& src)
{
	cv::Mat alpha = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	cv::Mat gray = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

	cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			alpha.at<uchar>(i, j) = 255 - gray.at<uchar>(i, j);
		}
	}

	return alpha;
}*/
