#pragma once
using namespace cv;
using namespace std;

class ROIPick
{
public:
	ROIPick();
	~ROIPick();
	Rect2d select(Mat img);
	Rect2d select(const cv::String& windowName, Mat img);
	void select(const cv::String& windowName, Mat img,
		std::vector<Rect2d>& boundlingBox);
	struct handlerT{
		bool isDrawing;
		Rect2d box;
		Mat image;
		handlerT() :isDrawing(false){};
	}selctorParams;
	std::vector<Rect2d>objects;
private:
	static void mouseHandler(int event, int x, int y,
		int flags, void *param);
	void opencv_mouse_callback(int event, int x, int y,
		int, void *param);
	int key;
};

Rect2d CV_EXPORTS_W pickROI(Mat img);
Rect2d CV_EXPORTS_W pickROI(const cv::String& windowName,
	Mat img);
void CV_EXPORTS_W pickROI(const cv::String& windowName,
	Mat img, std::vector<Rect2d>& boundlingBox);


