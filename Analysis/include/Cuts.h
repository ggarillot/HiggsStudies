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

bool rangeInvis(const Event& event)
{
	if ( (event.zMassVec[2] > 70 && event.zMassVec[2] < 110) && (event.recMassVec[2] > 100 && event.recMassVec[2] < 160)  )
		return true ;
	else
		return false ;
}

bool WW_h(const Event& event)
{
	if ( (event.ww12mass-80)*(event.ww12mass-80) + (event.ww34mass-80)*(event.ww34mass-80) > 100 )
		return true ;
	else
		return false ;
}

bool ZZ_h(const Event& event)
{
	if ( (event.zz12mass-91)*(event.zz12mass-91) + (event.zz34mass-91)*(event.zz34mass-91) > 80 )
		return true ;
	else
		return false ;
}

bool WW_sl(const Event& event)
{
	if ( !(event.wwMass3<85 && event.wwMass3>75) || !(event.wwRecMass3>60 && event.wwRecMass3<160) )
		return true ;
	else
		return false ;
}

bool range(const Event& event)
{
	if ( (event.zMass > 70 && event.zMass < 110) && (event.recMass > 100 && event.recMass < 160)  )
		return true ;
	else
		return false ;
}

bool rangeLep(const Event& event)
{
	if ( (event.zMass > 60 && event.zMass < 110) && (event.recMass > 100 && event.recMass < 160)  )
		return true ;
	else
		return false ;
}

bool angle(const Event& event)
{
	if ( event.zPt > 20 && std::abs(event.cosThetaMiss) < 0.9 )
		return true ;
	else
		return false ;
}

bool thrust(const Event& event)
{
	if ( event.minorThrust > 0.12 || event.majorThrust>0.35 )
		return true ;
	else
		return false ;
}

bool mass2Jet(const Event& event)
{
	if ( event.mass2Jet > 100 )
		return true ;
	else
		return false ;
}

}
#endif //Cuts_h
