#ifndef KNIFABSTRACT_H_INCLUDED
#define KNIFABSTRACT_H_INCLUDED
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>
enum KnifeTrajectoryType
{
    KNI_UNKNOW,
    KNI_LINE,
    KNI_CW_ARC,
    KNI_CCW_ARC,
    KNI_CW_ELL,
    ENI_CCW_ELL
};

struct KnifeTrajectory
{
    KnifeTrajectoryType Type;
    float X0,Y0,Z0;
    float X1,Y1,Z1;
    float RR;
    float I,J,K;
    std::wstring Layer;
    int Flag;
    float A,B;
    float AGL;
    void Reset()
    {
        Type=KNI_UNKNOW;
        X0=0,Y0=0,Z0=0;
        X1=0,Y1=0,Z1=0;
        RR=0;
        I=0,J=0,K=0;
        Layer.clear();
        Flag=0;
        A=0,B=0;
        AGL=0;
    }
};

std::wostream& operator<< (std::wostream& L,KnifeTrajectory& P);


class KnifeAbstract
{

public:
    std::list<KnifeTrajectory> Trajectory;
    bool ReadKnifTarejectory( const std::wstring & Ad);
	template<typename T>
	void ForEach( bool (T::* F)(KnifeTrajectory&,void*),T* v,void* d) 
	{
		for(std::list<KnifeTrajectory>::iterator Po=Trajectory.begin();Po!=Trajectory.end();++Po)
		{
			if( (v->*F)(*Po,d)) break;
		}
	}
	void ForEach( bool (*)(KnifeTrajectory&,void*),void*);
};
std::wostream& operator<< ( std::wostream& L, KnifeAbstract& KA);

#endif // KNIFABSTRCT_H_INCLUDED
