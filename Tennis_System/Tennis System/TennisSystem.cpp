#include "stdafx.h"
#include "TennisSystem.h"
#include "Interaction.h"
#include "Calculation.h"


// main函数入口

int main( int argc, char** argv )
{
	//合并视频文件和video字符，用于显示在窗口的左上角
	s1 = strlen(video); 
	s2 = strlen(videoName); 
	char*  displayName= new char[s1+s2];

	memset(displayName,0,s1+s2);
	memcpy(displayName,video,s1);
	memcpy(displayName+s1,videoName,s2); 

	//申请分配内存
	CvMemStorage* storageContour = cvCreateMemStorage(0);
	CvMemStorage* storageHough = cvCreateMemStorage(0);

	//创建窗口
	cvNamedWindow("background",1);
	cvNamedWindow("binary",1);
	cvNamedWindow("background model",1);
	cvNamedWindow("point tracking",1);
	cvNamedWindow(displayName, 1);
	cvNamedWindow("trajectory tracking",1);
	cvNamedWindow("judging display",1);

	//调节窗口大小
	cvResizeWindow("judging display",250,200);

	//创建trackbar，用来实现开关
	cvCreateTrackbar("Pause/Run",displayName, &g_switch_value, 1, switch_callback );

	//创建鼠标的回调函数
	cvSetMouseCallback("trajectory tracking",mouse_callback1 ,0);
	cvSetMouseCallback("point tracking",mouse_callback2 ,0);
	cvSetMouseCallback("judging display",mouse_callback3 ,0);

	//使窗口有序排列
	cvMoveWindow(displayName, 0, 0);
	cvMoveWindow("background", 0, 0);
	cvMoveWindow("binary", 0, 0);
	cvMoveWindow("background model", 0, 0);
	cvMoveWindow("point tracking", 0, 0);
	cvMoveWindow("trajectory tracking", 620, 250);
	cvMoveWindow("judging display", 800, 0);

	//初始化字体
	cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX, 3, 3, 0.0, 5, CV_AA );

	//设定字体的大小    
	cvGetTextSize( "IN", &font, &text_size1, &ymin );
	cvGetTextSize( "OUT", &font, &text_size2, &ymin );	


	//定义字体的位置
	pt1.x = (250 - text_size1.width)/2;
	pt1.y = (200 + text_size1.height)/2;

	pt2.x = (250 - text_size2.width)/2;
	pt2.y = (200 + text_size2.height)/2;


	//如果要使用摄像头，请将 if 0 改为 if 1
#if 0 
  //打开摄像头
    if( !(pCapture = cvCaptureFromCAM(-1)))
    {
		fprintf(stderr, "Can not open camera.\n");
		return -1;
    }

#else
  //打开视频文件
    if( !(pCapture = cvCaptureFromFile(videoName)))
    {
		fprintf(stderr, "Can not open video file.\n");
		return -1;
    }
