#include <opencv2/opencv.hpp>
#include <iostream>
#include "func.cpp"
using namespace std;
using namespace cv;
//�������ű���ͼ
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
int s_low = 100;
int s_high;
int v_low;
int v_high;

//ѭ������
int currentFrames = 1;
int currentFramesbg = 1;

//��ʴ/���͵��ں˴�С
//int erode_structElementSize = 0;
//��Ļ��ͼ��ʵ�ֺ���
int dilate_structElementSize = 1;
//͸��ֵ
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
	double fps = 20.0;
	string filename = "save.avi";
	Size videoSize(1920, 1080);
	// check if we succeeded  
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
	//char * title = "��ȡ��Ƶ";
	//char * resultWin = "Ч��ͼ";
	//namedWindow("��ȡ��Ƶ", CV_WINDOW_NORMAL);
	//resizeWindow("��ȡ��Ƶ", 960,1000);
	//moveWindow("��ȡ��Ƶ",0,0);
	namedWindow("Ч��ͼ", CV_WINDOW_NORMAL);
	resizeWindow("Ч��ͼ", 960, 1000);
	moveWindow("Ч��ͼ", 0, 0);
	namedWindow("mask", CV_WINDOW_NORMAL);
	resizeWindow("mask", 960, 1000);
	moveWindow("mask", 950, 0);
	//Mat frame,framebg;//����һ��Mat�����������洢ÿһ֡��ͼ��
	Mat hsv, mask;
	//���������ÿһ֡��ͼ��
	while (1)
	{
		Mat frame, framebg;
		capture >> frame;
		capturebg >> framebg;
		//��ÿһ֡��ͼ��ת����hsv�ռ�
		cvtColor(frame, hsv, COLOR_BGR2HSV);

		//createTrackbar("h_low", "��ȡ��Ƶ", &h_low, h_max, on_Trackbar);
		//createTrackbar("h_high", "��ȡ��Ƶ", &h_high, h_max, on_Trackbar);

		createTrackbar("s_low", "Ч��ͼ", &s_low, h_max, on_Trackbar);
		createTrackbar("s_high", "Ч��ͼ", &s_high, h_max, on_Trackbar);

		createTrackbar("v_low", "Ч��ͼ", &v_low, h_max, on_Trackbar);
		createTrackbar("v_high", "Ч��ͼ", &v_high, h_max, on_Trackbar);

		//createTrackbar("erode�ں�", "mask", &erode_structElementSize, 21, on_Trackbar);
		createTrackbar("dilate�ں�", "mask", &dilate_structElementSize, 21, on_Trackbar);
		//createTrackbar("͸��ֵ", "Ч��ͼ", &AlphaValueSilder, 100, on_Trackbar_Alpha);

		//��Ļ����ɫ��Χ�����������mask��

		inRange(hsv, Scalar(100, 130, 80), Scalar(124, 255, 255), mask);
		//inRange(hsv, Scalar(100, 120, 46), Scalar(124, 255, 255), mask);
		//��mask������̬ѧ����
		//����һ���ṹ
		Mat k = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
		//��mask������̬ѧ�ղ���
		morphologyEx(mask, mask, MORPH_CLOSE, k);
		//erode(mask, mask, k);

		//��mask���и�ʴ����΢��
		mask = erode_and_dilate(mask, dilate_structElementSize);

		//��˹ģ��
		GaussianBlur(mask, mask, Size(9, 9),11);
		//bitwise_not(mask, mask);
		//threshold(mask, mask, 124, 255, thresh_binary);
		Mat result = replace_and_blend(frame, mask, framebg);
		char c = waitKey(38);//��ʱ30ms
		if (c == 27)//�������ESC��ô���˳�
			break;

		write << result;
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

//͸����
//void on_Trackbar_Alpha(int , void*)
//{
//	Mat alpha = addAlpha();
//	//��ǰAlphaValue��������ı���
//	double AlphaValue = (double)AlphaValueSilder / 100;
//	//beta
//	double beta= (1.0 - AlphaValue);
//    
//}


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
		for (int j = 0; j < width; j++)
		{
			m = *pmask++;//��ȡmask������ֵ
			if (m == 253)//����Ǳ����Ļ�
			{
				//��������ͨ���ĸ�ֵ
				*presult++ = *pbg++;
				*presult++ = *pbg++;
				*presult++ = *pbg++;
				pframe += 3;//��frame��ͼ������ص�ͨ��Ҳ�ƶ���������һ��
			}
			else if (m == 0)//�����ǰ���Ļ�
			{
				//��������ͨ���ĸ�ֵ
				*presult++ = *pframe++;
				*presult++ = *pframe++;
				*presult++ = *pframe++;
				pbg += 3;//��frame��ͼ������ص�ͨ��Ҳ�ƶ���������һ��
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
			//�ڱ���
			//for (int j = 0; j < width; j++)
			//{
			//	m = *pmask++;//��ȡmask������ֵ
			//	if (m == 0)//����Ǳ����Ļ�
			//	{
			//		//��������ͨ���ĸ�ֵ
			//		*presult++ = 0;
			//		*presult++ = 0;
			//		*presult++ = 0;
			//		pbg += 3;
			//		pframe += 3;//��frame��ͼ������ص�ͨ��Ҳ�ƶ���������һ��
			//	}
			//	else
			//	{
			//		*presult++ = *pframe++;
			//		*presult++ = *pframe++;
			//		*presult++ = *pframe++;
			//		pbg += 3;
			//	}

			//}
	}


	return result;
}