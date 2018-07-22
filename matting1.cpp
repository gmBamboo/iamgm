#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

//hsv�ռ��9������
int h_max = 255;
int s_max = 255;
int v_max = 255;

int h_low;
int h_high;
int s_low = 100;
int s_high;
int v_low;
int v_high;

//��ֵ���ֵ
//int value_min = 0;
//int value_max = 150;

//�����뷶Χ
int dist_thres = 0;
int dist_value = 0;
int dist_thres_max = 100;
int dist_value_max = 300;

//ѭ������
int currentFrames = 1;
int currentFramesbg = 1;

//��ʴ/���͵��ں˴�С
//int erode_structElementSize = 0;
int dilate_structElementSize = 0;

//͸��ֵ
//int AlphaValueSilder = 0;

//�Աȶ�������
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

//����ͼ
Mat dist;
void on_Trackbar(int, void*)
{

}

int main()
{
    //д����Ƶ
	int codec = CV_FOURCC('M', 'J', 'P', 'G');
	double fps = 20.0;
	string filename = "save.avi";
	Size videoSize(1920, 1080);
	VideoWriter write(filename, codec, fps, videoSize, 1);

	//������Ƶ
	VideoCapture capture, capturebg;
	capture.open("001.mp4");
	if (!capture.isOpened())
		cout << "fail to open!!!" << endl;
	capturebg.open("bg.mp4");
	if (!capturebg.isOpened())
		cout << "fail to open!!!" << endl;
	int totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
	int totalFramesbg = capturebg.get(CV_CAP_PROP_FRAME_COUNT);

	//���ڴ���
	//namedWindow("��ȡ��Ƶ", CV_WINDOW_NORMAL);
	//resizeWindow("��ȡ��Ƶ", 960,1000);
	//moveWindow("��ȡ��Ƶ",0,0);
	namedWindow("Ч��ͼ", CV_WINDOW_NORMAL);
	resizeWindow("Ч��ͼ", 960, 1000);
	moveWindow("Ч��ͼ", 0, 0);
	namedWindow("mask", CV_WINDOW_NORMAL);
	resizeWindow("mask", 960, 500);
	moveWindow("mask", 950, 0);
	namedWindow("����ͼ", CV_WINDOW_NORMAL);
	resizeWindow("����ͼ", 960, 500);
	moveWindow("����ͼ", 950, 500);

	Mat hsv, mask;
	//���������ÿһ֡��ͼ��
	while (1)
	{
		Mat frame, framebg;
		capture >> frame;
		capturebg >> framebg;
		//��ÿһ֡��ͼ��ת����hsv�ռ�
		cvtColor(frame, hsv, COLOR_BGR2HSV);

		//����������
		//createTrackbar("h_low", "Ч��ͼ", &h_low, h_max, on_Trackbar);
		//createTrackbar("h_high", "Ч��ͼ", &h_high, h_max, on_Trackbar);
		createTrackbar("s_low", "Ч��ͼ", &s_low, s_max, on_Trackbar);
		//createTrackbar("s_high", "Ч��ͼ", &s_high, s_max, on_Trackbar);
		createTrackbar("v_low", "Ч��ͼ", &v_low, v_max, on_Trackbar);
		//createTrackbar("v_high", "Ч��ͼ", &v_high, v_max, on_Trackbar);
		createTrackbar("alpha", "Ч��ͼ", &alpha, alpha_max, on_Trackbar); 
		createTrackbar("beta", "Ч��ͼ", &beta, beta_max, on_Trackbar);
		createTrackbar("alpha1", "Ч��ͼ", &alpha1, alpha_max, on_Trackbar);
		createTrackbar("beta1", "Ч��ͼ", &beta1, beta_max, on_Trackbar);

		createTrackbar("dilate�ں�", "mask", &dilate_structElementSize, 21, on_Trackbar);

		//createTrackbar("��ֵ", "����ͼ", &value_min, 150, on_Trackbar);
		//createTrackbar("��ֵ", "����ͼ", &value_max, 255, on_Trackbar);
		createTrackbar("����", "����ͼ", &dist_value, dist_value_max, on_Trackbar);
		createTrackbar("��ֵ", "����ͼ", &dist_thres, dist_thres_max, on_Trackbar);

		func(frame, alpha, beta);
		//��������ͼ
		distance(frame);

		imshow("����ͼ", dist);

		//��Ļ����ɫ��Χ��hsv������
		//inRange(hsv, Scalar(h_low, s_low, v_low), Scalar(h_high, s_high, v_max), mask);
		inRange(hsv, Scalar(100, s_low, v_low), Scalar(124, 255, 255), mask);

		//��mask������̬ѧ����
		//����һ���ṹ
		Mat k = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
		//��mask������̬ѧ�ղ���
		morphologyEx(mask, mask, MORPH_CLOSE, k);
		//erode(mask, mask, k);

		//��mask���и�ʴ����΢��
		mask = erode_and_dilate(mask, dilate_structElementSize);

		//��˹ģ��
		GaussianBlur(mask, mask, Size(9, 9), 11);

		//bitwise_not(mask, mask);
		//threshold(mask, mask, 124, 255, thresh_binary);

		Mat result = replace_and_blend(frame, mask, framebg);
		char c = waitKey(38);//��ʱ30ms
		if (c == 27)//�������ESC��ô���˳�
			break;

		func(result, alpha1, beta1);
		write << result;
		//Mat result1;
		//medianBlur(result, result1, 5);
		//�������ȶԱȶ�
		imshow("Ч��ͼ", result);

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

//distanceͼ

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
			//��ֵ���ֵ����
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

//��ʴ
Mat erode_and_dilate(Mat &mask, int dilate_structElementSize)
{
	Mat new_mask;
	Mat dilate_element = getStructuringElement(MORPH_RECT, Size(2 * dilate_structElementSize + 1, 2 * dilate_structElementSize + 1), Point(dilate_structElementSize, dilate_structElementSize));

	dilate(mask, new_mask, dilate_element);

	return new_mask;
}

//����Ƶ��ÿһ֡��ͼ����д���
Mat replace_and_blend(Mat &frame, Mat &mask, Mat &background)
{
	imshow("mask", mask);
	//����һ�Ž��ͼ
	Mat result = Mat(frame.size(), frame.type());
	//ͼ��ĸ� �� ��ͨ����
	int height = result.rows;
	int width = result.cols;
	int channels = result.channels();
	//int nstep = width*channels;

	// replace and blend
	int m = 0;//mask������ֵ
	double wt = 0;//�ںϵı���

	int r = 0, g = 0, b = 0;//���������
	int r1 = 0, g1 = 0, b1 = 0;
	int r2 = 0, g2 = 0, b2 = 0;
	for (int i = 0; i < height; i++)
	{
		//����ÿһ�� ÿһ֡ͼ���ָ�룬maskͼ���ָ�룬���ű���ͼ��ָ��,���ͼ��ָ��
		uchar *pbg = background.ptr<uchar>(i);
		uchar *pframe = frame.ptr<uchar>(i);
		uchar *pmask = mask.ptr<uchar>(i);
		uchar *presult = result.ptr<uchar>(i);
		uchar *pdist = dist.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			m = *pmask++;//��ȡmask������ֵ
			if (m == 253)//����Ǳ����Ļ�
			{
				//��������ͨ���ĸ�ֵ
				*presult++ = *pbg++ + 5;
				*presult++ = *pbg++ + 5;
				*presult++ = *pbg++ +5;
				pframe += 3;//��frame��ͼ������ص�ͨ��Ҳ�ƶ���������һ��
				pdist ++; //��ͨ��
			}
			else if (m == 0)//�����ǰ���Ļ�
			{
				//��������ͨ���ĸ�ֵ
				int mm = *pdist++;
				//����ͼÿ�����ص�����ͨ��
				b1 = *pbg++;
				g1 = *pbg++;
				r1 = *pbg++;

				//ÿһ֡ÿһ�����ص�����ͨ��
				b2 = *pframe++;
				g2 = *pframe++;
				r2 = *pframe++;
				// ���
				// ����
				//wt = 1.0-(mm / 255.0);
				//����
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
				//����ͼÿ�����ص�����ͨ��
				b1 = *pbg++;
				g1 = *pbg++;
				r1 = *pbg++;

				//ÿһ֡ÿһ�����ص�����ͨ��
				b2 = *pframe++;
				g2 = *pframe++;
				r2 = *pframe++;

				pdist ++;
				// Ȩ��
				wt = m / 255.0;
				wt *= 2.5;
				wt = min(wt, 1.0);
				//cout << wt << endl;
				// ���
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