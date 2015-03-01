#ifndef DXFRENDER_H_INCLUDE
#define DXFRENDER_H_INCLUDE
#include <fstream>
#include <string>
#include "dxfabstract.h"
#include "delegationdc.h"
#include "knifeabstract.h"
struct DxfRenderPlu
{
    //"MARK" ͼ������������������ԭʼ����
	float CenX,CenY;
	//ָ��ָ����"MARK"ͼ�����ҵ���ǰ������ͼԪ
	Entities* One,* Two;
	//���ֵ������Ƿ��ҵ�����ͼԪ
	bool FindOne,FindTwo;
	//�����Сֵ
	float MaxX,MinX,MaxY,MinY;
	//�߽�ֵ�ͱ߽��С
	float BoardX,BoardY,EachMax;
	//��dxfabstract����ѯ�ĺ��������⺯�������ҵ���ʼ������ͼԪ
	bool FindTwoLine( Entities& L,void* Data);
	//����߽�ֵ
	void GetRange( DxfAbstract& );
	//���ͼ�񣨲�����ˢ�£�
	void Render(Entities& E,DelegationDC3D& DP);
	//���ͼ�񣨲�����ˢ�£�
	void Render(KnifeTrajectory&,DelegationDC3D& Dp);
	//����������ֵ
	void Reset();
	//���¼��������ֵ
	bool ReCalculate( DxfAbstract& );
	//�Ե������ʵ�DXF�ļ���ͼ
	void AutoAdject(DelegationDC3D& DP);
};

//DxfRender����DelegationDC3D���һ����չ��
class DxfRender:public DelegationDC3D
{
	friend std::wostream& operator<< (std::wofstream&,const DelegationDC3D&);
public:
    //һ����ȡ����ʵ��
	DxfAbstract DER;

	KnifeAbstract KA;
	//һ���������������״��
	DxfRenderPlu Temporary;
	//�ص����ƺ������ú�����Ӧ�����û�����
	bool Draw(Entities& E,void* );
	//�ص����ƺ���
	bool DrawKnif(KnifeTrajectory&,void*);
	//������ȡ����ȡ�ļ���Ϣ��ԭ�����ݻᶪʧ
	bool ReadDxf( const std::wstring& Adress);
	//������ȡ����ȡ�ļ���Ϣ��ԭ�����ݻᶪʧ
	bool ReadKnif( const std::wstring& Adress);
	//����DXF�Ե������ʵı任����
	void AutoAdject();
	//����ͼԪ
	void DrawEntities();
	//���Ƶ���
	void DrawKnif();
};

std::wostream& operator<< (std::wostream& O,const DxfRender& DFR );



#endif
