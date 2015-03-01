#include "stdafx.h"
#include "delegationdc.h"
POINT DelegationDC3D::PT[8];
float DelegationDC3D::MatrixTem[4][3];
std::vector<float> DelegationDC3D::PointPool;
DelegationDC3D::DelegationDC3D():W(0),H(0),UsePerspective(false),Created(false) {}
DelegationDC3D::~DelegationDC3D()
{
    Free();
}
void DelegationDC3D::Create(unsigned int Wp,const int Hp,CDC* CP)
{
    W=Wp;
    H=Hp;
    Indentity();
    if(Created){Free();}
    CreateCompatibleDC(NULL);
    Buffer.CreateCompatibleBitmap(CP,Wp,Hp);
    CBitmap *pOldBit=SelectObject(&Buffer);
    FillSolidRect(0,0,W,H,RGB(200,200,200));
    Created=true;
}
void DelegationDC3D::RelativeTurnToAbusoult(const float * R,float *T)
{
    for(int i=0; i<3; ++i)
        T[i]=Rotate[i][0]*R[0]+Rotate[i][1]*R[1]+Rotate[i][2]*R[2]+Move[i];
}
bool DelegationDC3D::Turn3DTo2D(const float * D,POINT& P)
{
    if(!UsePerspective)
    {
        P.x=static_cast<int>((D[0]+1.0)/2.0*W);
        P.y=static_cast<int>((1.0-D[1])/2.0*H);
        return true;
    }
    else
    {
		if(D[2]>-0.0001&&D[2]<0.0001)
			return false;
        P.x=static_cast<int>((D[0]/D[2]*0.3+1)/2.0*W);
        P.y=static_cast<int>(1.0-(D[1]/D[2]*0.3)/2.0*H);
    }
    if(D[2]<0.05) return false;
    else return true;
}

void DelegationDC3D::DirectTranslation(float X,float Y,float Z)
{
    Move[0]=X;
    Move[1]=Y;
    Move[2]=Z;
}

void DelegationDC3D::PosTranslation(float X,float Y,float Z)
{
	Move[0]+=X;
	Move[1]+=Y;
	Move[2]+=Z;
}

void DelegationDC3D::RotateTo(float Angel,float X,float Y,float Z)
{
    static float Tem2[3][3];
    float   C=cos(Angel),
            S=sin(Angel),
            C1=1-C,
            XX=X*X,
            YY=Y*Y,
            ZZ=Z*Z,
            XY=X*Y,
            XZ=X*Z,
            YZ=Y*Z,
            XS=X*S,
            YS=Y*S,
            ZS=Z*S;
    Tem2[0][0]=XX*C1+C;
    Tem2[0][1]=XY*C1-ZS;
    Tem2[0][2]=XZ*C1+YS;
    Tem2[1][0]=XY*C1+ZS;
    Tem2[1][1]=YY*C1+C;
    Tem2[1][2]=YZ*C1-XS;
    Tem2[2][0]=XZ*C1-YS;
    Tem2[2][1]=YZ*C1+XS;
    Tem2[2][2]=ZZ*C1+C;
    for(int i=0; i<3; ++i)
        for(int k=0; k<3; ++k)
            MatrixTem[i][k]=Rotate[i][k];
    for(int i=0; i<3; ++i)
        for(int k=0; k<3; ++k)
        {
            Rotate[i][k]=0;
            for(int p=0; p<3; ++p)
                Rotate[i][k]+=MatrixTem[i][p]*Tem2[p][k];
        }

}

bool DelegationDC3D::LineCutOut3DTo2D(const float *P1,float *P2,POINT& PR1,POINT& PR2)
{
    if(!(P1[2]<0.05f&&P2[2]<0.05f))
    {
        if(P1[2]<0.05f)
        {
            float Ke=static_cast<float>((P2[2]-0.05)/(P2[2]-P1[2]));
            for(int i=0; i<2; ++i)
                MatrixTem[0][i]=static_cast<float>((P1[i]-P2[i])*Ke+P2[i]);
            MatrixTem[0][2]=0.05f;
            Turn3DTo2D(MatrixTem[0],PR1);
            Turn3DTo2D(P2,PR2);
        }
        else if(P2[2]<0.05f)
        {
            float Ke=static_cast<float>((P1[2]-0.05)/(P1[2]-P2[2]));
            for(int i=0; i<2; ++i)
                MatrixTem[0][i]=static_cast<float>((P2[i]-P1[i])*Ke+P1[i]);
            MatrixTem[0][2]=0.05f;
            Turn3DTo2D(P1,PR1);
            Turn3DTo2D(MatrixTem[0],PR2);
        }
        else
        {
            Turn3DTo2D(P1,PR1);
            Turn3DTo2D(P2,PR2);
        }
        return true;
    }
    return false;
}

