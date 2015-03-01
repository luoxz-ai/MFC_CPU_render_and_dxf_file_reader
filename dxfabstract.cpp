#include "stdafx.h"
#include "dxfabstract.h"
void DxfAbstract::ReCalculate()
{
	for(MapIterator L=LayerEntities.begin();L!=LayerEntities.end();++L)
	{
		for(ListIterator P=L->second.begin();P!=L->second.end();++P)
		{
			if(P==L->second.begin()&&L==LayerEntities.begin())
			{
				MaxX=P->Un.X;
				MinX=MaxX;
				MaxY=P->Un.Y;
				MinY=MaxY;
			}
			switch( P->Type )
			{
			case DXF_EN_POINT:
				Select(MinX,P->Po.X,MaxX,MaxX,MinX);
				Select(MinY,P->Po.Y,MaxY,MaxY,MinY);
				break;
			case DXF_EN_LINE:
				{
					Select(MinX,P->Li.Sx,MaxX,MaxX,MinX);
					Select(MinX,P->Li.Ex,MaxX,MaxX,MinX);
					Select(MinY,P->Li.Sy,MaxY,MaxY,MinY);
					Select(MinY,P->Li.Ey,MaxY,MaxY,MinY);
				}
				break;
			case DXF_EN_CIRCLE:
				{
					Select(MinX,P->Ci.Cx+P->Ci.R,MaxX,MaxX,MinX);
					Select(MinX,P->Ci.Cx-P->Ci.R,MaxX,MaxX,MinX);
					Select(MinY,P->Ci.Cy+P->Ci.R,MaxY,MaxY,MinY);
					Select(MinY,P->Ci.Cy+P->Ci.R,MaxY,MaxY,MinY);
				}
				break;
			case DXF_EN_ARC:
				{
					float SC=cos(P->Ar.Sa/180.0f*3.1415926f);
					float SS=sin(P->Ar.Sa/180.0f*3.1415926f);
					float EC=cos(P->Ar.Ea/180.0f*3.1415926f);
					float ES=sin(P->Ar.Ea/180.0f*3.1415926f);
					Select(MinX,P->Ar.Cx+SC*P->Ar.R,MaxX,MaxX,MinX);
					Select(MinX,P->Ar.Cx+EC*P->Ar.R,MaxX,MaxX,MinX);
					Select(MinY,P->Ar.Cy+SS*P->Ar.R,MaxY,MaxY,MinY);
					Select(MinY,P->Ar.Cy+ES*P->Ar.R,MaxY,MaxY,MinY);
					float SA=P->Ar.Sa;
					float EA=P->Ar.Ea;
					if(EA<SA) EA+=360.0f;
					if(		SA<0.0&&EA>0.0				||	SA<180.0*2.0 && EA>180*2.0)    Select(MinX,P->Ar.Cx+P->Ar.R,MaxX,MaxX,MinX);
					if(		SA<180.0/2&&EA>180.0/2		||	SA<180.0*2.5 && EA>180.0*2.5 )  Select(MinY,P->Ar.Cy+P->Ar.R,MaxY,MaxY,MinY);
					if(		SA<180.0&&EA>180.0			||  SA<180.0*3.0 && EA>180.0*3.0   )  Select(MinX,P->Ar.Cx-P->Ar.R,MaxX,MaxX,MinX);
					if(		SA<180.0*1.5&&EA>180.0*1.5	||	SA<180.0*3.5 && EA>180.0*3.5) Select(MinY,P->Ar.Cy-P->Ar.R,MaxY,MaxY,MinY);
				}
				break;
			}
		}
	}
}

void DxfAbstract::AutoSetMarkPoint()
{
	ReCalculate();
	Move(-MinX,-MinY,0);
}

void DxfAbstract::Move(float X,float Y,float Z)
{
	MaxX+=X;MinX+=X;MaxY+=Y;MinY+=Y;
	for(MapIterator Find=LayerEntities.begin();Find!=LayerEntities.end();++Find)
		for(ListIterator Kp=Find->second.begin();Kp!=Find->second.end();++Kp)
		{
			if(Kp->Type==DXF_EN_LINE)
			{
				Kp->Li.Sx+=X;
				Kp->Li.Ex+=X;
				Kp->Li.Sy+=Y;
				Kp->Li.Ey+=Y;
			}else{
				Kp->Un.X+=X;
				Kp->Un.Y+=Y;
			}
		}
}







bool DxfAbstract::LayerForEach(const std::wstring& P,bool (*FP)(Entities&,void*),void* Data)
{
	MapIterator Find=LayerEntities.find(P);
	if(Find==LayerEntities.end()) return false;
	for(ListIterator Ite=Find->second.begin();Ite!=Find->second.end();++Ite)
		if((*FP)(*Ite,Data))     return true;
	return true;
}




