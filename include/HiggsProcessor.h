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

#include <IMPL/ReconstructedParticleImpl.h>
#include <IMPL/MCParticleImpl.h>
#include <IMPL/LCRelationImpl.h>
#include <UTIL/LCRelationNavigator.h>

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>
#include <fastjet/ClusterSequence.hh>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include "Particle.h"
#include "Jet.h"


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

		double getZEnergyMC() ;
		double getCosAngleBetweenZJetsMC() ;


		void computeOriginMap() ;

		std::map<int,float> mcOriginOfParticle(ReconstructedParticleImpl* recoPart) ;

		std::map<int,float> mcOriginOfJet(const fastjet::PseudoJet& jet, fastjet::ClusterSequence& cs) ;


	protected :

		int _nRun ;
		int _nEvt ;

		LCEvent* currentEvt ;

		std::string mcPartColName ;
		std::string recoPartColName ;
		std::string linkColName ;

		LCCollection* mcCol ;
		LCCollection* linkCol ;
		LCCollection* recoCol ;

		std::vector<ReconstructedParticleImpl*> recoParticles ;
		std::vector<fastjet::PseudoJet> particles ;
//		std::vector<fastjet::PseudoJet> jets ;

//		ParticleVec particles ;


		std::map<ReconstructedParticleImpl* , std::map<int,float> > originMap ;

		std::string rootFileName ;
		TFile* file ;
		TTree* tree ;

		double sqrtS ;

		int decayID ;
		double totalEnergy ;
		double energy4Jets ;
		double ISREnergy ;
		double zMassMC ;
		double recMassMC ;

		double zEnergyMC ;
		double cosAngleBetweenZJetsMC ;
		double cosAngleBetweenHJetsMC ;
		double zPhiMC ;
		double zRapMC ;

		double hPhiMC ;
		double hRapMC ;

		double zMass ;
		double recMass ;
		double recMass2 ;
		double zEnergy ;
		double cosAngleBetweenZJets ;
		double cosAngleBetweenHJets ;
		double zPhi ;
		double zRap ;

		double hPhi ;
		double hRap ;


		double hDiJetMass ;

		double ww12mass ;
		double ww34mass ;

		double zz12mass ;
		double zz34mass ;

		double zPurity ;
		double hCont ;

		double idealRecoZmass ;
		double idealRecoHmass ;
		double idealRecoRecMass ;



		double pX ;
		double pY ;
		double pZ ;
		double deltaP ;

		std::vector<double> yValueVec ;


	private :


} ;


#endif //HiggsProcessor_h
