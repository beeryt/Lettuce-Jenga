#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <stdio.h>
#include <string>
#include <algorithm> // max_element
using namespace std;
using namespace cv;

#include "JengaTower.h"

const string win1 = "Edge Detect";
const string win2 = "Contours";
const string win3 = "Lines";
const string win4 = "Mask";

int edg_thresh = 32;
int max_edg_thresh = 100;
int edg_ratio = 30;
int max_edg_ratio = 50;
int edg_blur = 2;
int max_edg_blur = 10;

int cont_dilate = 2;
int max_cont_dilate = 10;

int msk_thresh = 30;
int max_msk_thresh = 100;
int msk_ratio = 30;
int max_msk_ratio = 50;
int msk_blur = 10;
int max_msk_blur = 10;

int max_dot_thresh = 100;
int dot_thresh = max_dot_thresh/2;

Mat src_orig, src_gray, src_mask, src_edge, src_contours;

void MaskTower(Mat src, Mat& dst);
void DetectEdge(Mat src, Mat& dst);
void FindContours(Mat src, Mat& dst);
void FindCorners(Mat src, Mat& dst);
void ContourCenters(Mat src, Mat& dst);

static void TrackBarCallback(int,void*)
{
	MaskTower(src_gray, src_mask);
	DetectEdge(src_mask, src_edge);
	FindContours(src_edge, src_contours);
	// FindCorners(src_edge, src_mask);
	ContourCenters(src_edge, src_mask);

	imshow(win1, src_edge);
	imshow(win2, src_contours);
	imshow(win3, src_gray);
	imshow(win4, src_mask);
}

void savevars()
{
	FILE* fp = fopen("vars.cfg", "w");
	if (fp == NULL)
	{
		perror("savevars");
		return;
	}
	fprintf(fp, "%d\n", edg_thresh);
	fprintf(fp, "%d\n", edg_ratio);
	fprintf(fp, "%d\n", edg_blur);
	fprintf(fp, "%d\n", cont_dilate);
	fprintf(fp, "%d\n", dot_thresh);
	fprintf(fp, "%d\n", msk_thresh);
	fprintf(fp, "%d\n", msk_ratio);
	fprintf(fp, "%d\n", msk_blur);
	fclose(fp);
	printf("Param's saved\n");
}

void loadvars()
{
	FILE* fp = fopen("vars.cfg", "r");
	if (fp == NULL)
	{
		perror("loadvars");
		return;
	}
	fscanf(fp, "%d", &edg_thresh);
	fscanf(fp, "%d", &edg_ratio);
	fscanf(fp, "%d", &edg_blur);
	fscanf(fp, "%d", &cont_dilate);
	fscanf(fp, "%d", &dot_thresh);
	fscanf(fp, "%d", &msk_thresh);
	fscanf(fp, "%d", &msk_ratio);
	fscanf(fp, "%d", &msk_blur);
	fclose(fp);
}

void loadimg(int &i)
{
	if (i == -1) 
	{
		i++;
		return;
	}
	char file[200];
	sprintf(file, "../../imgs/phone/j%d.jpg", i);
	printf("Attempting to open file: %s\n", file);

	Mat img = imread(file, IMREAD_COLOR);
	if (img.empty())
	{
		i--;
		return;
	}

	double ratio = ((double)img.rows)/img.cols;
	resize(img, src_orig, Size(340, 340*ratio));
	cvtColor(src_orig, src_gray, COLOR_BGR2GRAY);

	TrackBarCallback(0,0);
}

