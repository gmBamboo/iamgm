
#include <opencv2/opencv.hpp>
#include <iostream>
#include "func.cpp"
using namespace std;
using namespace cv;
//定义两张背景图
//Mat background_01;
//Mat background_02;
//Mat background;
int h_l;
int h_h;
int s_l = 100;
int s_h;
int v_l;
int v_h;

int h_max = 255;
int s_max = 255;
int v_max = 255;

int h_low;
int h_high;
int s_low=100;
int s_high;
int v_low;
int v_high;

//循环播放
int currentFrames = 1;
int currentFramesbg = 1;

//腐蚀/膨胀的内核大小
//int erode_structElementSize = 0;
//绿幕抠图的实现函数
int dilate_structElementSize = 1;
//透明值
int AlphaValueSilder = 0;

Mat replace_and_blend(Mat &frame, Mat &mask, Mat &background);
Mat erode_and_dilate(Mat &mask, int dilate_structElementSize);
void on_Trackbar(int, void*)
{
	h_l = h_low;
	h_h = h_high;

	s_l = s_low;
	s_h = s_high;

	v_l = v_low;
	v_h = v_high;
}

int main()
{
	//background_01 = imread("bg01.jpg");
	//background_02 = imread("bg01.jpg");
	//background = imread("bg.jpg");

	int codec = CV_FOURCC('M', 'J', 'P', 'G');
	double fps = 25.0;                         
	string filename = "save.avi"; 
	Size videoSize(1920, 1080);
	// check if we succeeded  
	VideoWriter write(filename, codec, fps, videoSize, 1);

	//读入视频
	VideoCapture capture, capturebg;
	capture.open("001.mp4");
	if (!capture.isOpened())
		cout << "fail to open!!!" << endl;
	capturebg.open("bg.mp4");
	if (!capturebg.isOpened())
		cout << "fail to open!!!" << endl;
	int totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
	int totalFramesbg = capturebg.get(CV_CAP_PROP_FRAME_COUNT);
	//char * title = "读取视频";
	//char * resultWin = "效果图";
	//namedWindow("读取视频", CV_WINDOW_NORMAL);
	//resizeWindow("读取视频", 960,1000);
	//moveWindow("读取视频",0,0);
	namedWindow("效果图", CV_WINDOW_NORMAL);
	resizeWindow("效果图", 960, 1000);
	moveWindow("效果图", 0, 0);
	namedWindow("mask", CV_WINDOW_NORMAL);
	resizeWindow("mask", 960, 1000);
	moveWindow("mask", 950, 0);
	//Mat frame,framebg;//定义一个Mat变量，用来存储每一帧的图像
	Mat hsv, mask;
	//如果读到了每一帧的图像
	while (1)
	{
		Mat frame, framebg;
		capture >> frame;
		capturebg >> framebg;
		//将每一帧的图像转换到hsv空间
		cvtColor(frame, hsv, COLOR_BGR2HSV);

		//createTrackbar("h_low", "读取视频", &h_low, h_max, on_Trackbar);
		//createTrackbar("h_high", "读取视频", &h_high, h_max, on_Trackbar);

		createTrackbar("s_low", "效果图", &s_low, h_max, on_Trackbar);
		createTrackbar("s_high", "效果图", &s_high, h_max, on_Trackbar);

		createTrackbar("v_low", "效果图", &v_low, h_max, on_Trackbar);
		createTrackbar("v_high", "效果图", &v_high, h_max, on_Trackbar);

		//createTrackbar("erode内核", "mask", &erode_structElementSize, 21, on_Trackbar);
		createTrackbar("dilate内核", "mask", &dilate_structElementSize, 21, on_Trackbar);
		//createTrackbar("透明值", "效果图", &AlphaValueSilder, 100, on_Trackbar_Alpha);

		//蓝幕的颜色范围，将结果存在mask中

		inRange(hsv, Scalar(100, s_l, 46), Scalar(124, 255, 255), mask);
		//inRange(hsv, Scalar(100, 120, 46), Scalar(124, 255, 255), mask);
		//对mask进行形态学操作
		//定义一个结构
		Mat k = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
		//对mask进行形态学闭操作
		morphologyEx(mask, mask, MORPH_CLOSE, k);
		erode(mask, mask, k);

		//对mask进行腐蚀膨胀微调
		mask = erode_and_dilate(mask, dilate_structElementSize);
		
		//高斯模糊
		GaussianBlur(mask, mask, Size(9, 9), 10, 10);

		Mat result = replace_and_blend(frame, mask, framebg);
		write << result;
		imshow("效果图", result);
		
		char c = waitKey(30);//延时30ms
		if (c == 27)//如果按下ESC那么就退出
			break;

		if (currentFrames == totalFrames - 1)
		{
			currentFrames = 0;
			capture.set(CV_CAP_PROP_POS_FRAMES, 1);
		}
		if (currentFramesbg == totalFramesbg - 1)
		{
			currentFramesbg = 0;
			capturebg.set(CV_CAP_PROP_POS_FRAMES, 1);
		}
		currentFrames++;
		currentFramesbg++;
	}
	waitKey(0);
	return 0;
}