bool DxfAbstract::ForEach(bool (*FP)(Entities&,void*),void * Data)
{
	if(LayerEntities.empty()) return true;
	for(MapIterator Find=LayerEntities.begin();Find!=LayerEntities.end();++Find)
		for(ListIterator Kp=Find->second.begin();Kp!=Find->second.end();++Kp)
			if((*FP)(*Kp,Data)) return true;
	return true;
}
void DxfAbstract::GetRangeXY(float& MaX,float & MiX,float& MaY,float& MiY)
{
	MaX=MaxX;
	MiX=MinX;
	MaY=MaxY;
	MiY=MinY;
}
bool DxfAbstract::ReadDxfFile(const std::wstring& L)
{
	std::wifstream Go;
	std::locale Lo("");
	Go.imbue(Lo);
	Go.open(L.c_str());
	if(!LayerEntities.empty())	LayerEntities.clear();
	if(!Go.is_open())	return false;
	bool FindBlock=false;
	Entities En;
	std::wstring Layer;
	std::wstring Pre;
	std::wstring App;
	std::wstringstream SS;
	En.Type=DXF_EN_NEW;
	while(GetDoubleLine(Go,Pre,App))
	{
		if(!FindBlock)      {if(App==L"ENTITIES") FindBlock=true;}
		else{
			if(Pre==L"0")
			{
				if(App==L"ENDSES")           break;
				if(En.Type!=DXF_EN_NEW)              LayerEntities[Layer].push_back(En);
				if(App==L"LINE")             En.Type=DXF_EN_LINE;
				else if(App==L"CIRCLE")      En.Type=DXF_EN_CIRCLE;
				else if(App==L"ARC")         En.Type=DXF_EN_ARC;
				else if(App==L"POINT")       En.Type=DXF_EN_POINT;
				else                        En.Type=DXF_EN_NEW;
			}else if(Pre==L"8")              Layer=App;
			else if(Pre==L"10")
			{
				SS.clear();
				SS<<App;
				SS>>En.Un.X;
			}else if(Pre==L"11")
			{
				SS.clear();
				SS<<App;
				SS>>En.Li.Ex;
			}else if(Pre==L"20")
			{
				SS.clear();
				SS<<App;
				SS>>En.Un.Y;
				//En.Un.Y=-En.Un.Y;
			}else if(Pre==L"21")
			{
				SS.clear();
				SS<<App;
				SS>>En.Li.Ey;
				//En.Li.Ey=-En.Li.Ey;
			}else if(Pre==L"40")
			{
				SS.clear();
				SS<<App;
				SS>>En.Ci.R;
			}else if(Pre==L"50")
			{
				SS.clear();
				SS<<App;
				SS>>En.Ar.Sa;
			}else if(Pre==L"51")
			{
				SS.clear();
				SS<<App;
				SS>>En.Ar.Ea;
			}
		}
	}
	return true;
}


std::wostream& operator<< (std::wostream& L,const DxfAbstract& DER)
{
	L<<L"Min X: "<<DER.MinX<<L" Max X: "<<DER.MaxX<<L" Min Y: "<<DER.MinY<<L" Max Y: "<<DER.MaxY<<std::endl;
	for(DxfAbstract::MapConstIterator Tem=DER.LayerEntities.begin();Tem!=DER.LayerEntities.end();++Tem)
	{
		L<<L"In Layer: "<<Tem->first<<std::endl;
		for(DxfAbstract::ListConstIterator Ko=Tem->second.begin();Ko!=Tem->second.end();++Ko)
		{
			L<<L"         ";
			switch(Ko->Type)
			{
			case DXF_EN_LINE:
				L<<L"Line With < SX:"<<Ko->Li.Sx<<L" SY:"<<Ko->Li.Sy<<L" EX:"<<Ko->Li.Ex<<L" EY:"<<Ko->Li.Ey<<L" >";
				break;
			case DXF_EN_CIRCLE:
				L<<L"Circle With < CX:"<<Ko->Ci.Cx<<L" CY:"<<Ko->Ci.Cy<<L" R:"<<Ko->Ci.R<<L" >";
				break;
			case DXF_EN_ARC:
				L<<L"Arc With < CX:"<<Ko->Ar.Cx<<L" CY:"<<Ko->Ar.Cy<<L" R:"<<Ko->Ar.R<<L" SA:"<<Ko->Ar.Sa<<L" EA:"<<Ko->Ar.Ea<<L" >";
				break;
			case DXF_EN_POINT:
				L<<L"Point With < X:"<<Ko->Po.X<<L" Y:"<<Ko->Po.Y<<L" >";
				break;
			}
			L<<std::endl;
		}
	}
	return L;
}
void DxfAbstract::Select(float P1,float P2,float P3,float& Max,float& Min)
{
	if(P1<P2)   std::swap(P1,P2);
	if(P2<P3)   std::swap(P2,P3);
	if(P1<P2)   std::swap(P1,P2);
	Max=P1;
	Min=P3;
}

bool DxfAbstract::GetDoubleLine(std::wifstream& F,std::wstring& P,std::wstring& D)
{
	P.clear();
	D.clear();
	wchar_t T;
	bool PReady=false;
	for(F.get(T);F.good();F.get(T))
	{
		if(T!=L' ')
		{
			if(!PReady)
			{
				if(T!=L'\n') P+=T;
				else    PReady=true;
			}else{
				if(T!=L'\n') D+=T;
				else    return true;
			}
		}
	}
	if(!PReady) return false;
	return true;
}

