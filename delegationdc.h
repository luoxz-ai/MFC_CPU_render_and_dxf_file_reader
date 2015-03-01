#ifndef DELEGATIONDC_H_INCLUDE
#define DELEGATIONDC_H_INCLUDE
#include <vector>
#include <math.h>
#include <windows.h>
/*
    �����DC��һ�������࣬����ֱ�ӵ���DCʹ�ã����Ǹ������ʵ��3Dת2D�ٻ��������Ĺ��ܡ�
    �����Դ���һ��CBitmap�࣬���жԸ�����еĻ�ͼ���������������ϣ��ȴ���Ҫ��ʱ���ٽ�����CBitmap
һ����Ⱦ������DC�ϡ�

    �ڿ�ʼ������ʹ��DelegationDC3D().Create(...)����CBitmap���ͱ���CDC�ĳ�ʼ�������ڲ���Ҫʹ�õ�ʱ��
ʹ��DelegationDC3D().Free()�ͷ�������Դ��

    ��Ȼ����DelegationDC3D���ʵ���뿪��������ʱ������ͨ�����������Զ�����Free()������������ε���
��Create()��Free()�ǰ�ȫ�ģ�Create�����һ�ε��õĲ���Ϊ׼����Free()ֻ����Create()��ĵ�һ�ε��ò�
��Ч��
*/
class DelegationDC3D:public CDC
{
    /*����ת�þ�����Ҫ������| A,B,C |����ƫ�ã�����õĹ�ʽΪ��
    |  R[0][0]  R[0][1]  R[0][2] |   | A |   | M[0] |
    |  R[1][0]  R[1][1]  R[1][2] | * | B | + | M[1] |
    |  R[2][0]  R[2][1]  R[2][2] |   | C |   | M[2] |
    */
	float Rotate[3][3],Move[3];

    //CBitmap�����س�������������DC��ͬ���ɴ��С�ɵ��ڣ�����ֻ��1:1������Ⱦ���������DC��ʱ�����ᶪʧ���֡�
	unsigned int W,H;

    //CBitmap�࣬���жԸ�����еĻ�ͼ��������ֱ��д���䣬���������ʱһ�������
	CBitmap Buffer;

    //һ������������ʾʹ��ƽ��ͶӰ����͸��ͶӰ��ǰ����ȫ����1��1��ʵ��ԭ���壬���������ֵ��z��Խ��������ԽС��
	bool UsePerspective;
	//��ʾ������Դ�Ƿ񴴽����ǳ���Ҫ����Ҫ�ֶ��޸ġ�
	bool Created;

    //���������Ǳ�������ʵ�干�еĻ�����������
	static POINT PT[8];
	static float MatrixTem[4][3];
	static std::vector<float> PointPool;
public:
    //���캯������������
	DelegationDC3D();
	~DelegationDC3D();

	//����������Դ�����и��ֻ�ͼǰ������ã������׷������������
	void Create(unsigned int Wp,const int Hp,CDC* CP);

	//������ڱ�����������ת������������ľ���Ŀ�꣬���ܵ���һ������ָ�룬
	//ǰ�߱�ʾ������Դ�����߱�ʾ���������ע����ǣ�����������д��ڵ���3���Ĵ���ռ䡣
	//�����ڲ���������κ��ڴ�Ŀ��ٺ��ͷš�
	void RelativeTurnToAbusoult(const float * R,float *T);

	//��3D����ת����2Dƽ�����꣬��ʹ������ͶӰ��ʽ�������ú�����ִ�з�ʽ��
	//ֵ��ע����ǣ���ʹ��͸��ͶӰʱ������õ㷵����flase�����ʾ�õ��ڿ�������֮�⡣
	bool Turn3DTo2D(const float * D,POINT& P);

	//ֱ���޸�ת�þ����е�M[3]��ֵ��
	void DirectTranslation(float X,float Y,float Z);

	//
	void PosTranslation(float X,float Y,float Z);

	//��������| X,Y,Z |ֵ�����ɵ������������ֶ���ת��һ���Ļ��ȣ����и���������Ϊ��λ������
	void RotateTo(float Angel,float X,float Y,float Z);

	//���ضϵ��߶�2Dת3D�����߶ε�һ��λ���������һ��λ�������ڣ��򳬳�����Ĳ��ֽ����ᱻ���ƣ��亯��������false��
	bool LineCutOut3DTo2D(const float *P1,float *P2,POINT& PR1,POINT& PR2);

	//����������ɫ����һ��3d�㡣
	void DrawPoint(const float* L,unsigned int R=0,unsigned int G=0,unsigned int B=0);

