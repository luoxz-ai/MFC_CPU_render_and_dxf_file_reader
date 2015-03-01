#ifndef DELEGATIONDC_H_INCLUDE
#define DELEGATIONDC_H_INCLUDE
#include <vector>
#include <math.h>
#include <windows.h>
/*
    这个是DC的一个派生类，可以直接当成DC使用，但是更多的是实现3D转2D再画到画布的功能。
    该类自带了一个CBitmap类，所有对该类进行的画图操作均画到该类上，等待需要的时候，再将整个CBitmap
一起渲染到外界的DC上。

    在开始，必须使用DelegationDC3D().Create(...)进行CBitmap，和本体CDC的初始化，并在不需要使用的时候
使用DelegationDC3D().Free()释放所有资源。

    当然，在DelegationDC3D类的实体离开其生存期时，将会通过析构函数自动调用Free()，并且连续多次调用
与Create()或Free()是安全的，Create以最后一次调用的参数为准，而Free()只有在Create()后的第一次调用才
有效。
*/
class DelegationDC3D:public CDC
{
    /*两个转置矩阵，若要对向量| A,B,C |进行偏置，则调用的公式为：
    |  R[0][0]  R[0][1]  R[0][2] |   | A |   | M[0] |
    |  R[1][0]  R[1][1]  R[1][2] | * | B | + | M[1] |
    |  R[2][0]  R[2][1]  R[2][2] |   | C |   | M[2] |
    */
	float Rotate[3][3],Move[3];

    //CBitmap的像素长宽，可以与外界的DC不同，可大可小可等于，但是只能1:1进行渲染，当比外界DC大时，将会丢失部分。
	unsigned int W,H;

    //CBitmap类，所有对该类进行的绘图操作都将直接写入其，并且在输出时一次输出。
	CBitmap Buffer;

    //一个开关量，表示使用平行投影还是透视投影，前者完全按照1：1真实还原物体，而后者深度值（z）越大，则看起来越小。
	bool UsePerspective;
	//表示各种资源是否创建，非常重要，不要手动修改。
	bool Created;

    //下面三个是本类所有实体共有的缓冲数据区。
	static POINT PT[8];
	static float MatrixTem[4][3];
	static std::vector<float> PointPool;
public:
    //构造函数和析构函数
	DelegationDC3D();
	~DelegationDC3D();

	//创建各种资源，进行各种绘图前必须调用，否则易发生意外情况。
	void Create(unsigned int Wp,const int Hp,CDC* CP);

	//将相对于本类的相对坐标转换成相对于外界的绝对目标，接受的是一个数组指针，
	//前者表示数据来源，后者表示数据输出，注意的是，各数组必须有大于等于3个的储存空间。
	//在其内部不会进行任何内存的开辟和释放。
	void RelativeTurnToAbusoult(const float * R,float *T);

	//将3D坐标转换成2D平面坐标，其使用哪种投影方式将决定该函数的执行方式。
	//值得注意的是，当使用透视投影时，如果该点返回了flase，则表示该点在可视视体之外。
	bool Turn3DTo2D(const float * D,POINT& P);

	//直接修改转置矩阵中的M[3]的值。
	void DirectTranslation(float X,float Y,float Z);

	//
	void PosTranslation(float X,float Y,float Z);

	//将按照由| X,Y,Z |值所构成的向量，按右手定则转过一定的弧度，其中该向量必须为单位向量。
	void RotateTo(float Angel,float X,float Y,float Z);

	//带截断的线段2D转3D，当线段的一点位于视体外而一点位于视体内，则超出视体的部分将不会被绘制，其函数将返回false。
	bool LineCutOut3DTo2D(const float *P1,float *P2,POINT& PR1,POINT& PR2);

	//以所给的颜色绘制一个3d点。
	void DrawPoint(const float* L,unsigned int R=0,unsigned int G=0,unsigned int B=0);

