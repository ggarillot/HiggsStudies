#ifndef Cuts_h
#define Cuts_h

#include "Event.h"

namespace Cut
{

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
	if ( (event.zMass > 70 && event.zMass < 110) && (event.recMass > 100 && event.recMass < 200)  )
		return true ;
	else
		return false ;
}

bool angle(const Event& event)
{
	if ( event.zPt < 20 && std::abs(event.cosThetaMiss) < 0.9 )
		return true ;
	else
		return false ;
}

}
#endif //Cuts_h