int main(int argc, char const *argv[])
{
	namedWindow(win1, WINDOW_AUTOSIZE);
	namedWindow(win2, WINDOW_AUTOSIZE);
	namedWindow(win3, WINDOW_AUTOSIZE);
	namedWindow(win4, WINDOW_AUTOSIZE);

	moveWindow(win1, 340*0, 0);
	moveWindow(win2, 340*1, 0);
	moveWindow(win3, 340*2, 0);
	moveWindow(win4, 340*3, 0);

	createTrackbar("Threshold:", win1, &edg_thresh, max_edg_thresh, TrackBarCallback);
	createTrackbar("Ratio:", win1, &edg_ratio, max_edg_ratio, TrackBarCallback);
	createTrackbar("Blur:", win1, &edg_blur,  max_edg_blur, TrackBarCallback);

	createTrackbar("Dilation:", win2, &cont_dilate, max_cont_dilate, TrackBarCallback);

	createTrackbar("Threshold", win3, &dot_thresh, max_dot_thresh, TrackBarCallback);

	createTrackbar("Threshold:", win4, &msk_thresh, max_msk_thresh, TrackBarCallback);
	createTrackbar("Ratio:", win4, &msk_ratio, max_msk_ratio, TrackBarCallback);
	createTrackbar("Blur:", win4, &msk_blur, max_msk_blur, TrackBarCallback);

	src_orig = imread(argv[1], IMREAD_COLOR);
	if (src_orig.empty()) return -1;
	double ratio = ((double)src_orig.rows)/src_orig.cols;
	resize(src_orig, src_orig, Size(340, 340*ratio));

	cvtColor(src_orig, src_gray, COLOR_BGR2GRAY);

	TrackBarCallback(0,0);

	int i = 0;

	char k;
	while (k = waitKey(200))
	{
		switch (k)
		{
			case -1: // Tab and timeout
			case -23: continue;

			case 'a': loadimg(--i);
			break;
			case 'd': loadimg(++i);
			break;

			case 'l': loadvars();
				TrackBarCallback(0,0);
			break;
			case 's': savevars();
			break;

			default: printf("Key Pressed (%d '%c')! Exiting.\n", (int)k, k);
			destroyAllWindows();
			return 0;
		}
	}
}

static bool cmp_contour(vector<Point> a, vector<Point> b)
{
	return contourArea(a) < contourArea(b);
}

void MaskTower(Mat src, Mat& dst)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3), Point(1,1));

	int blur = 2*msk_blur + 1;
	GaussianBlur(src, dst, Size(blur,blur), 0, 0);

	int ratio = msk_ratio / max_msk_ratio;
	Canny(dst,dst,msk_thresh,ratio*msk_thresh);
	dilate(dst,dst,kernel);
	findContours(dst,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE,Point(0,0));

	vector<vector<Point> >::iterator max_iter;
	max_iter = max_element(contours.begin(), contours.end(), cmp_contour);

	vector<Point> max_contour = contours[max_iter-contours.begin()];
	Mat img(src.size(), CV_8UC3);
	src.copyTo(img);
	cvtColor(img,img,CV_GRAY2BGR);
	drawContours(img,contours,max_iter-contours.begin(), Scalar(128,255,0), 5);
	Rect b = boundingRect(max_contour);
	// rectangle(img,b,Scalar(0xBE,0xDF,0xAD), 1);
	int dy = b.height / 18.0;
	int dx = b.width / 3.0;
	for (int i = 0; i < 18; ++i)
	{
		if (i % 2)
		{
			Point tl(b.x, b.y + (dy*i));
			Point br(b.x + b.width, tl.y + dy);
			Scalar c(255,0,128);
			int t = 2;
			rectangle(img,tl,br,Scalar(255,128,0), t);
			line(img,tl,br,c,t);
			line(img,Point(br.x, tl.y), Point(tl.x, tl.y+dy), c, t);
			continue;
		}
		for (int j = 0; j < 3; ++j)
		{
			Point tl(b.x + (dx*j), b.y + (dy*i));
			Point br(tl.x + dx, tl.y + dy);
			rectangle(img,tl,br, Scalar(255,128,0), 2);
		}
	}
	// for (int i = 0; i < 3+1; ++i)
	// {
	// 	Point top(b.x + (dx*i), b.y);
	// 	Point bot(b.x + (dx*i), b.y + b.height);
	// 	line(img,top,bot, Scalar(240,0,0), 1);
	// }
	// for (int i = 0; i < 18+1; ++i)
	// {
	// 	Point lt(b.x, b.y + (dy*i));
	// 	Point rt(b.x + b.width, b.y + (dy*i));
	// 	line(img,lt,rt,Scalar(240,0,0), 1);
	// }
	imshow("Mask Contour", img);

	Mat maskInsideContour = Mat::zeros(dst.size(), CV_8UC1);
	drawContours(maskInsideContour, contours, max_iter-contours.begin(), Scalar(255), CV_FILLED);
	Mat maskedImage = Mat(dst.size(), CV_8UC3);
	maskedImage.setTo(Scalar(0));
	src.copyTo(maskedImage, maskInsideContour);

	maskedImage.copyTo(dst);

}

