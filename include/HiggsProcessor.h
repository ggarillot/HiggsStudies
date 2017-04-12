#ifndef HiggsProcessor_h
#define HiggsProcessor_h


#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <cstring>
#include <EVENT/CalorimeterHit.h>
#include <vector>
#include <map>
#include <limits>


using namespace lcio ;
using namespace marlin ;

class HiggsProcessor : public Processor
{
	public :

		virtual Processor*  newProcessor() { return new HiggsProcessor ; }

		HiggsProcessor() ;

		virtual void init() ;

		virtual void processRunHeader( LCRunHeader* run ) ;

		virtual void processEvent( LCEvent * evt ) ;

		virtual void check( LCEvent * evt ) ;

		virtual void end() ;

		std::vector<int> findDecayMode(LCCollection* mcCol) ;



	protected :

		int _nRun ;
		int _nEvt ;


	private :


} ;


#endif //HiggsProcessor_h
