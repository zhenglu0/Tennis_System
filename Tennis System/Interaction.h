//本头文件"Interaction.h"用于定义视频中的一些交互操作


//在视频中实现暂停按钮在控制台的显示
void switch_off_function() 
{
	printf("Pause g_switch_value = %d \n",g_switch_value);
}

void switch_on_function() 
{
	printf("Run g_switch_value = %d \n",g_switch_value);

}

//在视频中实现暂停按钮的函数
void switch_callback( int position )
{
	if( position == 0 ) 
	{
		switch_off_function();
	} 
	else 
	{
		switch_on_function();
	}
}


//用于实现鼠标的操作,实现对trajectory tracking窗口的操作
void mouse_callback1(int event, int x, int y, int flags, void* param)
{
    if( !pTrjImg )    
	return; 

	//进行识别在点击鼠标左键的同时需要按住SHIFT键就可以执行操作
	if( event == CV_EVENT_LBUTTONDOWN &&(flags & CV_EVENT_FLAG_SHIFTKEY) )
	{
		//如果机器识别不准，可以用鼠标进行对场地线的点击而进行场地线的标定
		if( down == 0)
		{
			printf("传入的坐标为：(%d,%d)\n",x,y);
			printf("第一次修改的坐标为：(%d,%d)\n",Xa,Ya);
			Xa = x;
			Ya = y;		
			//对down这个参数进行自增
			down++;
		}
		else if( down == 1)
		{
			
			printf("传入的坐标为：(%d,%d)\n",x,y);
			printf("第二次修改的坐标为：(%d,%d)\n",Xb,Yb);
			Xb = x;
		    Yb = y;
			//对down这个参数进行自增
		    down++;
		
			//画出人工识别的直线
			cvLine( pOper, cvPoint(Xa,Ya),cvPoint(Xb,Yb),CV_RGB(255,0,255), 5, CV_AA, 0 );

			//复制图像
			cvCopy(pOper,pTrjImg);	
		}
	}

    if( event == CV_EVENT_RBUTTONDOWN )
	{
		//恢复轨迹图为没有轨迹的空场景状态
		if(down ==2)
		{
			cvCopy(pOper,pTrjImg);
		}
		else
		{
			printf("down = %d",down);
			cvCopy(pHough,pTrjImg);	
		}
	}
}


//用于实现鼠标的操作,实现对point tracking窗口的操作
void mouse_callback2(int event, int x, int y, int flags, void* param)
{
    if( !pTrpImg )
		return; 
    if( event == CV_EVENT_RBUTTONDOWN )
	//恢复轨迹点图为空
	cvZero(pTrpImg);	
}


//操作说明 
void manual()
{
	printf( "热键指南: \n"
            "\tESC - 退出程序\n"
            "\t在 trajectory tracking 窗口中点击右键即可清除上一个球的轨迹\n" 
			"\t在 trajectory tracking 窗口按住shift键然后点击鼠标左键可以人工识别网球线，"
			"特别注意因为采用有向线段的判别方式，所以请从按顺序图像底部到顶部进行点的选择\n" 
			"\t在 point tracking 窗口中点击右键即可清除上图像中的表示球\n" 
			"\t在 judging display  窗口中点击右键即可清除上图像中的对上一球的判决\n"
			"\t在 video 窗口中Pause/Run 中0为暂停，1为暂停后继续播放，点击1之后，还需要按一下键盘上的任意按键才可继续操作\n" );
}



//用于实现鼠标的操作,实现对judging display 窗口的操作
void mouse_callback3(int event, int x, int y, int flags, void* param)
{
    if( !pJudge )
		return; 
    if( event == CV_EVENT_RBUTTONDOWN )
	//恢复判决图片图为空
	cvZero(pJudge);	
}