void DetectEdge(Mat src, Mat& dst)
{
	int blur = 2*edg_blur+1;
	int ratio = edg_ratio / max_edg_ratio;

	GaussianBlur(src, dst, Size(blur,blur), 0, 0);
	Canny(dst,dst,edg_thresh,ratio*edg_thresh);

}

void FindContours(Mat src, Mat& dst)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Size ksize(2*cont_dilate+1, 2*cont_dilate+1);
	Point kpoint(cont_dilate, cont_dilate);
	Mat kernel = getStructuringElement(MORPH_RECT, ksize, kpoint);

	dilate(src,dst,kernel);
	findContours(dst, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

	RNG rng(123456);
	Mat img = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); ++i)
	{
		Scalar color(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
		drawContours(img, contours, i, color, 2, 8, hierarchy, 0, Point());
	}
	dst = img;
}

void FindCorners(Mat src, Mat& dst)
{
	Mat dst_scaled;
	dst = Mat::zeros(src.size(), CV_32FC1);
	Mat img = src.clone();
	if (img.type() != CV_8UC1)
		cvtColor(src,img,COLOR_BGR2GRAY);

	// Detector parameters
	int blockSize = 3;
	int apertureSize = 3;
	double k = 0.01;
	cornerHarris(img, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

	// Normalizing
	normalize(dst, dst, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst,dst_scaled);

	for (int j = 0; j < dst.rows; ++j)
		for (int i = 0; i < dst.cols; ++i)
			if ((int)dst.at<float>(j,i) > msk_thresh)
				circle(dst_scaled, Point(i,j), 2, Scalar(0), 1, 8, 0);

	dst_scaled.copyTo(dst);
}

void ContourCenters(Mat src, Mat& dst)
{
	if (dst.empty())
		dst = Mat::zeros(src.size(), CV_8UC3);
	else if (dst.type() == CV_8UC1)
		cvtColor(dst,dst,CV_GRAY2BGR);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Size ksize(2*cont_dilate+1, 2*cont_dilate+1);
	Point kpoint(cont_dilate, cont_dilate);
	Mat kernel = getStructuringElement(MORPH_RECT, ksize, kpoint);

	Mat img;
	dilate(src,img,kernel);
	findContours(img, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

	vector<vector<Point> >::iterator it;
	it = max_element(contours.begin(), contours.end(), cmp_contour);
	// vector<Point> max = contours[it - contours.begin()];

	contours.erase(it);

	double avg_area = 0;
	for (it = contours.begin(); it != contours.end(); ++it)
		avg_area += contourArea(contours[it - contours.begin()]);
	avg_area /= contours.size();

	double thresh = avg_area * (1 + (double)(dot_thresh-(max_dot_thresh/2))/20);

	RNG rng(123456);
	for (it = contours.begin(); it != contours.end(); ++it)
	{
		vector<Point> c = contours[it - contours.begin()];
		Scalar color(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
		Moments M = moments(c);

		if (contourArea(c) < thresh)
			continue;

		circle(dst, Point(M.m10/M.m00, M.m01/M.m00), 3, color, 5);
	}
}