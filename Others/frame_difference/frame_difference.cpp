
#include "stdafx.h"
#include <stdio.h>
#include "cv.h"
#include "highgui.h"



int main(int argc, char* argv[])
{
	IplImage *m_pPreImage = NULL;
	IplImage *m_pGrayImage = NULL;
	IplImage *m_pSmoothImage = NULL;
	IplImage *pPrev = NULL;
	IplImage *pCurr = NULL;
	IplImage *pDest = NULL;
	IplImage *pMask = NULL;
	IplImage *pMaskDest = NULL;
	IplImage *dst = NULL;
	CvMat *pPrevF = NULL;
	CvMat *pCurrF = NULL;
	CvSize imgSize;

    CvCapture *m_pCapture = NULL;
	CvVideoWriter *writer = 0;
	IplConvKernel* element;
	CvSeq* contour = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvRect r;

	// IplConvKernel* element;

    cvNamedWindow( "VideoDisplay1", 1 );
	cvNamedWindow( "VideoDisplay2", 1 );
	cvNamedWindow( "VideoDisplay3", 1 );
	cvNamedWindow( "VideoDisplay4", 1 );
	
// Capture
	m_pCapture = cvCreateFileCapture("MVI_8833.AVI");
	contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),storage);
	

    if( !m_pCapture )
    {
        fprintf(stderr,"Could not initialize capturing! \n");
        return -1;
    }
// Display
    while ( (m_pPreImage = cvQueryFrame(m_pCapture)))
    {	
		imgSize = cvSize(m_pPreImage->width, m_pPreImage->height);
		if(!m_pGrayImage)
			m_pGrayImage = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
		if(!pCurr)
			pCurr = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);	
		if(!m_pSmoothImage)
			m_pSmoothImage = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);

		//图像预处理
		cvCvtColor(m_pPreImage, m_pGrayImage, CV_BGR2GRAY);//转化为灰度图像
		cvSmooth(m_pGrayImage,m_pSmoothImage,CV_GAUSSIAN,3,0,0,0 );//GAUSSIAN平滑去噪声
		cvEqualizeHist(m_pSmoothImage,pCurr );//直方图均衡


		 if(!pPrevF)
			pPrevF = cvCreateMat(m_pGrayImage->width,m_pPreImage->height, CV_32FC1);
		 if(!pCurrF)
			pCurrF = cvCreateMat(m_pGrayImage->width,m_pPreImage->height, CV_32FC1);
		 if(!pPrev)
			pPrev = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
		 if(!pMask)
			pMask = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
		 if(!pMaskDest)
			pMaskDest = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
		 if(!dst)
			dst = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
		 if(!pDest)
			{
				pDest = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
				
			}
	
		cvAbsDiff(pPrev, pCurr, pDest);   //帧差
		cvCopy(pCurr, pPrev, NULL);  // 当前帧存入前一帧

		
		cvThreshold(pDest, pMask, 80, 255, CV_THRESH_BINARY);     // 二值化
		element = cvCreateStructuringElementEx( 9, 9, 3, 3, CV_SHAPE_RECT, NULL);
		cvMorphologyEx( pMask, pMaskDest, NULL, element, CV_MOP_CLOSE, 1);//形态学处理
		
		//查找并且画出团块轮廓
		cvFindContours( pMaskDest, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

		//画出包含目标的最小矩形
		for(;contour;contour=contour->h_next)
		{
			r=((CvContour*)contour)->rect;
			if(r.height*r.width>100)
			{
				cvRectangle(m_pPreImage,cvPoint(r.x,r.y),cvPoint(r.x+r.width,r.y+r.height),CV_RGB(255,0,0),1,CV_AA,0);
				
			}
		}


		cvShowImage( "VideoDisplay1", m_pPreImage );
		cvShowImage( "VideoDisplay2", pMask);
		cvShowImage( "VideoDisplay3", pMaskDest );
		cvShowImage( "VideoDisplay4", pPrev );

		if(cvWaitKey(50)>0)
			return 0;
	}

	// Realease
    cvReleaseImage( &m_pPreImage );
	cvReleaseImage( &m_pGrayImage );
	cvReleaseImage( &m_pSmoothImage );
	cvReleaseImage( &pCurr );
	cvReleaseImage( &pDest );
	cvReleaseImage( &pMask );
	cvReleaseImage( &pMaskDest );
	cvReleaseImage( &dst );
	cvReleaseMemStorage( &storage );
    cvDestroyWindow("VideoDisplay1");
	cvDestroyWindow("VideoDisplay2");
	cvDestroyWindow("VideoDisplay3");
	cvDestroyWindow("VideoDisplay4");
	cvReleaseStructuringElement( &element ); 

	return 0;
}