void DelegationDC3D::DrawPoint(const float* L,unsigned int R,unsigned int G,unsigned int B)
{
    RelativeTurnToAbusoult(L,MatrixTem[0]);
    Turn3DTo2D(MatrixTem[0],PT[0]);
    SetPixel(PT[0],RGB(R,G,B));
}


void DelegationDC3D::DrawAbsolutePoint(int X,int Y,unsigned int R,unsigned int G,unsigned int B)
{
    SetPixel(X,Y,RGB(R,G,B));
}

void DelegationDC3D::DrawArc(  const float* P1,const float *P2,const float *P3,unsigned int R,unsigned int G,unsigned int B)
{
	CPen Pen;
	Pen.CreatePen(PS_SOLID,1,RGB(R,G,B));
	DrawArc(P1,P2,P3,Pen);
	Pen.DeleteObject();
}

void DelegationDC3D::DrawArc(  const float* P1,const float *P2,const float *P3,CPen& CP)
{
	CPen* Old=SelectObject(&CP);
	float Dx1=(P2[0]*P2[0]-P1[0]*P1[0]-P2[1]*P2[1]+P1[1]*P1[1]  )/2.0f;
	float Dy1=(P3[0]*P3[0]-P2[0]*P2[0]-P3[1]*P3[1]+P2[1]*P2[1]  )/2.0f;
	float D=(P2[0]-P1[0])*(P2[1]-P3[1])-(P1[1]-P2[1])*(P3[0]-P2[0]);
	float Dx=( Dx1)*(P2[1]-P3[1])-(P1[1]-P2[1])*(Dy1);
	float Dy=(P2[0]-P1[0])*(Dx1)-(Dy1)*(P3[0]-P2[0]);
	MatrixTem[0][0]=Dx/D;
	MatrixTem[0][1]=Dy/D;
	MatrixTem[0][2]=0.0;
	float Ra=sqrt( pow(P1[0]-MatrixTem[0][0],2)+pow(P1[1]-MatrixTem[0][1],2)   );
	int T=static_cast<int>(Ra*sqrt( pow(Rotate[0][0],2)+pow(Rotate[1][0],2)+pow(Rotate[2][0],2) )/2.0*W);
	RelativeTurnToAbusoult(MatrixTem[0],MatrixTem[1]);
	Turn3DTo2D(MatrixTem[1],PT[0]);
	RelativeTurnToAbusoult(P1,MatrixTem[1]);
	Turn3DTo2D(MatrixTem[1],PT[1]);
	RelativeTurnToAbusoult(P3,MatrixTem[1]);
	Turn3DTo2D(MatrixTem[1],PT[2]);

	Arc(PT[0].x-T,PT[0].y-T,PT[0].x+T,PT[0].y+T,PT[1].x,PT[1].y,PT[2].x,PT[2].y);

	SelectObject(Old);
}


void DelegationDC3D::DrawArcCenter( const float* C,const float* S,const float* E,unsigned int R,unsigned int G,unsigned int B)
{
	CPen Pen;
	Pen.CreatePen(PS_SOLID,1,RGB(R,G,B));
	DrawArcCenter(C,S,E,Pen);
	Pen.DeleteObject();
}

void DelegationDC3D::DrawArcCenter( const float* C,const float* S,const float* E,CPen& Pen)
{
	CPen* Old=SelectObject(&Pen);


	RelativeTurnToAbusoult(C,MatrixTem[0]);
	RelativeTurnToAbusoult(S,MatrixTem[1]);
	RelativeTurnToAbusoult(E,MatrixTem[2]);

	for(int i=0;i<3;++i)
		Turn3DTo2D(MatrixTem[i],PT[i]);

	float Ra=sqrt( pow(C[0]-S[0],2)+pow(C[1]-S[1],2) );
	int T=static_cast<int>(Ra*sqrt( pow(Rotate[0][0],2)+pow(Rotate[1][0],2)+pow(Rotate[2][0],2) )/2.0*W);

	Arc(PT[0].x-T,PT[0].y-T,PT[0].x+T,PT[0].y+T,PT[1].x,PT[1].y,PT[2].x,PT[2].y);

	SelectObject(Old);
}

