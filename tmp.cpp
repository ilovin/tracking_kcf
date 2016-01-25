// tmp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cv.h"
#include <tracker.hpp>
#include <tldDataset.hpp>
#include <opencv2/opencv.hpp>
#include <ROIPick.h>
#include "ViBe.h"

using namespace cv;
using namespace std;
//#define minWidth 8
//#define minHeight 22
#define track_target mask_color

void getCanny(Mat gray, Mat &canny) {
	Mat thres;
	double high_thres = threshold(gray, thres, 0, 255, THRESH_BINARY | THRESH_OTSU), low_thres = high_thres * 0.5;
	Canny(gray, canny, low_thres, high_thres);
}

std::vector<Rect2d> findmaxcontour(Mat &_src,int minWidth,int minHeight);
Rect2d limit_area;
Mat dst;


int _tmain(int argc, _TCHAR* argv[])
{
	string str;
	//str = argv[1];
	//str = "d:/Document/class/Digital_Image/Sample/1/%5d.jpg";
	str = "d:/Document/class/Digital_Image/Sample/step_3.wmv";
	//str = "d:/tmp/demo.jpg";
	//str = "d:/tmp/demo0.avi";
	//Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	VideoCapture cap;
	cap.open(str);
	if (!cap.isOpened())
	{
		cout << "cannot open the file" << endl;
		cin.get();
		return -1;
	}
	Mat frame, fgKnn, fgMog2;
	Mat frame_color,fgKnn_color;
	Mat gray, mask,mask_color;
	Ptr<BackgroundSubtractor>pKnn, pMog2;
	pKnn = createBackgroundSubtractorKNN();
	pMog2 = createBackgroundSubtractorMOG2();

	cap >> frame;
	limit_area = pickROI(frame);//[266 x 89 from (81, 64)]
	//limit_area = Rect2d(81, 64, 266, 89);
	namedWindow("findcontours");
	//namedWindow("org");
	std::vector<Rect2d> boundRect;
	int onece = 1;
	int tmp=0;

	ViBe_BGS Vibe_Bgs;
	int count = 0;
	int personNum = 0;
	int final_num = 0;

	char buffer[50];
	MultiTracker obj("KCF");
	MultiTrackerTLD tracker;
	vector<int> targeNUm(30);
	vector<int>::const_iterator itt;
	vector<Rect2d>::const_iterator itObj;
	int sum;
	bool isContinue;
	int key = waitKey(1);
	while (key != 32)
	{
		key = waitKey(10);
		if (key=='p'||key==27)
			waitKey(0);
		count++;
		cap >> frame;
		if (frame.empty())
			break;
		cvtColor(frame, gray, CV_RGB2GRAY);
		if (count<10)
			continue;
		else if (count == 10)
		{
			Vibe_Bgs.init(gray);
			Vibe_Bgs.processFirstFrame(gray);
			cout << " Training GMM complete!" << endl;
		}
		else
		{
			Vibe_Bgs.testAndUpdate(gray);
			mask = Vibe_Bgs.getMask();
			cvtColor(mask, mask_color, CV_GRAY2RGB);
			Mat kernal = getStructuringElement(MORPH_RECT, Size(2, 5), Point(1, 2));
			//morphologyEx(mask, mask, MORPH_OPEN, Mat());
			morphologyEx(mask, mask, MORPH_CLOSE, kernal);
			imshow("mask", mask);
			boundRect = findmaxcontour(mask,4,9);
			if (targeNUm.size()>29)
			{
				itt = targeNUm.begin();
				targeNUm.erase(itt);
			}
			targeNUm.push_back(boundRect.size());
			if (targeNUm.size()>29)
			{
				itt = targeNUm.end();
				//add person
				if (boundRect.size()>personNum)
				{
					isContinue = true;
					for (int i = 0; i < 2; i++)
					{
						if (*(--itt)<=personNum)
						{
							isContinue = false;
							break;
						}			
					}
					if (isContinue)
					{
						personNum = boundRect.size();
						while (obj.objects.size())
							obj.del(0);
						obj.add(track_target, boundRect);
						final_num = personNum;
						for (int i = 0; i < personNum; i++)
						{
							if (boundRect[i].width > 12 && boundRect[i].width < 20&&boundRect[i].height>21)
								final_num++;
						}
					}
				}
				//reduce person
				else if (boundRect.size()<personNum)
				{
					isContinue = true;
					sum = 0;
					for (int i = 0; i < 30; i++)
					{
						if (*(--itt)>=personNum)
						{
							isContinue = false;
							break;
						}
						else
						{
							sum += boundRect.size();
						}
					}
					if (isContinue)
					{
						personNum = round(sum / 30);
						while (obj.objects.size())
							obj.del(0);
						if (boundRect.size()>0)
							obj.add(track_target, boundRect);
						final_num = personNum;
					}
				}
			}
		}

		//if (onece)
		//	if (boundRect.size() !=personNum)
		//	{
		//		tracker.addTarget(track_target, boundRect[0], "KCF");
		//		obj.add(track_target, boundRect);
		//		onece = 0;
		//		continue;
		//	}
		//if (!onece)
		//{
		//	tracker.update(track_target);
		//	obj.update(track_target);
		//	if (boundRect.size()>personNum)
		//	{
		//		if (abs(boundRect[0].x-obj.objects[obj.objects.size()-1].x)>20)
		//		{
		//			obj.add(track_target,boundRect);
		//			//vector<Rect2d>::const_iterator itb = obj.objects.begin();
		//			//itb = obj.objects.erase(itb);
		//		}
		//		//if (abs(boundRect[0].x-tracker.boundingBoxes[tracker.boundingBoxes.size()-1].x)>20)
		//		//{
		//		//	tracker.addTarget(track_target, boundRect[boundRect.size()-1], "KCF");
		//		//	//vector<Rect2d>::const_iterator itb = obj.objects.begin();
		//		//	//itb = obj.objects.erase(itb);
		//		//	cout << tracker.targetNum << endl;
		//		//}

		//	}
		//		//rectangle(frame_color, obj.objects[obj.objects.size()-1], Scalar(255, 0, 0), 1, 8);
		//		//imshow("dst", frame_color);
		//	//for (int i = 0; i < boundRect.size(); i++)
		//	//{
		//	//	rectangle(frame, boundRect[i], Scalar(0, 255, 0), 1, 8);
		//	//}
		//	for (int i = 0; i < obj.objects.size(); i++)
		//	{
		//		rectangle(frame, obj.objects[i], Scalar(255, 0, 0), 1, 8);
		//	}
		//	//for (int i = 0; i < personNum; i++)
		//	//{
		//	//	rectangle(frame, tracker.boundingBoxes[tracker.boundingBoxes.size()-1-i], Scalar(255, 0, 0), 1, 8);
		//	//}
		//}
		obj.update(track_target);
		for (int i = 0; i < obj.objects.size(); i++)
		{
			rectangle(frame, obj.objects[obj.objects.size()-1-i], Scalar(255, 0, 0), 1, 8);
		}
		
		sprintf_s(buffer, "%.1d", final_num);
		putText(frame, buffer, Point(25, 25), FONT_HERSHEY_SCRIPT_SIMPLEX,
			1, Scalar::all(255), 1, 8, false);
		rectangle(frame, limit_area, Scalar(0, 128, 0), 1, 4);
		imshow("input", frame);
		cout << "poly " << boundRect.size() << " track:" << obj.objects.size() << endl;

		//if (waitKey(1)==32)
		//	break;
	}

	//while (true)
	//{
	//	count++;
	//	if (waitKey(10)==27)
	//		break;
	//	cap >> frame;
	//	if (frame.empty())
	//		break;
	//	//pyrDown(frame, frame, Size(), BORDER_DEFAULT);
	//	frame_color = frame.clone();
	//	//threshold(frame, frame, 100, 255,THRESH_TOZERO);
	//	imshow("org", frame);
	//	pKnn->apply(frame, fgKnn, -0.5);
	//	cvtColor(fgKnn, fgKnn_color, CV_GRAY2BGR);
	//	Mat kernal = getStructuringElement(MORPH_RECT, Size(3, 5), Point(1, 2));
	//	//morphologyEx(mask, mask, MORPH_OPEN, Mat());
	//	morphologyEx(fgKnn_color, fgKnn_color, MORPH_CLOSE, kernal);
	//	//pMog2->apply(frame, fgMog2, -0.1);
	//	imshow("Knn", fgKnn_color);
	//	//imshow("Mog2", fgMog2);
	//	boundRect = findmaxcontour(fgKnn);
	//	if (count<20)
	//		continue;
	//	if (onece)
	//		if (boundRect.size() ==1)
	//		{
	//			tracker.addTarget(fgKnn_color, boundRect[0], "KCF");
	//			obj.add(fgKnn_color, boundRect);
	//			onece = 0;
	//		}
	//	if (!onece)
	//	{
	//		tracker.update(fgKnn_color);
	//		obj.update(fgKnn_color);
	//		if (boundRect.size()==1)
	//		{
	//			if (abs(boundRect[0].x-obj.objects[obj.objects.size()-1].x)>20)
	//			{
	//				obj.add(fgKnn_color, boundRect);
	//				//vector<Rect2d>::const_iterator itb = obj.objects.begin();
	//				//itb = obj.objects.erase(itb);
	//			}
	//		}
	//			//rectangle(frame_color, obj.objects[obj.objects.size()-1], Scalar(255, 0, 0), 1, 8);
	//			//imshow("dst", frame_color);
	//		for (int i = 0; i < obj.objects.size(); i++)
	//		{
	//			rectangle(frame_color, obj.objects[i], Scalar(255, 0, 0), 1, 8);
	//			imshow("dst", frame_color);
	//		}
	//		//rectangle(frame_color, tracker.boundingBoxes[0], Scalar(255, 0, 0), 1, 8);
	//		//imshow("dst", frame_color);
	//	}
	//}

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


std::vector<Rect2d> findmaxcontour(Mat &_src,int minWidth,int minHeight)
{
	vector<vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;
	vector<cv::Point> contourmax;

	Mat src;
	src = _src.clone();
	Mat blocking_mask = Mat::zeros(src.size(), CV_8UC1);
	rectangle(blocking_mask, limit_area, Scalar::all(255), -1);
	src &= blocking_mask;

	//rectangle(src, Rect(0, src.rows / 3 * 2, src.cols, src.rows / 3), Scalar::all(0), -1);
	blur(src, src, Size(3, 5));
	/// Detect edges using Threshold
	threshold(src, src, 60, 255, THRESH_BINARY);
		
	dst = src.clone();
	cvtColor(dst, dst, CV_GRAY2BGR);
	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//findContours(src, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//src = src > 1;
	//int area, maxArea = 1;
	vector<vector<cv::Point>>::const_iterator itc = contours.begin();
	//while (itc!=contours.end())
	//{
	//	if (itc->size() > 100 || itc->size() < 10)
	//		itc = contours.erase(itc);
	//	else
	//		++itc;
	//}
	vector<vector<cv::Point>> contours_poly(contours.size());
	vector<Rect2d> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());

	for (int i = 0; i < (int)contours.size(); i++)
	{
		approxPolyDP(contours[i], contours_poly[i], 3, true);
		boundRect[i] = boundingRect(contours_poly[i]);
		minEnclosingCircle(contours_poly[i], center[i], radius[i]);
	}
	vector<vector<cv::Point>>::const_iterator itpoly = contours_poly.begin();
	vector<Rect2d>::const_iterator itb = boundRect.begin();
	vector<Point2f>::const_iterator itp = center.begin();
	vector<float>::const_iterator itr = radius.begin();

	while (itb != boundRect.end())
	{
		if (itb->width<minWidth || itb->height<minHeight||(itb->width<50&&itb->width>20))
		{
			itb = boundRect.erase(itb);
			itpoly = contours_poly.erase(itpoly);
			itp = center.erase(itp);
			itr = radius.erase(itr);
		}
		else
		{
			++itb;
			++itpoly;
			++itp;
			++itr;
		}
	}

	for (int i = 0; i< contours_poly.size(); i++)
	{
		Scalar color(rand() % 255, rand() % 255, rand() % 255);
		//drawContours(dst, contours, i, Scalar::all(0), CV_FILLED, 8, vector<Vec4i>(), 0, Point());
		drawContours(dst, contours_poly, i, Scalar(128, 128, 0), 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(dst, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 1, 8, 0);
		circle(dst, center[i], (int)radius[i], Scalar(0, 0, 200), 1, 8, 0);
		//drawContours(dst, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}

	//find the max area
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
	//cout << " contour_poly_size: "<<contours_poly.size()<< endl;

	//drawContours(dst, contours,idx,Scalar(255,0,0),2,8,noArray(),10,Point(0,0));
	imshow("findcontours", dst);
	return boundRect;
}