//透明度
//void on_Trackbar_Alpha(int , void*)
//{
//	Mat alpha = addAlpha();
//	//当前AlphaValue相对于最大的比例
//	double AlphaValue = (double)AlphaValueSilder / 100;
//	//beta
//	double beta= (1.0 - AlphaValue);
//    
//}


//腐蚀
Mat erode_and_dilate(Mat &mask, int dilate_structElementSize)
{   
	Mat new_mask;
	Mat dilate_element = getStructuringElement(MORPH_RECT, Size(2 * dilate_structElementSize + 1, 2 * dilate_structElementSize + 1), Point(dilate_structElementSize, dilate_structElementSize));

	dilate(mask, new_mask, dilate_element);

	return new_mask;
}


//对视频的每一帧的图像进行处理
Mat replace_and_blend(Mat &frame, Mat &mask ,Mat &background)
{

	imshow("mask", mask);
	//创建一张结果图
	Mat result = Mat(frame.size(), frame.type());
	//图像的高 宽 与通道数
	int height = result.rows;
	int width = result.cols;
	int channels = result.channels();
	//int nStep = width*channels;

	// replace and blend
	int m = 0;//mask的像素值
	double wt = 0;//融合的比例

	int r = 0, g = 0, b = 0;//输出的像素
	int r1 = 0, g1 = 0, b1 = 0;
	int r2 = 0, g2 = 0, b2 = 0;
	for (int i = 0; i < height; i++)
	{
		//定义每一行 每一帧图像的指针，mask图像的指针，两张背景图的指针,结果图的指针
		uchar *pbg = background.ptr<uchar>(i);
		uchar *pframe = frame.ptr<uchar>(i);
		uchar *pmask = mask.ptr<uchar>(i);
		uchar *presult = result.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			m = *pmask++;//读取mask的像素值
			if (m == 255)//如果是背景的话
			{
				//进行三个通道的赋值
				*presult++ = *pbg++;
				*presult++ = *pbg++;
				*presult++ = *pbg++;
				pframe += 3;//将frame的图像的像素的通道也移动单个保持一致
			}
			else if (m == 0)//如果是前景的话
			{
				//进行三个通道的赋值
				*presult++ = *pframe++;
				*presult++ = *pframe++;
				*presult++ = *pframe++;
				pbg += 3;//将frame的图像的像素的通道也移动单个保持一致
			}
			else
			{
				//背景图每个像素的三个通道
				b1 = *pbg++;
				g1 = *pbg++;
				r1 = *pbg++;

				//每一帧每一个像素的三个通道
				b2 = *pframe++;
				g2 = *pframe++;
				r2 = *pframe++;

				// 权重
				wt = m / 255.0;
				wt *= 2.5;
				wt = min(wt, 1.0);
				//cout << wt << endl;
				// 混合
				b = b1 * wt + b2 * (1.0 - wt);
				g = g1 * wt + g2 * (1.0 - wt);
				r = r1 * wt + r2 * (1.0 - wt);

				*presult++ = b;
				*presult++ = g;
				*presult++ = r;
			}
		}
	}


	return result;
}
