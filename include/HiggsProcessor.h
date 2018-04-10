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
#include <fastjet/JadePlugin.hh>

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
		double totalNeutrinoEnergy() ;

		std::map<int,float> mcOriginOfParticle(ReconstructedParticleImpl* recoPart) ;
		std::map<int,float> mcOriginOfJet(const fastjet::PseudoJet& jet, fastjet::ClusterSequence& cs) ;

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

		double dMin = 0.0 ;
		double alphaAngle = 0.0 ;
		double valueAngle = 0.0 ;

		std::vector<ReconstructedParticleImpl*> recoParticles = {} ;

		std::vector<fastjet::PseudoJet> particles = {} ;
		std::vector<fastjet::PseudoJet> zParticles = {} ;
		std::vector<fastjet::PseudoJet> hParticles = {} ;
		std::vector<fastjet::PseudoJet> allExceptHParticles = {} ;

		std::map<ReconstructedParticleImpl* , std::map<int,float> > originMap ;

		std::string rootFileName = "" ;
		TFile* file = nullptr ;
		TTree* tree = nullptr ;

		double sqrtS = 0 ;

		unsigned int nJets = 0 ;

		double zMass = 0 ;
		double recMass = 0 ;
		double recMass2 = 0 ;
		double recMassInv = 0 ;

		double cosThetaZ = 0 ;

		double cosThetaZ12 = 0 ;
		double cosThetaH12 = 0 ;

		double z1e = 0 ;
		double z2e = 0 ;
		double h1e = 0 ;
		double h2e = 0 ;

		double ww12mass = 0 ;
		double ww34mass = 0 ;
		double zz12mass = 0 ;
		double zz34mass = 0 ;

		double totalEnergy = 0 ;
		double totalEnergyJets = 0 ;

		std::vector<double> pMiss = {} ;
		double pMissNorm = 0 ;


		//MC infos
		int decayID = 0 ;

		double zPurity = 0 ;
		double zTagged = 0 ;

		double zMassIdeal = 0 ;
		double recMassIdeal = 0 ;

		double cosThetaZIdeal = 0 ;

		double cosThetaBetwZDiJetIdeal = 0 ;
		double cosThetaBetwHDiJetIdeal = 0 ;

		double neutrinoEnergy = 0 ;
		double ISREnergy = 0 ;





		double zMassMC = 0 ;
		double recMassMC = 0 ;





		std::vector<double> zPurityJets {} ;


		std::vector<double> yValueVec ;

	private :


} ;


#endif //HiggsProcessor_h
