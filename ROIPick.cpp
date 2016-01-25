#include "stdafx.h"
#include "ROIPick.h"

ROIPick::ROIPick()
{
}


ROIPick::~ROIPick()
{
}

void ROIPick::mouseHandler(int event, int x, int y, int flags, void *param){
	ROIPick *self = static_cast<ROIPick*>(param);
	self->opencv_mouse_callback(event, x, y, flags, param);
}

void ROIPick::opencv_mouse_callback(int event, int x, int y, int, void *param){
	handlerT* data = (handlerT*)param;
	switch (event)
	{
	case EVENT_MOUSEMOVE:
		if (data->isDrawing)
		{
			data->box.width = x - data->box.x;
			data->box.height = y - data->box.y;
		}
		break;
	case EVENT_LBUTTONDOWN:
		data->isDrawing = true;
		data->box = Rect(x, y, 0, 0);
		break;
	case EVENT_LBUTTONUP:
		data->isDrawing = false;
		if (data->box.width<0)
		{
			data->box.x += data->box.width;
			data->box.width *= -1;
		}
		if (data->box.height<0)
		{
			data->box.y += data->box.height;
			data->box.y *= -1;
		}
		break;
	default:
		break;
	}
}

Rect2d ROIPick::select(Mat img){
	return select("Draw a roi", img);
}

Rect2d ROIPick::select(const cv::String& windowName, Mat img){
	key = 0;
	imshow(windowName, img);
	selctorParams.image = img.clone();
	setMouseCallback(windowName, mouseHandler, (void *)&selctorParams);
	while (!(key==8||key==32||key==13||key==27))
	{
		rectangle(selctorParams.image, selctorParams.box, Scalar(255, 0, 0), 1, 8, 0);
		imshow(windowName, selctorParams.image);
		selctorParams.image = img.clone();
		key = waitKey(1);
	}
#if DEBUG
	if (!(key==8||key==27))
		cout << "select a object,Done!" << endl;
#endif
	destroyWindow(windowName);
	return selctorParams.box;
}

void ROIPick::select(const cv::String& windowName, Mat img,
	std::vector<Rect2d>& boundingBox){
	std::vector<Rect2d> objBox;
	Rect2d temp;
	key = 0;
	cout << "select an object and press SPACE of ENTER" << endl
		<< "press BACKSPACE to finish" << endl;
	while (key!=8)
	{
		temp = select(windowName, img);
		if (temp.width>0&&temp.height>0)
		{
			objBox.push_back(temp);
		}
		boundingBox = objBox;
	}
}

ROIPick _picker;
Rect2d pickROI(Mat img){
	return _picker.select("Draw a roi", img);
}

Rect2d pickROI(const cv::String& windowName, Mat img){
	cout << "select a object an press ENTER or SPACE" << endl;
	return _picker.select(windowName, img);
}

void pickROI(const cv::String& windowName, Mat img, std::vector<Rect2d>& boundingBox){
	return _picker.select(windowName, img, boundingBox);
}