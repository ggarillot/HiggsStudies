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
#include "ISR.h"

using namespace lcio ;
using namespace marlin ;

class HiggsProcessor : public Processor
{
	public :
		double zMassRef = 91.2 ;
		double wMassRef = 80.43 ;

		virtual Processor* newProcessor() { return new HiggsProcessor ; }

		HiggsProcessor() ;

		virtual void init() ;

		virtual void processEvent( LCEvent * evt ) ;

		virtual void check(LCEvent* ) ;

		virtual void end() ;


		std::vector<int> findDecayMode(LCCollection* _mcCol) ;

		ISR processISR() ;

		CLHEP::HepLorentzVector getZLorentzVectorMC(LCCollection* _mcCol) ;
		CLHEP::HepLorentzVector getHLorentzVectorMC(LCCollection* _mcCol) ;

		double getZEnergyMC() ;
		double getCosAngleBetweenZJetsMC() ;


		void computeOriginMap() ;

		std::map<int,float> mcOriginOfParticle(ReconstructedParticleImpl* recoPart) ;

		std::map<int,float> mcOriginOfJet(const fastjet::PseudoJet& jet, fastjet::ClusterSequence& cs) ;

		template<typename T>
		std::vector<fastjet::PseudoJet> exclusiveJets(T value) ;


//		std::vector<fastjet::PseudoJet> exclusiveJets(double dcut) ;
//		std::vector<fastjet::PseudoJet> exclusiveJets(int nJets) ;

		DiJet chooseZDiJet(const std::vector<fastjet::PseudoJet>& jets , std::vector<fastjet::PseudoJet>& remainingJets) ;

		std::pair<DiJet,DiJet> choosePairOfZDiJets(const std::vector<fastjet::PseudoJet>& jets) ;
		std::pair<DiJet,DiJet> choosePairOfWDiJets(const std::vector<fastjet::PseudoJet>& jets) ;

		HiggsProcessor(const HiggsProcessor &toCopy) = delete ;
		void operator=(const HiggsProcessor &toCopy) = delete ;

	protected :
		std::pair<DiJet,DiJet> choosePairDiJets(const std::vector<fastjet::PseudoJet>& jets , const double& targetMass) ;

	protected :

		int _nRun = 0 ;

		LCEvent* currentEvt = nullptr ;

		std::string mcPartColName = "" ;
		std::string recoPartColName = "" ;
		std::string linkColName = "" ;

		LCCollection* mcCol = nullptr ;
		LCCollection* linkCol = nullptr ;
		LCCollection* recoCol = nullptr ;

		std::vector<ReconstructedParticleImpl*> recoParticles = {} ;

		std::vector<fastjet::PseudoJet> particles = {} ;
		std::vector<fastjet::PseudoJet> zParticles = {} ;
		std::vector<fastjet::PseudoJet> hParticles = {} ;
		std::vector<fastjet::PseudoJet> allExceptHParticles = {} ;

		//		std::vector<fastjet::PseudoJet> jets ;

		//		ParticleVec particles ;


		std::map<ReconstructedParticleImpl* , std::map<int,float> > originMap ;

		std::string rootFileName = "" ;
		TFile* file = nullptr ;
		TTree* tree = nullptr ;

		double sqrtS = 0 ;

		int eventNumber = 0 ;

		int decayID = 0 ;
		double totalEnergy = 0 ;
		double energy4Jets = 0 ;
		double ISREnergy = 0 ;

		std::vector<double> energyISRVec {} ;
		std::vector<double> etaISRVec {} ;

		double zMassMC = 0 ;
		double recMassMC = 0 ;

		double zEnergyMC = 0 ;
		double cosAngleBetweenZJetsMC = 0 ;
		double cosAngleBetweenHJetsMC = 0 ;
		double zPhiMC = 0 ;
		double zRapMC = 0 ;

		double hPhiMC = 0 ;
		double hRapMC = 0 ;

		std::vector<double> phiVec {} ;
		std::vector<double> thetaVec {} ;
		std::vector<double> jetVec {} ;

		double zMass = 0 ;
		double recMass = 0 ;
		double recMass2 = 0 ;
		double zEnergy = 0 ;
		double cosAngleBetweenZJets = 0 ;
		double cosAngleBetweenHJets = 0 ;

		double maxCosBetweenTwoJets = 0 ;

		double zPhi = 0 ;
		double zRap = 0 ;

		double hPhi = 0 ;
		double hRap = 0 ;


		double hDiJetMass = 0 ;

		double ww12mass = 0 ;
		double ww34mass = 0 ;

		double zz12mass = 0 ;
		double zz34mass = 0 ;

		double zPurity = 0 ;
		double hCont = 0 ;

		std::vector<double> zPurityJets {} ;

		double idealRecoZmass = 0 ;
		double idealRecoHmass = 0 ;
		double idealRecoRecMass = 0 ;


		std::vector<double> yValueVec ;

		double invRecMass = 0 ;

		double inclRecMass = 0 ;

	private :


} ;


#endif //HiggsProcessor_h
