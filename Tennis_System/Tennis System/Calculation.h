
//将数组中的少量元素用冒泡排序将第一列元素从大到小排列，同时移动第二列元素

void sort(int a[][2],int n) /*定义两个参数：数组首地址与数组行数大小*/ 
{ 
	int i,j,temp; 
	for(i=0;i<n-1;i++) 
	for(j=i+1;j<n;j++)
	//需要从大到小对第一元素同时绑定第二个元素进行排列
	if(a[i][0]<a[j][0])
	{ 
		temp = a[i][0]; 
		a[i][0] = a[j][0]; 
		a[j][0] = temp;
		temp = a[i][1]; 
		a[i][1] = a[j][1]; 
		a[j][1] = temp;
	} 
} 



//用于判断点是否在直线外的函数

void judge(int x0, int y0, int x1, int y1, int x, int y)
{
	fx = (y1 - y0)*(x - x0)-(y - y0)*(x1 - x0);
	printf("fx = %d\n",fx);
	//通过计算出的fx的值来判断球是否在界内
	if ( fx >0 || fx == 0)
	{	
		printf("球再界内\n");
		//改变judging的值
		judging = true;
	}
	else
	{
		printf("球再界外\n");	
		//改变judging的值
		judging = false;
	}
}



//求两个直线方程的公共解
/*
首先要找到二元一次方程组的通解，例如：
ax+by=e
cx+dy=f
不难算出
x=(ed-bf)/(ad-bc)
y=(ec-af)/(bc-ad) 
我们最后将结果转换为整形

同时我们可以知道通过两点的直线方程为：
(y1-y0)*x + (x0-x1)*y = x0*y1 - x1*y0
*/

void intersect(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3)
{
	//用于直线方程系数计算的变量
	int a = y1-y0; 
	int b = x0-x1;
	int e = x0*y1 - x1*y0; 
	int c = y3-y2; 
	int d = x2-x3; 
	int f = x2*y3 - x3*y2;

	//求两条直线的交点
	Xs=(e*d-b*f)/(a*d-b*c);
	Ys=(e*c-a*f)/(b*c-a*d);

	printf("方程的解为 Xs=%d,Ys=%d\n",Xs,Ys); 
}



//Surendra用于背景更新
void Surendra(CvMat* t,CvMat* B,CvMat* f,CvMat* D )
{
	int i,j;
	//取出矩阵中所有的元素用遍历
	for(i = 0;i<D->rows;i++)
		for(j = 0;j<D->cols;j++)
		{   //如果发现时背景就用当前帧更新，发现是运动物体就用背景更新
			if((int)cvGetReal2D(D,i,j) == 0)
			{	
				cvSetReal2D(t,i,j,cvGetReal2D(f,i,j));
			}
			else
			{
				cvSetReal2D(t,i,j,cvGetReal2D(B,i,j));
			}
		}
}

