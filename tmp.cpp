// tmp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cv.h"
#include <tracker.hpp>
#include <tldDataset.hpp>
#include <opencv2/opencv.hpp>
#include <ROIPick.h>

using namespace cv;
using namespace std;

void getCanny(Mat gray, Mat &canny) {
	Mat thres;
	double high_thres = threshold(gray, thres, 0, 255, THRESH_BINARY | THRESH_OTSU), low_thres = high_thres * 0.5;
	Canny(gray, canny, low_thres, high_thres);
}

void findmaxcontour(Mat &_src);

int _tmain(int argc, _TCHAR* argv[])
{
	MultiTracker obj("KCF");
	string str;
	//str = argv[1];
	str = "d:/Document/class/Digital_Image/Sample/step_1.wmv";
	//str = "d:/tmp/demo.jpg";
	//Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Mat src ,src_gray;
	//src = imread(str);
	//if (src.empty())
	//{
	//	cout << "canno open" << endl;
	//	return -1;
	//}
	//cvtColor(src, src_gray, CV_BGR2GRAY);
	//blur(src_gray, src_gray, Size(3, 3));
	///// Detect edges using Threshold
	//threshold(src_gray, threshold_output, 128, 255, THRESH_BINARY);
	//findmaxcontour(threshold_output);
	///// Find contours
	//findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	//str = "d:/tmp/step_2.avi";
	VideoCapture cap;
	cap.open(str);
	if (!cap.isOpened())
	{
		cout << "cannot open the file" << endl;
		cin.get();
		return -1;
	}
	Mat frame, fgKnn, fgMog2;
	Ptr<BackgroundSubtractor>pKnn, pMog2;
	pKnn = createBackgroundSubtractorKNN();
	pMog2 = createBackgroundSubtractorMOG2();

	cap >> frame;
	pickROI(frame);
	namedWindow("findcontours");
	namedWindow("org");
	while (true)
	{
		if (waitKey(10)==27)
			break;
		cap >> frame;
		if (frame.empty())
			break;
		imshow("org", frame);
		pKnn->apply(frame, fgKnn, -0.5);
		//pMog2->apply(frame, fgMog2, -0.1);
		imshow("Knn", fgKnn);
		//imshow("Mog2", fgMog2);
		findmaxcontour(fgKnn);
	}

	//Mat src = imread(str,0);
	//double scale = 0.3;
	//Size display_size(int(src.cols*scale), int (src.rows*0.3));
	//resize(src, src, display_size);
	//imshow("src",src);//缩小显示

	//open or tophat
	//Mat element = getStructuringElement(MORPH_RECT, Size(10, 35), Point(0, 0));
	//Mat dst;
	////morphologyEx(src, dst, MORPH_OPEN, element);
	//morphologyEx(src, dst, MORPH_OPEN, element,Point(0,0));
	//floodFill(dst, Point(20, dst.rows*0.8), Scalar(255, 255, 255));
	
	//equalizeHist(dst, dst);
	//imshow("dst", dst);
	//Mat binary_pic;
	//threshold(dst, binary_pic, 70, 255, THRESH_BINARY);
	
/*	element = getStructuringElement(MORPH_RECT, Size(300, 10), Point(0, 0));
	morphologyEx(binary_pic, binary_pic, MORPH_OPEN, element);
	*///element = getStructuringElement(MORPH_RECT, Size(10, 300), Point(0, 0));
	//morphologyEx(binary_pic, binary_pic, MORPH_OPEN, element);
	//imshow("output", binary_pic);

	//Mat canny;
	//getCanny(dst, canny);
	//imshow("canny", canny);

	////findmaxcontour(canny);

	waitKey(0);
	return 0;
}


void findmaxcontour(Mat &_src)
{
	vector<vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;
	vector<cv::Point> contourmax;

	//cvFindContours(src, coutour, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	//findContours(src,contours, CV_RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat src;
	//if (_src.channels() == 1)
	//{
	//	_src.copyTo(src);
	//	src.convertTo(src,CV_8UC3);
	//}
	//else
	src = _src.clone();
	//rectangle(src, Rect(0, src.rows / 3 * 2, src.cols, src.rows / 3), Scalar::all(0), -1);
	//blur(src, src, Size(3, 3));
	/// Detect edges using Threshold
	threshold(src, src, 10, 255, THRESH_BINARY);
		
	//src.convertTo(src, CV_8UC1);
	//threshold(src, src, 128, 255, THRESH_BINARY|THRESH_OTSU);
	//adaptiveThreshold(src, src, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 9, 0);
	//imshow("org", src);
	//Mat canny;
	//getCanny(src, canny);
	//findContours(src, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	Mat dst;
	dst = src.clone();
	cvtColor(dst, dst, CV_GRAY2BGR);
	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//findContours(src, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//src = src > 1;
	int idx = 0;
	int max_idx = 0;
	//int area, maxArea = 1;
	vector<vector<cv::Point>>::const_iterator itc = contours.begin();
	while (itc!=contours.end())
	{
		if (itc->size() > 200 || itc->size() < 3)
			itc = contours.erase(itc);
		else
			++itc;
	}
	//for (int i = 0; i < (int)contours.size(); i++)
	//{
	//	area = int(fabs(contourArea(contours[i], 0)));
	//	if (area>maxArea)
	//	{
	//		maxArea = area;
	//		max_idx = i;
	//		contourmax = contours[i];
	//	}
	//}
	//approximate contours to polygons
	vector<vector<cv::Point>> contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], contours_poly[i], 3, true);
		boundRect[i] = boundingRect(contours_poly[i]);
		minEnclosingCircle(contours_poly[i], center[i], radius[i]);
	}

	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color(rand() % 255, rand() % 255, rand() % 255);
		//drawContours(dst, contours, i, Scalar::all(0), CV_FILLED, 8, vector<Vec4i>(), 0, Point());
		drawContours(dst, contours_poly,i, Scalar(128, 128, 0), 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(dst, boundRect[i].tl(), boundRect[i].br(),Scalar(0,200,0),1, 8, 0);
		circle(dst, center[i], (int)radius[i], Scalar(0, 0, 200), 1, 8, 0);
		//drawContours(dst, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}

	//for (; idx >= 0; idx = hierarchy[idx][0])
	//{
	//	Scalar color(rand() & 255, rand() & 255, rand() & 255);
	//	drawContours(dst, contours, idx, color, CV_FILLED, 8, hierarchy);
	//	area = int(fabs(contourArea(contours[idx], 0)));
	//	//cout << "area=" << area << endl;
	//	if (area>maxArea)
	//	{
	//		maxArea = area;
	//		max_idx = idx;
	//		contourmax = contours[idx];
	//	}
	//}

	//drawContours(dst, contours, max_idx, Scalar::all(255), CV_FILLED, 8, vector<Vec4i>(), 0, Point());
	//drawContours(dst, contours, max_idx, Scalar::all(255), CV_FILLED, 8, hierarchy);
	cout << " contour_size"<<contours.size()<< endl;

	//drawContours(dst, contours,idx,Scalar(255,0,0),2,8,noArray(),10,Point(0,0));
	imshow("findcontours", dst);
}