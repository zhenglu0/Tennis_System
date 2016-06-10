#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include "math.h"


int main( int argc, char** argv )
{
	IplImage* image = cvLoadImage("tennis_shape2.jpg",1);


	cvNamedWindow("original",1);

	cvShowImage("original",image);

	CvSize size = cvGetSize(image);

	IplImage* gray;

	gray =  cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);

	cvCvtColor(image, gray, CV_BGR2GRAY);

	cvNamedWindow("gray",1);

	cvShowImage("gray",gray);

	IplImage* pCannyImg; 

	IplImage* hough_in; 

	IplImage* pCanny; 

	pCannyImg = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);

	hough_in =  cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);

	pCanny = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);

	cvCanny(gray, pCannyImg, 50, 150, 3);

	cvCvtColor( pCannyImg, pCanny, CV_GRAY2BGR );

	CvMemStorage* storage = cvCreateMemStorage(0);

	cvSmooth (pCannyImg,hough_in,CV_GAUSSIAN,5,5);

	CvSeq* results = cvHoughCircles(hough_in,storage,CV_HOUGH_GRADIENT,2,image->width/2);

	for(int i = 0;i < results->total; i++)
	{
	float* p = (float*)cvGetSeqElem(results,i);

	CvPoint pt = cvPoint(cvRound(p[0]),cvRound(p[1]));

	cvCircle(image,pt,cvRound(p[2]),CV_RGB(255,0,0),4,8,0);

	cvCircle(pCanny,pt,cvRound(p[2]),CV_RGB(255,0,0),4,8,0);

	cvNamedWindow("cvHoughCircle",1);

	cvShowImage("cvHoughCircle",image);

	cvNamedWindow("pCannyImg",1);

	cvShowImage("pCannyImg",pCanny);

	cvWaitKey(0);
	
	}

}