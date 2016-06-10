#include <stdio.h>
#include <math.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define MAXSIZE 1000


//载入视频名称
const char* videoName = "MVI_8834.AVI";


//显示的视频名称
const char*  video = "video    ";

//计算字符的长度
int s1;
int s2;


//声明IplImage指针
IplImage* pFrame = NULL;	//frame       
IplImage* pFrImg = NULL;	//forground   
IplImage* pBkImg = NULL;	//background  
IplImage* pEqlImg = NULL;	//equalized
IplImage* pBinImg = NULL;	//binary
IplImage* pTrpImg = NULL;   //point tracking
IplImage* pTrjImg = NULL;   //trajectory tracking
IplImage* pHough  = NULL;   //the image after hough transformation
IplImage* pCanny = NULL;    //the image after canny 
IplImage* pJudge = NULL;    //the image used for display judging
IplImage* pOper = NULL;    //the image after human operation


//声明CvMat矩阵，cvRunningAvg需要使用矩阵
CvMat* pFrameMat = NULL;
CvMat* pFrMat = NULL;
CvMat* pBkMat = NULL;
//用于实现Surendra算法，用来保存变量
CvMat* temp = NULL;
CvMat* Dxy = NULL;

//声明读取视频需要用的指针
CvCapture* pCapture = NULL;


//初始化轮廓
CvSeq* contour = 0;
CvSeq* lines = 0;


//矩形类型
CvRect r;


//形态学用的参数
IplConvKernel* element;


//声明给出判定是需要用到的字体
CvFont font;


//定义字体的大小
CvSize text_size1;
CvSize text_size2;

//定义点的数据结构
CvPoint pt1;
CvPoint pt2;



//程序中会用到的整型变量
int nFrmNum = 0;
int i = 0;
int k = 0;
int m = 0;
int ymin = 0;
int down = 0;


//定义识别网球线需要用的点
int Xa;
int Ya;
int Xb;
int Yb;


//记录球落点的帧号码
int bounceFrmNum[MAXSIZE];


//球心坐标
int Xo;
int Yo;

//跟踪中球心坐标数组
int centerX[MAXSIZE];
int centerY[MAXSIZE];

//在视频中实现暂停按钮值
int g_switch_value = 1;


//二维数组用于存储hough变换后发现的可能的直线上的点
int point[MAXSIZE][2];


//用来保存判定球在指定的场地线以内还是以外
bool judging;


//用来计算球在指定的场地线以内还是以外
int fx;


//用来记录计算出的球的落点的数组
int Xg[MAXSIZE]; //g means ground
int Yg[MAXSIZE];


//用来记录直线方程交点的变量
int Xs;    //s means intersect
int Ys;


//用来保存状态的一个bool变量
bool lastGrounded;//用来表示上一次是否球可能落地


//用来保存第三种情况的球的落点
int xi0;
int yi0;
int xi1;
int yi1;
int xi2;
int yi2;
int xi3;
int yi3;



