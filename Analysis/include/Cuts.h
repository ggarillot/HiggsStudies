#ifndef Cuts_h
#define Cuts_h

#include "Event.h"

namespace Cut
{

bool WW_h(const Event& event)
{
	if ( (event.ww12mass-80)*(event.ww12mass-80) + (event.ww34mass-80)*(event.ww34mass-80) > 120 )
		return true ;
	else
		return false ;
}

bool ZZ_h(const Event& event)
{
	if ( (event.zz12mass-91)*(event.zz12mass-91) + (event.zz34mass-91)*(event.zz34mass-91) > 120 )
		return true ;
	else
		return false ;
}

bool WW_sl(const Event& event)
{
	if ( !(event.wwMass3<90 && event.wwMass3>70) || !(event.wwRecMass3>60 && event.wwRecMass3<120) )
		return true ;
	else
		return false ;
}

bool range(const Event& event)
{
	if ( (event.zMass > 70 && event.zMass < 110) && (event.recMass > 100 && event.recMass < 200) && std::abs(event.cosThetaMiss) < 0.9 )
		return true ;
	else
		return false ;
}

}
#endif //Cuts_h
