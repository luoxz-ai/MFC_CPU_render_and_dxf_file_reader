#ifndef DXFABSTRACT_HPP_INCLUDED
#define DXFABSTRACT_HPP_INCLUDED
#include <ostream>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <list>
//Ŀǰ֧�ֵ�DXFͼԪ����
enum DxfEntitiesType
{
    //��ʼ�������ͣ�ɶҲ����
    DXF_EN_NEW,
    //ֱ��ͼԪ
    DXF_EN_LINE,
    //��ͼԪ
    DXF_EN_POINT,
    //ԲͼԪ
    DXF_EN_CIRCLE,
    //����ͼԪ
    DXF_EN_ARC
};

//ͨ�����ݣ�Ϊ�˷�����ȡ��
struct Univer
{
    DxfEntitiesType Type;
    float X, Y;
};

//����ͼԪ������
struct Line
{
    DxfEntitiesType Type;
    float Sx,Sy,Ex,Ey;
};

//ԲͼԪ������
struct Circle
{
    DxfEntitiesType Type;
    float Cx,Cy,R;
};

//����ͼԪ������
struct ArcE
{
    DxfEntitiesType Type;
    float Cx,Cy,R,Sa,Ea;
};

//��ͼԪ������
struct Point
{
    DxfEntitiesType Type;
    float X,Y;
};

//����һ����ͬ�壬�����������е�����
union Entities
{
    DxfEntitiesType Type;
    Univer Un;
    Line Li;
    Circle Ci;
    ArcE Ar;
    Point Po;
};

//��ȡ��������
class DxfAbstract
{
    //��+����Ľṹ������ǰԪ����ͼ�㣬��Ԫ�������������ͼԪ�����ʷ�ʽ��Ҫ��ָ��ͼ�㣬Ȼ����ָ��ͼԪ��
    std::map<std::wstring,std::list<Entities> > LayerEntities;

    //��ƽ���ϵ�������С����ֵ��
    float MaxX,MaxY,MinX,MinY;

    //����<<������������������ļ����е��Բ�����
    friend std::wostream& operator<< (std::wostream& ,const DxfAbstract&);

public:

    //������һЩ������������
    typedef std::map<std::wstring,std::list<Entities> >::iterator MapIterator;
    typedef std::list<Entities>::iterator  ListIterator ;
	typedef std::map<std::wstring,std::list<Entities> >::const_iterator MapConstIterator;
	typedef std::list<Entities>::const_iterator  ListConstIterator ;
    typedef std::map<std::string,std::list<Entities> > Map ;
    typedef std::list<Entities> List ;

    //��P1,P2,P3�е���Сֵ����Min�����ֵ��ֵ��Max��
    void Select(float P1,float P2,float P3,float& Max,float& Min);

    //���ļ���ͬʱ�����У��������־Ϊ���з�����һ�зŵ�P�У���һ���ŵ�D�С�
	bool GetDoubleLine(std::wifstream& F,std::wstring& P,std::wstring& D);

	//���¼��������С��XYֵ��
    void ReCalculate();

	//����С��X,Yֵ������ʼ��
	void AutoSetMarkPoint();

    //����������������ͽ���ƽ��ƫ�ơ�
    void Move(float X=0,float Y=0,float Z=0);

    //��ȡ�ļ�������ȡ����������֧�ֵ�ͼԪ��
    bool ReadDxfFile(const std::wstring& L);

    //ĳ�����Ա��������ѯ��
    template<typename Tem>
        bool LayerForEach(const std::wstring& P,bool (Tem::*)(Entities&,void*),Tem* T,void*);

    //�Ե���ĳ��ͼ���ͼԪ������ѯ��
    bool LayerForEach(const std::wstring& P,bool (*)(Entities&,void*),void*);
    template<typename Tem>
        bool ForEach(bool (Tem::*)(Entities&,void*),Tem* T,void*);
    bool ForEach(bool (*)(Entities&,void*),void*);

    //��������СXYֵ��
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