#endif
	
	//初始化轮廓的序列
	contour = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),storageContour);

	//显示热键和帮助
	manual();

	//外层循环，用于开关的实现
	printf("case1 g_switch_value %d\n",g_switch_value);
      
    while(1)
	{	   
		//逐帧读取视频
		if(g_switch_value)
		{
			pFrame = cvQueryFrame( pCapture);
			//如果视频已经读完，则跳出循环
			if( !pFrame) 
				break;
			nFrmNum++;
			printf("Frame Number: %d \n",nFrmNum);
		}
		else
		{
			//需要等待用户按键,按任意键继续
			cvWaitKey(0);
			//循环体入口
			continue;
		}
		
		//如果是第一帧，需要申请内存，并将变量初始化
		if(nFrmNum == 1)
		{
			//对图像的初始化
			pBkImg	 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
			pFrImg	 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
			pEqlImg	 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
			pBinImg	 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
			pTrpImg	 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,3);
			cvZero( pTrpImg );
			pTrjImg	 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,3);
			pHough	 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,3);
			pCanny	 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
			pJudge   = cvCreateImage(cvSize(250, 200),  IPL_DEPTH_8U,3);
			cvZero( pJudge );	
			pOper    = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,3);

			//对矩阵的初始化，用于实现背景更新的矩阵
			pBkMat	 = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrMat	 = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pFrameMat= cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			//用于实现Surendra算法
			temp     = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			Dxy      = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

			//转化成单通道图像再处理
			cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);

			//将图像转换为矩阵
			cvConvert(pFrImg, pFrameMat);
			cvConvert(pFrImg, pFrMat);
			cvConvert(pFrImg, pBkMat);

			//对建模需要使用的背景用canny 算子
			cvCanny( pBkImg, pCanny, 50, 200, 3 );

			//转换为GRB三通道
			cvCvtColor( pCanny, pHough, CV_GRAY2BGR );

			//复制图像
			cvCopy(pHough,pOper);	

			//使用Hough线变换找出网球场线用于场景建模，找出所有可能的直线
			lines = cvHoughLines2( pCanny, storageHough, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 50, 50, 25 );

			//打印出找出的线的数目
			printf("total lines : %d\n",lines->total);

			//在场景中显示出网球球场线
			for(int j = 0; j < lines->total; j++ )
			{
				CvPoint* linePoint = (CvPoint*)cvGetSeqElem(lines,j);
				//水平方向的场地线不用显示出来，只保留竖直方向上的底线
				if( abs(linePoint[0].y - linePoint[1].y) <25 || abs(linePoint[0].x - linePoint[1].x) >25)
				{
					continue;
				}
				else
				{
					// 一般来说场地线都会在中央
					if ((linePoint[0].x > 270)&& (linePoint[0].x < 370))
					{
						cvCircle(pHough,linePoint[0],1,CV_RGB(0,0,255),3,8,0);
						//需要对场地线的边缘进行判定，这里取得右边缘进行判定，然后选择出几个点来代表直线
						point[m][0] =  linePoint[0].x;
						point[m][1] =  linePoint[0].y;
						//打印存入的场地点
						printf("point[%d][0]=%d  point[%d][1]=%d\n",m,point[m][0],m,point[m][1]);
						//对m的下标进行自增
						m++;
					}
					// 一般来说场地线都会在中央
					if ((linePoint[1].x > 270) && (linePoint[1].x < 370))
					{
						cvCircle(pHough,linePoint[1],1,CV_RGB(0,0,255),3,8,0);	
						//需要对场地线的边缘进行判定，这里取得右边缘进行判定，然后选择出几个点来代表直线
						point[m][0] =  linePoint[1].x;
						point[m][1] =  linePoint[1].y;
						//打印存入的场地点
						printf("point[%d][0]=%d  point[%d][1]=%d\n",m,point[m][0],m,point[m][1]);
						//对m的下标进行自增
						m++;
					}	
					//将直线点打印出来
					printf("%d   linePoint[0] (%d,%d) linePoint[1] (%d,%d) \n",j,linePoint[0].x,linePoint[0].y,linePoint[1].x,linePoint[1].y);	
				}//end else 
			} //end for

			//对可能的直线进行排序
			sort(point,m);
    		//对重新排序后的数组打印
    		for(int d = 0; d < m; d++)
			{
				printf("重新排序后的 point[%d][0]=%d  point[%d][1]=%d \n",d,point[d][0],d,point[d][1]);
			}
			//用找到的直线点进行赋值
			Xa = point[0][0];
			Ya = point[0][1];		
			
			//中间跳过几个点
			Xb = point[3][0];
			Yb = point[3][1];

			//在场地背景中调用cvLine函数画出场地线
			cvLine( pHough, cvPoint(Xa,Ya),cvPoint(Xb,Yb),CV_RGB(255,0,0), 5, CV_AA, 0 );		

			//复制场景图像用于建模
			cvCopy(pHough,pTrjImg);	
	
		}// end if 完成了场景建模的操作，下面开始对球进行识别和处理

		//开始对球的识别和处理，以及轨迹追踪，和落点判断
		else
		{
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
			cvConvert(pFrImg, pFrameMat);

			//高斯滤波先，以平滑图像
			cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);

			//当前帧跟背景图相减
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);

			cvConvert(pFrMat, pFrImg);

			//直方图均衡
			cvEqualizeHist(pFrImg,pEqlImg );

			//二值化前景图
			cvThreshold(pFrImg, pFrImg, 50, 255.0, CV_THRESH_BINARY);
	 
			//进行形态学滤波，去掉噪音  

			element = cvCreateStructuringElementEx( 9, 9, 3, 3, CV_SHAPE_RECT, NULL);
			cvMorphologyEx( pFrImg, pFrImg, NULL, element, CV_MOP_OPEN, 1);
			cvDilate(pFrImg,pFrImg,0,2);
		  
			//复制二值图像用于显示
			cvCopy(pFrImg,pBinImg);


			//查找并且画出团块轮廓,找出了轮廓之后进行遍历
			cvFindContours( pFrImg, storageContour, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

			//画出包含目标的最小矩形
			for(;contour;contour=contour->h_next)
			{
				r=((CvContour*)contour)->rect;

				if(r.height>20&&r.width>20)
				{	
					//在图中画出矩形框
					cvRectangle(pFrame,cvPoint(r.x,r.y),cvPoint(r.x+r.width,r.y+r.height),CV_RGB(255,0,0),1,CV_AA,0);

					//计算球心坐标
					Xo = r.x+(int)(r.width/2);
					Yo = r.y+(int)(r.height/2);

					//打印球心坐标
					printf("Center Point:(%d,%d)\n",Xo,Yo);

					//记录球心坐标
					centerX[i] = Xo;
					centerY[i] = Yo;
					printf("Center Point x[%d]y[%d]:(%d,%d)\n",i,i,centerX[i],centerY[i]);
					
					//在源图像中显示找出的球心的位置
					cvCircle(pFrame,cvPoint(Xo,Yo),1,CV_RGB(0,255,0),3,8,0);

					//在球心跟踪图中画出求的当前位置
					cvCircle(pTrpImg,cvPoint(Xo,Yo),5,CV_RGB(255,255,255),12,8,0);

					//在轨迹跟踪图中画出球的当前位置,需要进行边界判定
					if(i>0 && centerX[i-1]<610 && centerY[i-1]>20)
					{	
						//对网球的轨迹进行模拟
						cvLine(pTrjImg,cvPoint(centerX[i-1],centerY[i-1]),cvPoint(centerX[i],centerY[i]),CV_RGB(255,255,255),14,8,0);
					}
					else 
					{
						//对球心数组下标进行自增,并且跳出循环一次
						i++;
						continue;
					}
			
					//完成在视频中提取出的网球落点的提取
					if(i>1&&(centerY[i] - centerY[i-1]<0) && (centerY[i-1] - centerY[i-2]>0)&&(centerY[i-1]>100))
					{
						
						//记录下当前帧号码
						bounceFrmNum[k] = nFrmNum;
						printf("视频中球的落点是 ：x[%d]y[%d]:(%d,%d) FrmNum = %d \n",i-1,i-1,centerX[i-1],centerY[i-1],bounceFrmNum[k]);
						//进行第一种模式的判断
						//防止一个背景中有多个落点,一般来说，两个落点的间隔时间在1秒以上，保证落地点提取的精确性
						if(((k>0 && (bounceFrmNum[k]-bounceFrmNum[k-1]>30)) || k ==0)
							&& (abs(centerY[i-1] - centerY[i-2])>50) && (abs(centerY[i-1] - centerY[i])>50))

						{

							//在背景图中标示出网球的落点
							cvCircle(pTrpImg,cvPoint(centerX[i-1],centerY[i-1]),5,CV_RGB(0,255,0),12,8,0);

							//在场景图中标示出网球的落点
							cvCircle(pTrjImg,cvPoint(centerX[i-1],centerY[i-1]),2,CV_RGB(0,255,0),12,8,0);

							//打印出落点
							printf("用于判断的球的落点是 ：x[%d]y[%d]:(%d,%d) FrmNum = %d \n",i-1,i-1,centerX[i-1],centerY[i-1],bounceFrmNum[k]);
							
							//调用是否在线外的函数进行判断，这里采用的有向线段的计算方式
							judge( Xa, Ya, Xb, Yb, centerX[i-1], centerY[i-1]);


							//在judging display窗口中显示判决
							if( judging == true )
							{
								//首先清除原来的字体
								cvZero( pJudge );
								//显示字体
								cvPutText( pJudge, "IN", pt1, &font, CV_RGB(255,255,255));	
							}
							else
							{    
								//首先清除原来的字体
								cvZero( pJudge );	
								//显示字体
								cvPutText( pJudge, "OUT", pt2, &font, CV_RGB(255,255,255));							
							}
						}//end if 完成 第一种落地模式的判断

						//进行第二种模式的判断
						//防止一个背景中有多个落点,一般来说，两个落点的间隔时间在1秒以上，保证落地点提取的精确性
						if(((k>0 && (bounceFrmNum[k]-bounceFrmNum[k-1]>30)) || k ==0)
							&& ((abs(centerY[i-1] - centerY[i-2])<50) || (abs(centerY[i-1] - centerY[i])<50))
							&&((abs(centerY[i] - centerY[i-1])-abs(centerY[i-1] - centerY[i-2]))>0)
							&& (i>2))

						{
						
							//求出直线方程的解，求出Xs和Ys
							intersect(centerX[i],centerY[i],centerX[i-1],centerY[i-1],centerX[i-2],centerY[i-2],centerX[i-3],centerY[i-3]);
							
							//画出连接线
							cvLine(pTrjImg,cvPoint(centerX[i-1],centerY[i-1]),cvPoint(Xs,Ys),CV_RGB(255,255,0),14,8,0);
							cvLine(pTrjImg,cvPoint(centerX[i-2],centerY[i-2]),cvPoint(Xs,Ys),CV_RGB(255,255,0),14,8,0);

							//在背景图中标示出视频中的网球的落点
							cvCircle(pTrpImg,cvPoint(centerX[i-1],centerY[i-1]),5,CV_RGB(0,0,255),12,8,0);
							//在背景图中标示出计算出的网球的落点，绿色表示
							cvCircle(pTrpImg,cvPoint(Xs,Ys),5,CV_RGB(0,255,0),12,8,0);

							//在场景图中标示出网球的落点
							cvCircle(pTrjImg,cvPoint(centerX[i-1],centerY[i-1]),2,CV_RGB(0,0,255),8,8,0);
							//在背景图中标示出计算出的网球的落点，绿色表示
							cvCircle(pTrjImg,cvPoint(Xs,Ys),2,CV_RGB(0,255,0),12,8,0);

							//打印出落点
							printf("视频中的球的落点是 ：x[%d]y[%d]:(%d,%d) FrmNum = %d \n",i-1,i-1,centerX[i-1],centerY[i-1],bounceFrmNum[k]);
							printf("计算出的球的落点是 ：Xs Ys:(%d,%d) FrmNum = %d \n",Xs,Ys,bounceFrmNum[k]);

							//调用是否在线外的函数进行判断，这里采用的有向线段的计算方式
							judge( Xa, Ya, Xb, Yb, Xs, Ys);


							//在judging display窗口中显示判决
							if( judging == true )
							{							
								//首先清除原来的字体
								cvZero( pJudge );
								//显示字体
								cvPutText( pJudge, "IN", pt1, &font, CV_RGB(255,255,255));	
							}
							else
							{						    
								//首先清除原来的字体
								cvZero( pJudge );	
								//显示字体
								cvPutText( pJudge, "OUT", pt2, &font, CV_RGB(255,255,255));							
							}

						}//end if 完成 第二种落地模式的判断

						//进行第三种模式的判断
						//防止一个背景中有多个落点,一般来说，两个落点的间隔时间在1秒以上，保证落地点提取的精确性
						if(((k>0 && (bounceFrmNum[k]-bounceFrmNum[k-1]>30)) || k ==0)
							&& ((abs(centerY[i-1] - centerY[i-2])<50) || (abs(centerY[i-1] - centerY[i])<50))
							&&((abs(centerY[i] - centerY[i-1])-abs(centerY[i-1] - centerY[i-2]))<0))

						{ 					
							//把用于判断的标记表示为真
							lastGrounded = true;
							//将球的坐标记录下来
							xi1 = centerX[i];
							yi1 = centerY[i];
							xi2 = centerX[i-1];
							yi2 = centerY[i-1];
							xi3 = centerX[i-2];
							yi3 = centerY[i-2];
							printf("xi1 =%d yi1 =%d xi2 =%d yi2 =%d xi3 =%d yi3 =%d \n", xi1,yi1,xi2,yi2,xi3,yi3);	
							//对bounceFrmNum的下标进行自增
							k++;
							//对球心数组下标进行自增
							i++;
							continue;		  
						}  //end if 第三种判断的第一步结束

						//对bounceFrmNum的下标进行自增
						k++;				
					}// end if 完成对球落地点的提取

					//第三种判断的第二部开始
					if(lastGrounded == true)
					{
						printf("第三种判断之后的当前帧%d\n",nFrmNum);
						
						//对坐标进行赋值
						xi0 = centerX[i];
						yi0 = centerY[i];
						printf("xi0 =%d yi0 =%d \n", xi0,yi0);	

						//求两条直线的交点
						intersect(xi0,yi0,xi1,yi1,xi2,yi2,xi3,yi3);
						
						//画出连接线
						cvLine(pTrjImg,cvPoint(centerX[i-1],centerY[i-1]),cvPoint(Xs,Ys),CV_RGB(255,255,0),14,8,0);
						cvLine(pTrjImg,cvPoint(centerX[i-2],centerY[i-2]),cvPoint(Xs,Ys),CV_RGB(255,255,0),14,8,0);

						//在背景图中标示出视频中的网球的落点
						cvCircle(pTrpImg,cvPoint(centerX[i-2],centerY[i-2]),5,CV_RGB(0,0,255),12,8,0);
						//在背景图中标示出计算出的网球的落点，绿色表示
						cvCircle(pTrpImg,cvPoint(Xs,Ys),5,CV_RGB(0,255,0),12,8,0);

						//在场景图中标示出网球的落点
						cvCircle(pTrjImg,cvPoint(centerX[i-2],centerY[i-2]),2,CV_RGB(0,0,255),8,8,0);
						//在背景图中标示出计算出的网球的落点，绿色表示
						cvCircle(pTrjImg,cvPoint(Xs,Ys),2,CV_RGB(0,255,0),12,8,0);

						//打印出落点
						printf("视频中的球的落点是 ：x[%d]y[%d]:(%d,%d) FrmNum = %d \n",i-1,i-1,centerX[i-2],centerY[i-2],bounceFrmNum[k]);
						printf("计算出的球的落点是 ：Xs Ys:(%d,%d) FrmNum = %d \n",Xs,Ys,bounceFrmNum[k]);

						//调用是否在线外的函数进行判断，这里采用的有向线段的计算方式
						judge( Xa, Ya, Xb, Yb, Xs, Ys);

						//在judging display窗口中显示判决
						if( judging == true )
						{
							//首先清除原来的字体
							cvZero( pJudge );
							//显示字体
							cvPutText( pJudge, "IN", pt1, &font, CV_RGB(255,255,255));	

						}
						else
						{
							//首先清除原来的字体
							cvZero( pJudge );	
							//显示字体
							cvPutText( pJudge, "OUT", pt2, &font, CV_RGB(255,255,255));						
						}

						//把判断标志置为假
						lastGrounded = false;
					}//end if 完成对第三种情况的分析
		    			
					//对球心数组下标进行自增
					i++;

				}//end if 目标的识别

			}//end for
		
			//背景更新模式选择，如果需要使用Surendra更新模式，请将if 0改为 if 1
	  
#if 0		
			//将图像中的背景转换为矩阵
			cvConvert(pFrImg, Dxy);
			
			//使用Surendra更新模式，但是运算速度会变慢，可以取出运动的网球的影响
			Surendra(temp,pBkMat,pFrameMat,Dxy );
		    	  
			//更新背景，权值取得是0.3
			cvRunningAvg(pFrameMat, temp, 0.3, 0);

#else
			//更新背景，权值取得是0.01，不考虑背景中的运动物体，速度较快
			cvRunningAvg(pFrameMat, pBkMat, 0.01, 0);

#endif

			//将背景转化为图像格式，用以显示
			cvConvert(pBkMat, pBkImg);
	 
			//显示图像
			cvShowImage(displayName, pFrame);
			cvShowImage("background", pBkImg);
			cvShowImage("background model", pHough);
			cvShowImage("binary", pBinImg);
			cvShowImage("point tracking", pTrpImg);
			cvShowImage("trajectory tracking", pTrjImg);
			cvShowImage("judging display", pJudge);
	 
			//如果有按键事件，则跳出循环,退出程序
			char c = cvWaitKey(10);
			if (c == 27 )
			return 0;
 
		}//end else
	
	}// end  while

	cvWaitKey(0);
	 
    //销毁窗口
	cvDestroyWindow(displayName);
    cvDestroyWindow("background");
	cvDestroyWindow("background model");
	cvDestroyWindow("binary");
	cvDestroyWindow("point tracking");
	cvDestroyWindow("trajectory tracking");
	cvDestroyWindow("judging display");
 
	//释放图像
	cvReleaseImage(&pFrame);
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);
	cvReleaseImage(&pEqlImg);
	cvReleaseImage(&pBinImg);
	cvReleaseImage(&pTrpImg);
	cvReleaseImage(&pTrjImg);	
	cvReleaseImage(&pHough);
	cvReleaseImage(&pCanny);
	cvReleaseImage(&pJudge);
	cvReleaseImage(&pOper);

    //释放矩阵
	cvReleaseMat(&pFrameMat);
	cvReleaseMat(&pFrMat);
	cvReleaseMat(&pBkMat);
	cvReleaseMat(&temp);
	cvReleaseMat(&Dxy);

	//释放内存
	cvReleaseMemStorage( &storageContour );
	cvReleaseMemStorage( &storageHough );
	cvReleaseCapture(&pCapture);

	return 0;
}




