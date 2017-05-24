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

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>
#include <fastjet/ClusterSequence.hh>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>


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
		double getISREnergy(LCCollection* mcCol) ;
		CLHEP::HepLorentzVector getZLorentzVectorMC(LCCollection* mcCol) ;
		CLHEP::HepLorentzVector getHLorentzVectorMC(LCCollection* mcCol) ;

		void getLinkTest(LCCollection* linkCol) ;

		std::map<int,float> mcOriginOfJet(fastjet::PseudoJet& jet) ;


	protected :

		int _nRun ;
		int _nEvt ;

		LCEvent* currentEvt ;

		std::string mcPartColName ;
		std::string recoPartColName ;
		std::string linkColName ;

		std::vector<fastjet::PseudoJet> particles ;
		std::vector<fastjet::PseudoJet> jets ;

		std::string rootFileName ;
		TFile* file ;
		TTree* tree ;

		int decayID ;
		double energy4Jets ;
		double ISREnergy ;
		double zMassMC ;
		double recMassMC ;

		double zMass ;
		double recMass ;
		double recMass2 ;

		double ww12mass ;
		double ww34mass ;

		double zz12mass ;
		double zz34mass ;

		double pX ;
		double pY ;
		double pZ ;
		double deltaP ;

		std::vector<double> yValueVec ;


	private :


} ;


#endif //HiggsProcessor_h
