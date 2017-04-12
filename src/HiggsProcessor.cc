#include "HiggsProcessor.h"


#include <iostream>

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <EVENT/LCParameters.h>
#include <UTIL/CellIDDecoder.h>
#include <IMPL/MCParticleImpl.h>


#include "marlin/VerbosityLevels.h"
#include <string.h>


using namespace lcio ;
using namespace marlin ;

HiggsProcessor aHiggsProcessor ;

HiggsProcessor::HiggsProcessor() : Processor("HiggsProcessor")
{

}

void HiggsProcessor::init()
{

}


std::vector<int> HiggsProcessor::findDecayMode(LCCollection* mcCol)
{
	std::vector<int> toReturn ;
	if ( mcCol->getNumberOfElements() < 10 )
		return toReturn ;

	MCParticleImpl* part = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 9 ) ) ;

	MCParticleVec vec = part->getDaughters() ;


	for ( unsigned int i = 0 ; i < vec.size() ; ++i )
		toReturn.push_back( vec.at(i)->getPDG() ) ;

	return toReturn ;
}


void HiggsProcessor::processRunHeader(LCRunHeader* run)
{
	_nRun++ ;
	_nEvt = 0 ;
}


void HiggsProcessor::processEvent(LCEvent * evt)
{
	std::vector<int> vec = findDecayMode( evt->getCollection("MCParticle") ) ;

	for ( unsigned int i = 0 ; i < vec.size() ; ++i )
		std::cout << vec.at(i) << " " ;

	std::cout << std::endl ;
}


void HiggsProcessor::check(LCEvent * evt)
{
}


void HiggsProcessor::end()
{

}
