#include <iostream>

#include "Event.h"
#include "ChannelInfo.h"
#include "Cuts.h"


int main ( int argc , char** argv )
{
	if ( argc == 1 )
	{
		std::cout << "ERROR : no pol provided " << std::endl ;
		return 1 ;
	}

	double ePol = 0 ;
	double pPol = 0 ;

	std::string weightFileName = "" ;

	if ( argv[1] == std::string("RL") )
	{
		ePol = 0.8 ;
		pPol = -0.3 ;
	}
	else if ( argv[1] == std::string("LR") )
	{
		ePol = -0.8 ;
		pPol = +0.3 ;
	}
	else
	{
		std::cout << "invalid pol" << std::endl ;
		return 1 ;
	}

	constexpr double lumi = 500 ;

	double eR = 0.5*(ePol+1) ;
	double eL = 0.5*(1-ePol) ;

	double pR = 0.5*(pPol+1) ;
	double pL = 0.5*(1-pPol) ;

	double weightLR = eL*pR ;
	double weightRL = eR*pL ;

	double weightLL = eL*pL ;
	double weightRR = eR*pR ;

	std::cout << "Luminosity : " << lumi << std::endl ;
	std::cout << "Polarisation : (" << 100*ePol << "% , " << 100*pPol << "%)" << std::endl ;

	std::cout << "weightLR : " << weightLR << std::endl ;
	std::cout << "weightRL : " << weightRL << std::endl ;
	std::cout << "weightLL : " << weightLL << std::endl ;
	std::cout << "weightRR : " << weightRR << std::endl ;


	std::vector<int> processIDVec = {106485,106551,106577,106573,106575,106607,106486,106552,106578,106574,106576,106608} ;

	std::string dir = argv[2] ;




	return 0 ;
}
