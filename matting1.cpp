#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

//hsv空间的9个参数
int h_max = 255;
int s_max = 255;
int v_max = 255;

int h_low;
int h_high;
int s_low = 100;
int s_high;
int v_low;
int v_high;

//高值与低值
//int value_min = 0;
//int value_max = 150;

//距离与范围
int dist_thres = 0;
int dist_value = 0;
int dist_thres_max = 100;
int dist_value_max = 300;

//循环播放
int currentFrames = 1;
int currentFramesbg = 1;

//腐蚀/膨胀的内核大小
//int erode_structElementSize = 0;
int dilate_structElementSize = 0;

//透明值
//int AlphaValueSilder = 0;

//对比度与亮度
int alpha = 10;
int beta;
int alpha_max = 30;
int beta_max = 100;

int alpha1 = 10;
int beta1;

Mat replace_and_blend(Mat &frame, Mat &mask, Mat &background);
Mat erode_and_dilate(Mat &mask, int dilate_structElementSize);
void distance(cv::Mat &frame);
void func(Mat &frame, int alpha, int beta);

//距离图
Mat dist;
void on_Trackbar(int, void*)
{

}

int main()
{
    //写入视频
	int codec = CV_FOURCC('M', 'J', 'P', 'G');
	double fps = 20.0;
	string filename = "save.avi";
	Size videoSize(1920, 1080);
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

	//窗口创建
	//namedWindow("读取视频", CV_WINDOW_NORMAL);
	//resizeWindow("读取视频", 960,1000);
	//moveWindow("读取视频",0,0);
	namedWindow("效果图", CV_WINDOW_NORMAL);
	resizeWindow("效果图", 960, 1000);
	moveWindow("效果图", 0, 0);
	namedWindow("mask", CV_WINDOW_NORMAL);
	resizeWindow("mask", 960, 500);
	moveWindow("mask", 950, 0);
	namedWindow("距离图", CV_WINDOW_NORMAL);
	resizeWindow("距离图", 960, 500);
	moveWindow("距离图", 950, 500);

	Mat hsv, mask;
	//如果读到了每一帧的图像
	while (1)
	{
		Mat frame, framebg;
		capture >> frame;
		capturebg >> framebg;
		//将每一帧的图像转换到hsv空间
		cvtColor(frame, hsv, COLOR_BGR2HSV);

		//滑动条创建
		//createTrackbar("h_low", "效果图", &h_low, h_max, on_Trackbar);
		//createTrackbar("h_high", "效果图", &h_high, h_max, on_Trackbar);
		createTrackbar("s_low", "效果图", &s_low, s_max, on_Trackbar);
		//createTrackbar("s_high", "效果图", &s_high, s_max, on_Trackbar);
		createTrackbar("v_low", "效果图", &v_low, v_max, on_Trackbar);
		//createTrackbar("v_high", "效果图", &v_high, v_max, on_Trackbar);
		createTrackbar("alpha", "效果图", &alpha, alpha_max, on_Trackbar); 
		createTrackbar("beta", "效果图", &beta, beta_max, on_Trackbar);
		createTrackbar("alpha1", "效果图", &alpha1, alpha_max, on_Trackbar);
		createTrackbar("beta1", "效果图", &beta1, beta_max, on_Trackbar);

		createTrackbar("dilate内核", "mask", &dilate_structElementSize, 21, on_Trackbar);

		//createTrackbar("低值", "距离图", &value_min, 150, on_Trackbar);
		//createTrackbar("高值", "距离图", &value_max, 255, on_Trackbar);
		createTrackbar("距离", "距离图", &dist_value, dist_value_max, on_Trackbar);
		createTrackbar("阈值", "距离图", &dist_thres, dist_thres_max, on_Trackbar);

		func(frame, alpha, beta);
		//创建距离图
		distance(frame);

		imshow("距离图", dist);

		//蓝幕的颜色范围，hsv抠轮廓
		//inRange(hsv, Scalar(h_low, s_low, v_low), Scalar(h_high, s_high, v_max), mask);
		inRange(hsv, Scalar(100, s_low, v_low), Scalar(124, 255, 255), mask);

		//对mask进行形态学操作
		//定义一个结构
		Mat k = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
		//对mask进行形态学闭操作
		morphologyEx(mask, mask, MORPH_CLOSE, k);
		//erode(mask, mask, k);

		//对mask进行腐蚀膨胀微调
		mask = erode_and_dilate(mask, dilate_structElementSize);

		//高斯模糊
		GaussianBlur(mask, mask, Size(9, 9), 11);

		//bitwise_not(mask, mask);
		//threshold(mask, mask, 124, 255, thresh_binary);

		Mat result = replace_and_blend(frame, mask, framebg);
		char c = waitKey(38);//延时30ms
		if (c == 27)//如果按下ESC那么就退出
			break;

		func(result, alpha1, beta1);
		write << result;
		//Mat result1;
		//medianBlur(result, result1, 5);
		//调节亮度对比度
		imshow("效果图", result);

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

void func(Mat &frame, int alpha, int beta)
{
	double A = (double)alpha/10;
	int height = frame.rows;
	int width = frame.cols;
	for (int i = 0; i < height; i++){
		uchar *pframe = frame.ptr<uchar>(i);
		for (int j = 0; j < width; j++){
			int b = *pframe++;
			int g = *pframe++;
			int r = *pframe++;
			pframe -= 3;
			*pframe++ = min((int)(b*A + beta), 255);
			*pframe++ = min((int)(g*A + beta), 255);
			*pframe++ = min((int)(r*A + beta), 255);
		}
	}
}

//distance图

void distance(Mat &frame)
{
	dist = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
	int b, g, r;
	double dis;
	int height = frame.rows;
	int width = frame.cols;
	for (int i = 0; i < height; i++){
		uchar *presult = dist.ptr<uchar>(i);
		uchar *pbg = frame.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			b = *pbg++;
			g = *pbg++;
			r = *pbg++;
			dis = sqrt(pow((b - 255), 2) + pow((g - 126), 2) + pow((r - 61), 2));
			/*
			//低值与高值部分
			if (dis <= value_min){
				*presult++ = 0;
			}
			else if (dis >=value_max)
			{
				*presult++ = 255;
			}
			else if(dis>value_min && dis<value_min + value1){
				*presult++ = 0;
			}
			else if (dis>value_max - value1 && dis<value_max){
				*presult++ = 0;
			}
			else{
				*presult++ = dis;
			}
			*/
			if (dis <= dist_value){
				*presult++ = 0;
			}
			else if (dis >= dist_value + dist_thres)
			{
				*presult++ = 255;
			}
			else{
				*presult++ = dis;
			}
		}
	}
	GaussianBlur(dist, dist, Size(9, 9),7);
}

//腐蚀
Mat erode_and_dilate(Mat &mask, int dilate_structElementSize)
{
	Mat new_mask;
	Mat dilate_element = getStructuringElement(MORPH_RECT, Size(2 * dilate_structElementSize + 1, 2 * dilate_structElementSize + 1), Point(dilate_structElementSize, dilate_structElementSize));

	dilate(mask, new_mask, dilate_element);

	return new_mask;
}

//对视频的每一帧的图像进行处理
Mat replace_and_blend(Mat &frame, Mat &mask, Mat &background)
{
	imshow("mask", mask);
	//创建一张结果图
	Mat result = Mat(frame.size(), frame.type());
	//图像的高 宽 与通道数
	int height = result.rows;
	int width = result.cols;
	int channels = result.channels();
	//int nstep = width*channels;

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
		uchar *pdist = dist.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			m = *pmask++;//读取mask的像素值
			if (m == 253)//如果是背景的话
			{
				//进行三个通道的赋值
				*presult++ = *pbg++ + 5;
				*presult++ = *pbg++ + 5;
				*presult++ = *pbg++ +5;
				pframe += 3;//将frame的图像的像素的通道也移动单个保持一致
				pdist ++; //单通道
			}
			else if (m == 0)//如果是前景的话
			{
				//进行三个通道的赋值
				int mm = *pdist++;
				//背景图每个像素的三个通道
				b1 = *pbg++;
				g1 = *pbg++;
				r1 = *pbg++;

				//每一帧每一个像素的三个通道
				b2 = *pframe++;
				g2 = *pframe++;
				r2 = *pframe++;
				// 混合
				// 线性
				//wt = 1.0-(mm / 255.0);
				//二次
				//wt = 1.0/(pow(255,2)*2) * pow(mm,2) -1.0/255.0*mm + 0.5;
				wt = 0.0000077 * pow(mm,2) -0.0039215*mm + 0.5;
				//wt = min(wt, 0.3);
				b = b1 * wt + b2 * (1.0 - wt);
				g = g1 * wt + g2 * (1.0 - wt);
				r = r1 * wt + r2 * (1.0 - wt);

				*presult++ = b;
				*presult++ = g;
				*presult++ = r;
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

				pdist ++;
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