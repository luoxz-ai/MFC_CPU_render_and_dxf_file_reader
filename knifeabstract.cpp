#include "stdafx.h"
#include "knifeabstract.h"
std::wostream& operator<< (std::wostream& L,KnifeTrajectory& P)
{
	if(P.Type!=KNI_UNKNOW)
	{
		L<<L" < Knife Data - Type: ";
		switch( P.Type )
		{
		case KNI_LINE:
			L<<L"Line";
			break;
		case KNI_CW_ARC:
			L<<L"CW_ARC";
			break;
		case KNI_CCW_ARC:
			L<<L"CWW_ARC";
			break;
		case KNI_CW_ELL:
			L<<L"CW_FLL";
			break;
		case ENI_CCW_ELL:
			L<<L"CCW_ELL";
			break;
		}
		L<<L" X0: "<<P.X0<<L" Y0: "<<P.Y0<<L" Z0: "<<P.Z0;
		L<<L" X1: "<<P.X1<<L" Y1: "<<P.Y1<<L" Z1: "<<P.Z1;
		L<<L" RR: "<<P.RR;
		L<<L" I:  "<<P.I<<L" J:  "<<P.J<<L" K:  "<<P.K;
		L<<L" Layer: "<<P.Layer;
		L<<L" Flag:  "<<P.Flag;
		L<<L" A:     "<<P.A<<L" B:     "<<P.B;
		L<<L" ALG:   "<<P.AGL;
	}
	return L;
}
std::wostream& operator<< ( std::wostream& L, KnifeAbstract& KA)
{
	for(std::list<KnifeTrajectory>::iterator Po=KA.Trajectory.begin();Po!=KA.Trajectory.end();++Po)
	{
		L<<*Po<<std::endl;
	}
	return L;
}


void KnifeAbstract::ForEach( bool (*F )(KnifeTrajectory&,void*),void* D)
{
	for(std::list<KnifeTrajectory>::iterator Po=Trajectory.begin();Po!=Trajectory.end();++Po)
		if( (*F)(*Po,D)) break;
}

bool KnifeAbstract::ReadKnifTarejectory( const std::wstring & Ad)
{
	Trajectory.clear();
	KnifeTrajectory KT;
	KT.Reset();
	int Count=0;
	std::wfstream File(Ad.c_str(),std::ios::in);
	std::wstring Key;
	std::wstringstream SS;
	for(File>>Key;File.good();File>>Key)
	{
		if(Key==L"G01")
		{
			if(Count!=0)
			{
				Trajectory.push_back(KT);
				KT.Reset();
			}
			KT.Type=KNI_LINE;
			Count=1;
		}else if(Key==L"G02")
		{
			if(Count!=0)
			{
				Trajectory.push_back(KT);
				KT.Reset();
			}
			KT.Type=KNI_CW_ARC;
			Count=1;
		}else if(Key==L"G03")
		{
			if(Count!=0)
			{
				Trajectory.push_back(KT);
				KT.Reset();
			}
			KT.Type=KNI_CCW_ARC;
			Count=1;
		}else if(Key==L"G04")
		{
			if(Count!=0)
			{
				Trajectory.push_back(KT);
				KT.Reset();
			}
			KT.Type=KNI_CW_ELL;
			Count=1;
		}else if(Key==L"G05")
		{
			if(Count!=0)
			{
				Trajectory.push_back(KT);
				KT.Reset();
			}
			KT.Type=KNI_CW_ELL;
			Count=1;
		}else{
			switch(Count)
			{
			case 1:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.X0;
				break;
			case 2:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.Y0;
				break;
			case 3:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.Z0;
				break;
			case 4:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.X1;
				break;
			case 5:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.Y1;
				break;
			case 6:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.Z1;
				break;
			case 7:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.RR;
				break;
			case 8:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.I;
				break;
			case 9:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.J;
				break;
			case 10:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.K;
				break;
			case 11:
				KT.Layer=Key;
				break;
			case 12:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.Flag;
				break;
			case 13:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.A;
				break;
			case 14:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.B;
				break;
			case 15:
				SS.str(L"");
				SS.clear();
				SS<<Key;
				SS>>KT.AGL;
				break;
			case 16:
				break;
			default:
				return false;
			}
			++Count;
		}
	}
	if(Count==17)
	{
		Trajectory.push_back(KT);
		return true;
	}else{
		return false;
	}
}