	//不进行转置地绘制一个2d点（直接绘画）。
	void DrawAbsolutePoint(int X,int Y,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//不进行转置地绘制一个2d点（直接绘画）。
	void DrawAbsolutePoint(int X,int Y,CPen&);

	//绘制一个圆弧，其中c为三维的圆心坐标，ra为半径，sa为起始角度，ea为结束角度，都为角度值，后三个为绘制颜色。
	void DrawArc(  const float* C,float Ra,float SA,float EA,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//绘制一个圆弧，其中c为三维的圆心坐标，ra为半径，sa为起始角度，ea为结束角度，都为角度值，后三个为绘制颜色。
	void DrawArc(  const float* C,float Ra,float SA,float EA,CPen&);

	//绘制一个圆弧，其中参数为圆心坐标，起点坐标，结尾坐标。
	void DrawArcCenter( const float* C,const float* S,const float* E,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//绘制一个圆弧，其中参数为圆心坐标，起点坐标，结尾坐标。
	void DrawArcCenter( const float* C,const float* S,const float* E,CPen&);

	//绘制一个园，其中p1,p2,p3分别为三个点
	void DrawArc(  const float* P1,const float *P2,const float *P3,unsigned int R,unsigned int G,unsigned int B);
	//绘制一个园，其中p1,p2,p3分别为三个点
	void DrawArc(  const float* P1,const float *P2,const float *P3,CPen&);

	//绘制一个圆，其中c为三维圆心坐标，ra为半径，后三个为绘制颜色。
	void DrawCircle( const float* C,float Ra,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//绘制一个圆，其中c为三维圆心坐标，ra为半径，后三个为绘制颜色。
	void DrawCircle( const float* C,float Ra,CPen& );

	//直接绘制一个线段
	void DrawAbsoluteLine(int SX,int SY,int EX,int EY,unsigned int R=0,unsigned int G=0,unsigned int B=0);	
	//直接绘制一个线段
	void DrawAbsoluteLine(int SX,int SY,int EX,int EY,CPen&);

	//绘制直线，其中S为所有数据的点集，D为绘制顺序数组，PointSize为所有点的个数并乘上3，AllSize为在D数组中的长度。
	//当有直线点集P ={ 0,0,0,   1,1,1,    2,2,2,   4,4,4   }，若全部参与绘制，则PointSize的值应为3*4。
	//若有绘制顺序数组D = { 0,1,1,2,2,3  }，则AllSize的值为6。
	//其中，绘制顺序数组的意义为，每个索引均对应点集的一点，并且按顺序每两个索引为一组，每组表示一个线。
	void DrawLine(const float* S , const unsigned int *D, unsigned int PointSize,unsigned int AllSize,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	void DrawLine(const float* S , const unsigned int *D, unsigned int PointSize,unsigned int AllSize,CPen&);

	//这个是将一个vector<float>里面的数据直接绘制，里面即为一条多短线，线只有一个开头和一个结尾。
	void DrawLine(const std::vector<float>& All,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//这个是将一个vector<float>里面的数据直接绘制，里面即为一条多短线，线只有一个开头和一个结尾。
	void DrawLine(const std::vector<float>& All,CPen&);

	//将转置矩阵变成初始单位矩阵。
	void Indentity();

	//将转置矩阵平移。
	void Translation(float X,float Y,float Z);

	//对转置矩阵进行缩放，注意，必须为非0值。
	void Scale(float X,float Y,float Z);

	//将CbitMap渲染到所提供的DC的以S,E坐标为左上起始点的位置上。
	void DrawTo(int S,int E,CDC* PC);

	//以所提供的颜色刷屏
	void Clean(unsigned int R=255,unsigned int G=255,unsigned int B=255);

	//释放资源
	void Free();

	//设置为透视投影
	void SetPerspective();

	//设置为平行投影
	void SetParallel();

	//绘制网格
	void DrawNetXY( unsigned int Ex, unsigned int Ey ,unsigned int =255,unsigned int=255,unsigned int=255);
};

//POINT DrawPool::PT1;
//POINT DrawPool::PT2;
//float DrawPool::MatrixTem[3][3];
//std::vector<float> DrawPool::PointPool;
#endif
