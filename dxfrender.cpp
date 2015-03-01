#include "stdafx.h"
#include "dxfrender.h"
bool DxfRenderPlu::ReCalculate( DxfAbstract& DFE)
{
	if(FindOne&&FindTwo)
	{
		Entities*& Po=One;
		Entities*& Po2=Two;
		float PX1= Po->Li.Sy - Po->Li.Ey;
		float PX2= Po2->Li.Sy - Po2->Li.Ey;
		float PY1= - Po->Li.Sx - Po->Li.Ex;
		float PY2= - Po2->Li.Sx - Po2->Li.Ex;
		float PE1= PX1 * Po->Li.Ex  -  PY1 * Po->Li.Ey ;
		float PE2= PX2 * Po2->Li.Ex  -  PY2 * Po2->Li.Ey ;
		float Tem=PX1*PY2-PY1*PX2;
		if(Tem>0.00001||Tem<-0.00001)
		{
			float CX=(PE1*PY2-PY1*PE2)/Tem;
			float CY=(PX1*PE2-PE1*PX2)/Tem;
			DFE.Move(CX,CY);
			DFE.ReCalculate();
		}else{
			DFE.AutoSetMarkPoint();
		}
		GetRange(DFE);
		return true;
	}
	DFE.AutoSetMarkPoint();
	GetRange(DFE);
	return false;
}
bool DxfRenderPlu::FindTwoLine(Entities& L,void* Data)
{
	if(!FindOne)
	{
		if(  L.Type==DXF_EN_LINE )
		{
			One=&L;
			FindOne=true;
		}
	}else{
		if(L.Type==DXF_EN_LINE )
		{
			Two=&L;
			FindTwo=true;
			return true;
		}
	}
	return false;
}

void DxfRenderPlu::GetRange(DxfAbstract& DF)
{
	DF.GetRangeXY(MaxX,MinX,MaxY,MinY);
	BoardX=MaxX-MinX;
	BoardY=MaxY-MinY;
	if(BoardX>BoardY)
	{
		EachMax=BoardX;
		BoardY=MinY-(BoardX-BoardY)/2.0f;
		BoardX=MinX;
	}else
	{
		EachMax=BoardY;
		BoardX=MinX-(BoardY-BoardX)/2.0f;
		BoardY=MinY;
	}
}

void DxfRenderPlu::AutoAdject(DelegationDC3D& DP)
{
	DP.Indentity();
	//DP.Translation(-2.9f,-2.9f,0.0f);
	DP.Scale(1.8f/EachMax,1.8f/EachMax,1.0f);
	//DP.Translation()
	DP.Translation(-EachMax/2.0f-BoardX,-EachMax/2.0f-BoardY,0.0);
	//DP.Translation(-0.9,-0.9,0.0);
}

void DxfRender::AutoAdject()
{
	Temporary.AutoAdject(*this);
}

