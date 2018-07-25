#ifndef Cuts_h
#define Cuts_h

#include "Event.h"

namespace Cut
{

bool isInvis(const Event& event)
{
//	if ( event.mass2Jet < 110 && event.nJets < 4 )
	if ( event.mass2Jet < 110 && event.y34 > 2.22 )
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
	if ( event.zPt > 21 )
		return true ;
	else
		return false ;
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
	if ( event.zPtVec[2] > 21 )
		return true ;
	else
		return false ;
}

bool cosThetaZ(const Event& event)
{
	if ( std::abs(event.cosThetaZVec[2]) < 0.6 )
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

bool thrust(const Event& event)
{
	if ( event.majorThrust > 0.55 && event.majorThrust < 0.65 && event.minorThrust < 0.12 )
		return false ;
	else
		return true ;
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
		if ( (event.ww12mass-80.43)*(event.ww12mass-80.43) + (event.ww34mass-80)*(event.ww34mass-80) > 100 )
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
		if ( (event.zz12mass-91.2)*(event.zz12mass-91.2) + (event.zz34mass-90)*(event.zz34mass-90) > 100 )
			return true ;
		else
			return false ;
	}
}

bool WW_sl(const Event& event)
{
	if ( !(event.wwMass3>75 && event.wwMass3<85) || !(event.wwRecMass3>60 && event.wwRecMass3<160) )
		return true ;
	else
		return false ;
}

bool thrust(const Event& event)
{
	if ( event.minorThrust > 0.10 || event.majorThrust > 0.40 )
		return true ;
	else
		return false ;
}


unsigned int finalCut(const Event& event)
{
	if ( event.failedStep != 0 )
		return 1 ;

	if ( Cut::isInvis(event) )
	{
		if ( !event.validVec[2] )
			return 1 ;
		if ( !Cut::Invis::range(event) )
			return 2 ;


		if ( !Cut::Invis::zPt(event) )
			return 3 ;
		if ( !Cut::Invis::cosThetaZ(event) )
			return 3 ;

		if ( !Cut::Invis::massVsEnergy(event) )
			return 4 ;


		//		if ( !Cut::Invis::thrust(event) )
		//			return 5 ;


		if ( !Cut::WW_h(event) )
			return 6 ;

		if ( !Cut::ZZ_h(event) )
			return 7 ;



		if ( !Cut::WW_sl(event) )
			return 8 ;
		if ( event.nIsoLep > 0 )
			return 8 ;

		return 9 ;
	}
	else
	{
		if ( !Cut::Vis::range(event) )
			return 2 ;


		if ( !Cut::Vis::zPt(event) )
			return 3 ;
		if ( !Cut::Vis::cosThetaMiss(event) )
			return 3 ;
		if ( !Cut::Vis::cosThetaZ(event) )
			return 3 ;

		if ( !Cut::Vis::massVsEnergy(event) )
			return 4 ;

		if ( event.nIsoLep == 0 )
			if ( !Cut::thrust(event) )
				return 5 ;

		if ( !Cut::WW_h(event) )
			return 6 ;

		if ( !Cut::ZZ_h(event) )
			return 7 ;

		if ( !Cut::WW_sl(event) )
			return 8 ;

		return 9 ;
	}
}



}
#endif //Cuts_h