void DelegationDC3D::DrawArc(  const float* C,float Ra,float SA,float EA,unsigned int R,unsigned int G,unsigned int B)
{
    CPen Pen;
	Pen.CreatePen(PS_SOLID,1,RGB(R,G,B));
	DrawArc( C,Ra,SA,EA,Pen);
    Pen.DeleteObject();
}

void DelegationDC3D::DrawArc(  const float* C,float Ra,float SA,float EA,CPen& Pen)
{
	CPen* Old=SelectObject(&Pen);

	RelativeTurnToAbusoult(C,MatrixTem[0]);
	Turn3DTo2D(MatrixTem[0],PT[0]);
	int T=static_cast<int>(Ra*sqrt( pow(Rotate[0][0],2)+pow(Rotate[1][0],2)+pow(Rotate[2][0],2) )/2.0*W);
	MatrixTem[1][0]=C[0]+Ra*cos(  SA/180.0f*3.1415926f );
	MatrixTem[1][1]=C[1]+Ra*sin(  SA/180.0f*3.1415926f  );
	MatrixTem[1][2]=C[2];

	MatrixTem[2][0]=C[0]+Ra*cos(  EA/180.0f*3.1415926f  );
	MatrixTem[2][1]=C[1]+Ra*sin(  EA/180.0f*3.1415926f  );
	MatrixTem[2][2]=C[2];

	RelativeTurnToAbusoult(MatrixTem[1],MatrixTem[0]);
	Turn3DTo2D(MatrixTem[0],PT[1]);

	RelativeTurnToAbusoult(MatrixTem[2],MatrixTem[0]);
	Turn3DTo2D(MatrixTem[0],PT[2]);

	Arc(PT[0].x-T,PT[0].y-T,PT[0].x+T,PT[0].y+T,PT[1].x,PT[1].y,PT[2].x,PT[2].y);

	SelectObject(Old);
}



void DelegationDC3D::DrawCircle( const float* C,float Ra,unsigned int R,unsigned int G,unsigned int B)
{
    CPen Pen;
	Pen.CreatePen(PS_SOLID,1,RGB(R,G,B));
    DrawCircle(C,Ra,Pen);
    Pen.DeleteObject();
}

void DelegationDC3D::DrawCircle( const float* C,float Ra,CPen& Pen)
{
	CPen* Old=SelectObject(&Pen);
	RelativeTurnToAbusoult(C,MatrixTem[0]);
	Turn3DTo2D(MatrixTem[0],PT[0]);
	PT[1].x=static_cast<int>(Ra*sqrt( pow(Rotate[0][0],2)+pow(Rotate[1][0],2)+pow(Rotate[2][0],2) )/2.0*W);
	PT[1].y=static_cast<int>(Ra*sqrt( pow(Rotate[0][1],2)+pow(Rotate[1][1],2)+pow(Rotate[2][1],2) )/2.0*H);
	Ellipse(PT[0].x-PT[1].x,PT[0].y-PT[1].y,PT[0].x+PT[1].x,PT[0].y+PT[1].y);
	SelectObject(Old);
}

void DelegationDC3D::DrawAbsoluteLine(int SX,int SY,int EX,int EY,unsigned int R,unsigned int G,unsigned int B)
{
    CPen Pen;
	Pen.CreatePen(PS_SOLID,1,RGB(R,G,B));
    DrawAbsoluteLine(SX,SY,EX,EY,Pen);
    Pen.DeleteObject();
}

void DelegationDC3D::DrawAbsoluteLine(int SX,int SY,int EX,int EY,CPen& Pen)
{
	CPen* Old=SelectObject(&Pen);
	CDC::MoveTo(SX,SY);
	CDC::LineTo(EX,EY);
	SelectObject(Old);
}

void DelegationDC3D::DrawLine(const float* S ,const  unsigned int *D, unsigned int PointSize,unsigned int AllSize,unsigned int R,unsigned int G,unsigned int B)
{
    CPen Pen;
	Pen.CreatePen(PS_SOLID,1,RGB(R,G,B));
	DrawLine(S,D,PointSize,AllSize,Pen);
    Pen.DeleteObject();
}

