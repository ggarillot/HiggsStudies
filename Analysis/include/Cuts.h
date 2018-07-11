#ifndef Cuts_h
#define Cuts_h

#include "Event.h"

namespace Cut
{

bool isInvis(const Event& event)
{
	if ( event.mass2Jet < 120 && event.nJets < 4 && event.nIsoLep == 0 )
		return true ;
	else
		return false ;
}

namespace Vis
{

bool range(const Event& event)
{
	if ( (event.zMass > 70 && event.zMass < 110) && (event.recMass > 110 && event.recMass < 160)  )
		return true ;
	else
		return false ;
}

bool zPt(const Event& event)
{
	if ( event.zPt < 20 )
		return false ;
	else
		return true ;
}

bool cosThetaMiss(const Event& event)
{
	if ( std::abs(event.cosThetaMiss) < 0.9 )
		return true ;
	else
		return false ;
}

bool cosThetaZ(const Event& event)
{
	if ( std::abs(event.cosThetaZ) < 0.9 )
		return true ;
	else
		return false ;
}

bool massVsEnergy(const Event& event)
{
	if ( event.zMass/(event.z1e + event.z2e) > 0.8 )
		return true ;
	else
		return false ;
}

}
namespace Invis
{

bool range(const Event& event)
{
	if ( (event.zMassVec[2] > 70 && event.zMassVec[2] < 110) && (event.recMassVec[2] > 110 && event.recMassVec[2] < 160)  )
		return true ;
	else
		return false ;
}

bool zPt(const Event& event)
{
	if ( event.zPtVec[2] < 20 )
		return false ;
	else
		return true ;
}

bool cosThetaMiss(const Event& event)
{
	if ( std::abs(event.cosThetaMiss) < 0.65 )
		return true ;
	else
		return false ;
}

bool cosThetaZ(const Event& event)
{
	if ( std::abs(event.cosThetaZVec[2]) < 0.9 )
		return true ;
	else
		return false ;
}

bool massVsEnergy(const Event& event)
{
	if ( event.mass2Jet/event.totalEnergy > 0.8 )
		return true ;
	else
		return false ;
}

}


bool WW_h(const Event& event)
{
	if ( std::sqrt(event.pMiss[0]*event.pMiss[0] + event.pMiss[1]*event.pMiss[1]) > 20 )
	{
		return true ;
	}
	else
	{
//				if ( (event.ww12mass-80.5)*(event.ww12mass-80.5) + (event.ww34mass-79.5)*(event.ww34mass-79.5) > 100 )
				if ( (event.ww12mass-80)*(event.ww12mass-80)+ (event.ww34mass-80)*(event.ww34mass-80) > 100 )
			return true ;
		else
			return false ;
	}
}

bool ZZ_h(const Event& event)
{
	if ( std::sqrt(event.pMiss[0]*event.pMiss[0] + event.pMiss[1]*event.pMiss[1]) > 20 )
	{
		return true ;
	}
	else
	{
//		if ( (event.zz12mass-91.2)*(event.zz12mass-91.2) + (event.zz34mass-90)*(event.zz34mass-90) > 100 )
		if ( (event.zz12mass-90)*(event.zz12mass-90) + (event.zz34mass-90)*(event.zz34mass-90) > 100 )
			return true ;
		else
			return false ;
	}
}

bool WW_sl(const Event& event)
{
	if ( !(event.wwMass3<85 && event.wwMass3>75) || !(event.wwRecMass3>60 && event.wwRecMass3<160) )
		return true ;
	else
		return false ;
}

bool thrust(const Event& event)
{
	if ( event.minorThrust > 0.12 || event.majorThrust > 0.35 )
		return true ;
	else
		return false ;
}

}
#endif //Cuts_h
