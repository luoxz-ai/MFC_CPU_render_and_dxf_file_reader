#ifndef DXFRENDER_H_INCLUDE
#define DXFRENDER_H_INCLUDE
#include <fstream>
#include <string>
#include "dxfabstract.h"
#include "delegationdc.h"
#include "knifeabstract.h"
struct DxfRenderPlu
{
    //"MARK" 图层中由两点计算出来的原始坐标
	float CenX,CenY;
	//指针指向在"MARK"图层中找到的前两个线图元
	Entities* One,* Two;
	//标记值，标记是否找到两个图元
	bool FindOne,FindTwo;
	//最大最小值
	float MaxX,MinX,MaxY,MinY;
	//边界值和边界大小
	float BoardX,BoardY,EachMax;
	//被dxfabstract所轮询的函数，在这函数里面找到起始的两个图元
	bool FindTwoLine( Entities& L,void* Data);
	//计算边界值
	void GetRange( DxfAbstract& );
	//输出图像（并不会刷新）
	void Render(Entities& E,DelegationDC3D& DP);
	//输出图像（并不会刷新）
	void Render(KnifeTrajectory&,DelegationDC3D& Dp);
	//重置所有数值
	void Reset();
	//重新计算各种数值
	bool ReCalculate( DxfAbstract& );
	//自调整合适的DXF文件视图
	void AutoAdject(DelegationDC3D& DP);
};

//DxfRender类是DelegationDC3D类的一个扩展。
class DxfRender:public DelegationDC3D
{
	friend std::wostream& operator<< (std::wofstream&,const DelegationDC3D&);
public:
    //一个提取器的实体
	DxfAbstract DER;

	KnifeAbstract KA;
	//一个插件，分析各种状况
	DxfRenderPlu Temporary;
	//回调绘制函数，该函数不应该由用户调用
	bool Draw(Entities& E,void* );
	//回调绘制函数
	bool DrawKnif(KnifeTrajectory&,void*);
	//调用提取器提取文件信息，原先数据会丢失
	bool ReadDxf( const std::wstring& Adress);
	//调用提取器提取文件信息，原先数据会丢失
	bool ReadKnif( const std::wstring& Adress);
	//根据DXF自调整合适的变换矩阵
	void AutoAdject();
	//绘制图元
	void DrawEntities();
	//绘制刀具
	void DrawKnif();
};

std::wostream& operator<< (std::wostream& O,const DxfRender& DFR );



#endif
