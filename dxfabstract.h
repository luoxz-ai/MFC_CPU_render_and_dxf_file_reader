#ifndef DXFABSTRACT_HPP_INCLUDED
#define DXFABSTRACT_HPP_INCLUDED
#include <ostream>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <list>
//目前支持的DXF图元类型
enum DxfEntitiesType
{
    //初始化的类型，啥也不是
    DXF_EN_NEW,
    //直线图元
    DXF_EN_LINE,
    //点图元
    DXF_EN_POINT,
    //圆图元
    DXF_EN_CIRCLE,
    //弧线图元
    DXF_EN_ARC
};

//通用数据，为了方便提取。
struct Univer
{
    DxfEntitiesType Type;
    float X, Y;
};

//线性图元的数据
struct Line
{
    DxfEntitiesType Type;
    float Sx,Sy,Ex,Ey;
};

//圆图元的数据
struct Circle
{
    DxfEntitiesType Type;
    float Cx,Cy,R;
};

//弧线图元的数据
struct ArcE
{
    DxfEntitiesType Type;
    float Cx,Cy,R,Sa,Ea;
};

//点图元的数据
struct Point
{
    DxfEntitiesType Type;
    float X,Y;
};

//这是一个共同体，储存了了所有的数据
union Entities
{
    DxfEntitiesType Type;
    Univer Un;
    Line Li;
    Circle Ci;
    ArcE Ar;
    Point Po;
};

//提取器的类型
class DxfAbstract
{
    //树+链表的结构，其中前元素是图层，后元素是用链表储存的图元，访问方式需要先指定图层，然后再指定图元。
    std::map<std::wstring,std::list<Entities> > LayerEntities;

    //在平面上的最大和最小坐标值。
    float MaxX,MaxY,MinX,MinY;

    //重载<<操作符，方便输出到文件进行调试操作。
    friend std::wostream& operator<< (std::wostream& ,const DxfAbstract&);

public:

    //重载了一些迭代器的类型
    typedef std::map<std::wstring,std::list<Entities> >::iterator MapIterator;
    typedef std::list<Entities>::iterator  ListIterator ;
	typedef std::map<std::wstring,std::list<Entities> >::const_iterator MapConstIterator;
	typedef std::list<Entities>::const_iterator  ListConstIterator ;
    typedef std::map<std::string,std::list<Entities> > Map ;
    typedef std::list<Entities> List ;

    //将P1,P2,P3中的最小值赋给Min，最大值赋值给Max。
    void Select(float P1,float P2,float P3,float& Max,float& Min);

    //在文件中同时读两行，其结束标志为换行符，第一行放到P中，另一个放到D中。
	bool GetDoubleLine(std::wifstream& F,std::wstring& P,std::wstring& D);

	//重新计算最大最小的XY值。
    void ReCalculate();

	//以最小的X,Y值计算起始点
	void AutoSetMarkPoint();

    //对所有所储存的类型进行平行偏移。
    void Move(float X=0,float Y=0,float Z=0);

    //读取文件，并提取其中所有所支持的图元。
    bool ReadDxfFile(const std::wstring& L);

    //某个类成员函数的轮询。
    template<typename Tem>
        bool LayerForEach(const std::wstring& P,bool (Tem::*)(Entities&,void*),Tem* T,void*);

    //对单独某个图层的图元进行轮询。
    bool LayerForEach(const std::wstring& P,bool (*)(Entities&,void*),void*);
    template<typename Tem>
        bool ForEach(bool (Tem::*)(Entities&,void*),Tem* T,void*);
    bool ForEach(bool (*)(Entities&,void*),void*);

    //输出最大最小XY值。
	void GetRangeXY(float& MaX,float & MiX,float& MaY,float& MiY);
};

template<typename Tem>
bool DxfAbstract::ForEach(bool (Tem::*FP)(Entities& ,void* Data),Tem* T,void* Data)
{
	if(LayerEntities.empty()) return false;
	for(MapIterator Find=LayerEntities.begin();Find!=LayerEntities.end();++Find)
		for(ListIterator Kp=Find->second.begin();Kp!=Find->second.end();++Kp)
			if((T->*FP)(*Kp,Data))
				return true;
	return true;
}

template<typename Tem>
bool DxfAbstract::LayerForEach(const std::wstring& P,bool (Tem::*FP)(Entities&,void* Data),Tem* T,void* Data)
{
	MapIterator Find=LayerEntities.find(P);
	if(Find==LayerEntities.end()) return false;
	for(ListIterator Ite=Find->second.begin();Ite!=Find->second.end();++Ite)
		if((T->*FP)(*Ite,Data))  return true;
	return true;
}

std::wostream& operator<< (std::wostream& L,DxfAbstract& DER);
#endif // DXFENTITIESREADER_HPP_INCLUDED