void DelegationDC3D::DrawLine(const float* S ,const  unsigned int *D, unsigned int PointSize,unsigned int AllSize,CPen& Pen)
{
	if(PointSize>2&&AllSize>0)
	{
		CPen* Old=SelectObject(&Pen);
		PointPool.resize(PointSize);
		for(unsigned int i=0; i<PointSize; i+=3)
			RelativeTurnToAbusoult(S+i,&PointPool[i]);
		for(unsigned int i=0; i<AllSize; i+=2)
		{
			if(UsePerspective)
			{
				if(LineCutOut3DTo2D(&PointPool[D[i]*3],&PointPool[D[i+1]*3],PT[0],PT[1]))
				{
					MoveTo(PT[0]);
					LineTo(PT[1]);
				}
			}else{
				Turn3DTo2D( &PointPool[D[i]*3],PT[0] );
				Turn3DTo2D( &PointPool[D[i+1]*3],PT[1] );
				MoveTo(PT[0]);
				LineTo(PT[1]);
			}
		}
		SelectObject(Old);
	}

}

void DelegationDC3D::DrawLine(const std::vector<float>& All,unsigned int R,unsigned int G,unsigned int B)
{
	if(All.size()==1)
		DrawPoint(&All[0],R,G,B);
	else{
		CPen Pen;
		Pen.CreatePen(PS_SOLID,1,RGB(R,G,B));
		Pen.DeleteObject();
	}
}

void DelegationDC3D::DrawLine(const std::vector<float>& All,CPen& Pen)
{
	size_t AllSize=All.size()/3;
	if(AllSize>1)
	{
		CPen* Old=SelectObject(&Pen);
		PointPool.resize(AllSize*3);
		for(size_t i=0; i<(AllSize*3); i+=3)
			RelativeTurnToAbusoult(&All[i],&PointPool[i]);
		if(UsePerspective)
		{
			for(size_t i=0; i<((AllSize-1)*3); i+=3)
				if(LineCutOut3DTo2D(&PointPool[i],&PointPool[i+3],PT[0],PT[1]))
				{
					MoveTo(PT[0]);
					LineTo(PT[1]);
				}
		}
		else
		{
			Turn3DTo2D(&PointPool[0],PT[0]);
			MoveTo(PT[0]);
			for(size_t i=3; i<(AllSize*3); i+=3)
			{
				Turn3DTo2D(&PointPool[i],PT[0]);
				LineTo(PT[0]);
			}
		}
		SelectObject(Old);
	}
}

void DelegationDC3D::Indentity()
{
    for(int i=0; i<3; ++i)
        for(int k=0; k<3; ++k)
            if(i==k)
                Rotate[i][k]=1.0;
            else
                Rotate[i][k]=0.0;
    for(int i=0; i<3; ++i)
        Move[i]=0.0;
}
void DelegationDC3D::Translation(float X,float Y,float Z)
{
    for(int i=0; i<3; ++i)
        Move[i]+=Rotate[i][0]*X+Rotate[i][1]*Y+Rotate[i][2]*Z;
}
void DelegationDC3D::DrawTo(int S,int E,CDC* PC)
{
    PC->BitBlt(S,E,W+S,H+E,this,0,0,SRCCOPY);
}
void DelegationDC3D::Clean(unsigned int R,unsigned int G,unsigned int B)
{
    FillSolidRect(0,0,W,H,RGB(R,G,B));
}
void DelegationDC3D::Free()
{
    if(Created)
    {
        Buffer.DeleteObject();
        DeleteDC();
        Created=false;
    }
}
void DelegationDC3D::SetPerspective()
{
    UsePerspective=true;
}
void DelegationDC3D::SetParallel()
{
    UsePerspective=false;
}

void DelegationDC3D::Scale(float X,float Y,float Z)
{
    for(int i=0;i<3;++i)
        Rotate[i][0]*=X;
    for(int i=0;i<3;++i)
        Rotate[i][1]*=Y;
    for(int i=0;i<3;++i)
        Rotate[i][1]*=Z;
    Move[0]*=X;
    Move[1]*=Y;
    Move[2]*=Z;
}

void DelegationDC3D::DrawNetXY(unsigned int Ex,unsigned int Ey,unsigned int R,unsigned int G,unsigned int B)
{
	CPen CP;
	CP.CreatePen(PS_DASH,1,RGB(R,G,B));

	CPen *Old=SelectObject(&CP);

	unsigned int K=W/(Ex+1);
	if(K>0&&K<W)
	{
		for(unsigned int Oi=K;Oi<W;Oi+=K)
		{
			MoveTo(Oi,0);
			LineTo(Oi,H);
		}
	}
	K=H/(Ey+1);
	if(K>0&&K<H)
	{
		for(unsigned int Oi=K;Oi<W;Oi+=K)
		{
			MoveTo(0,Oi);
			LineTo(W,Oi);
		}
	}

	SelectObject(Old);

	CP.DeleteObject();
}