	//������ת�õػ���һ��2d�㣨ֱ�ӻ滭����
	void DrawAbsolutePoint(int X,int Y,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//������ת�õػ���һ��2d�㣨ֱ�ӻ滭����
	void DrawAbsolutePoint(int X,int Y,CPen&);

	//����һ��Բ��������cΪ��ά��Բ�����꣬raΪ�뾶��saΪ��ʼ�Ƕȣ�eaΪ�����Ƕȣ���Ϊ�Ƕ�ֵ��������Ϊ������ɫ��
	void DrawArc(  const float* C,float Ra,float SA,float EA,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//����һ��Բ��������cΪ��ά��Բ�����꣬raΪ�뾶��saΪ��ʼ�Ƕȣ�eaΪ�����Ƕȣ���Ϊ�Ƕ�ֵ��������Ϊ������ɫ��
	void DrawArc(  const float* C,float Ra,float SA,float EA,CPen&);

	//����һ��Բ�������в���ΪԲ�����꣬������꣬��β���ꡣ
	void DrawArcCenter( const float* C,const float* S,const float* E,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//����һ��Բ�������в���ΪԲ�����꣬������꣬��β���ꡣ
	void DrawArcCenter( const float* C,const float* S,const float* E,CPen&);

	//����һ��԰������p1,p2,p3�ֱ�Ϊ������
	void DrawArc(  const float* P1,const float *P2,const float *P3,unsigned int R,unsigned int G,unsigned int B);
	//����һ��԰������p1,p2,p3�ֱ�Ϊ������
	void DrawArc(  const float* P1,const float *P2,const float *P3,CPen&);

	//����һ��Բ������cΪ��άԲ�����꣬raΪ�뾶��������Ϊ������ɫ��
	void DrawCircle( const float* C,float Ra,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//����һ��Բ������cΪ��άԲ�����꣬raΪ�뾶��������Ϊ������ɫ��
	void DrawCircle( const float* C,float Ra,CPen& );

	//ֱ�ӻ���һ���߶�
	void DrawAbsoluteLine(int SX,int SY,int EX,int EY,unsigned int R=0,unsigned int G=0,unsigned int B=0);	
	//ֱ�ӻ���һ���߶�
	void DrawAbsoluteLine(int SX,int SY,int EX,int EY,CPen&);

	//����ֱ�ߣ�����SΪ�������ݵĵ㼯��DΪ����˳�����飬PointSizeΪ���е�ĸ���������3��AllSizeΪ��D�����еĳ��ȡ�
	//����ֱ�ߵ㼯P ={ 0,0,0,   1,1,1,    2,2,2,   4,4,4   }����ȫ��������ƣ���PointSize��ֵӦΪ3*4��
	//���л���˳������D = { 0,1,1,2,2,3  }����AllSize��ֵΪ6��
	//���У�����˳�����������Ϊ��ÿ����������Ӧ�㼯��һ�㣬���Ұ�˳��ÿ��������Ϊһ�飬ÿ���ʾһ���ߡ�
	void DrawLine(const float* S , const unsigned int *D, unsigned int PointSize,unsigned int AllSize,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	void DrawLine(const float* S , const unsigned int *D, unsigned int PointSize,unsigned int AllSize,CPen&);

	//����ǽ�һ��vector<float>���������ֱ�ӻ��ƣ����漴Ϊһ������ߣ���ֻ��һ����ͷ��һ����β��
	void DrawLine(const std::vector<float>& All,unsigned int R=0,unsigned int G=0,unsigned int B=0);
	//����ǽ�һ��vector<float>���������ֱ�ӻ��ƣ����漴Ϊһ������ߣ���ֻ��һ����ͷ��һ����β��
	void DrawLine(const std::vector<float>& All,CPen&);

	//��ת�þ����ɳ�ʼ��λ����
	void Indentity();

	//��ת�þ���ƽ�ơ�
	void Translation(float X,float Y,float Z);

	//��ת�þ���������ţ�ע�⣬����Ϊ��0ֵ��
	void Scale(float X,float Y,float Z);

	//��CbitMap��Ⱦ�����ṩ��DC����S,E����Ϊ������ʼ���λ���ϡ�
	void DrawTo(int S,int E,CDC* PC);

	//�����ṩ����ɫˢ��
	void Clean(unsigned int R=255,unsigned int G=255,unsigned int B=255);

	//�ͷ���Դ
	void Free();

	//����Ϊ͸��ͶӰ
	void SetPerspective();

	//����Ϊƽ��ͶӰ
	void SetParallel();

	//��������
	void DrawNetXY( unsigned int Ex, unsigned int Ey ,unsigned int =255,unsigned int=255,unsigned int=255);
};

//POINT DrawPool::PT1;
//POINT DrawPool::PT2;
//float DrawPool::MatrixTem[3][3];
//std::vector<float> DrawPool::PointPool;
#endif