void DxfRenderPlu::Render(Entities& E,DelegationDC3D& DP)
{
	static float P[10];
	static unsigned int Order[]={0,1};
	switch(E.Type)
	{
	case DXF_EN_LINE:
		/*P[0]=(E.Li.Sx-BoardX)/(EachMax)*1.8f-0.9f;
		P[1]=-(E.Li.Sy-BoardY)/(EachMax)*1.8f+0.9f;
		P[2]=0.0f;
		P[3]=(E.Li.Ex-BoardX)/(EachMax)*1.8f-0.9f;
		P[4]=-(E.Li.Ey-BoardY)/(EachMax)*1.8f+0.9f;
		P[5]=0.0;*/
		P[0]=E.Li.Sx;
		P[1]=E.Li.Sy;
		P[2]=0.0f;
		P[3]=E.Li.Ex;
		P[4]=E.Li.Ey;
		P[5]=0.0;
		DP.DrawLine(P,Order,6,2);
		break;
	case DXF_EN_POINT:
		/*P[0]=(E.Po.X-BoardX)/(EachMax)*1.8f-0.9f;
		P[1]=-(E.Po.Y-BoardY)/(EachMax)*1.8f+0.9f;
		P[2]=0.0f;*/
		P[0]=E.Po.X;
		P[1]=E.Po.Y;
		P[2]=0.0f;
		DP.DrawPoint(P);
		break;
	case DXF_EN_CIRCLE:
		{
			/*P[0]=(E.Ci.Cx-BoardX)/(EachMax)*1.8f-0.9f;
			P[1]=-(E.Ci.Cy-BoardY)/(EachMax)*1.8f+0.9f;
			P[2]=0.0;
			P[3]=E.Ci.R/EachMax*1.8f;*/
			P[0]=E.Ci.Cx;
			P[1]=E.Ci.Cy;
			P[2]=0.0;
			P[3]=E.Ci.R;
			DP.DrawCircle(P,P[3]);
		}
		break;
	case DXF_EN_ARC:
		/*P[0]=(E.Ar.Cx-BoardX)/(EachMax)*1.8f-0.9f;
		P[1]=-(E.Ar.Cy-BoardY)/(EachMax)*1.8f+0.9f;
		P[2]=0.0;
		P[3]=E.Ar.R/EachMax*1.8f;*/
		P[0]=E.Ar.Cx;
		P[1]=E.Ar.Cy;
		P[2]=0.0;
		P[3]=E.Ar.R;
		DP.DrawArc(P,P[3],E.Ar.Sa,E.Ar.Ea);
		break;
	}
}
void DxfRenderPlu::Render(KnifeTrajectory& E,DelegationDC3D& DP)
{
	static float P[10];
	static unsigned int Order[]={0,1};
	switch(E.Type)
	{
	case KNI_LINE:
		P[0]=E.X0;
		P[1]=E.Y0;
		P[2]=E.Z0;
		P[3]=E.X1;
		P[4]=E.Y1;
		P[5]=E.Z1;
		DP.DrawLine(P,Order,6,2);
		break;
	case KNI_CW_ARC:
		P[0]=E.X1;
		P[1]=E.Y1;
		P[2]=E.Z1;
		P[3]=E.X0;
		P[4]=E.Y0;
		P[5]=E.Z0;
		P[6]=E.X0+E.I;
		P[7]=E.Y0+E.J;
		P[8]=E.Z0+E.K;
		DP.DrawArcCenter(P+6,P,P+3);
		break;
	case KNI_CCW_ARC:
		P[0]=E.X0;
		P[1]=E.Y0;
		P[2]=E.Z0;
		P[3]=E.X1;
		P[4]=E.Y1;
		P[5]=E.Z1;
		P[6]=E.X0+E.I;
		P[7]=E.Y0+E.J;
		P[8]=E.Z0+E.K;
		DP.DrawArcCenter(P+6,P,P+3);
		break;
	case KNI_CW_ELL:
	case ENI_CCW_ELL:
		P[0]=E.X0;
		P[1]=E.Y0;
		P[2]=E.Z0;
		P[3]=E.RR;
		DP.DrawCircle(P,P[3]);
		break;
	}
}
void DxfRenderPlu::Reset()
{
	FindOne=false,FindTwo=false;
}

bool DxfRender::Draw(Entities& E,void* )
{
    Temporary.Render(E,*this);
    return false;
}
bool DxfRender::DrawKnif(KnifeTrajectory& E,void* )
{
	Temporary.Render(E,*this);
	return false;
}


bool DxfRender::ReadDxf( const std::wstring& Adress)
{
    Temporary.Reset();
    if(DER.ReadDxfFile(Adress))
    {
        DER.LayerForEach(L"MARK",&DxfRenderPlu::FindTwoLine,&Temporary,NULL);
        Temporary.ReCalculate(DER);
        return true;
    }
    return false;
}
bool DxfRender::ReadKnif( const std::wstring& Adress)
{
	//Temporary.Reset();
	return KA.ReadKnifTarejectory(Adress);
}
/*void DxfRender::Create(unsigned int W,unsigned int H,CDC* pDC)
{
    DP.Create(W,H,pDC);
}*/
void DxfRender::DrawEntities()
{
    DER.ForEach(&DxfRender::Draw,this,NULL);
}

void DxfRender::DrawKnif()
{
	KA.ForEach(&DxfRender::DrawKnif,this,NULL);
}

std::wostream& operator<< (std::wostream& O,const DxfRender& DFR )
{
	return operator<<(O,DFR.DER);
}
