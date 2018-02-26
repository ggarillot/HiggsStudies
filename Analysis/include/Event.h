#ifndef Event_h
#define Event_h

#include <vector>
#include <map>
#include <array>

#include <TROOT.h>
#include <TTree.h>

#include <iostream>


struct Event
{
		Event() ;

		double sqrtS = 0 ;

		unsigned int nJets = 0 ;

		double zMass = 0 ;
		double recMass = 0 ;

		double cosThetaZDiJet = 0 ;
		double cosThetaHDiJet = 0 ;

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


		//MC infos
		int decayID = 0 ;

		double zPurity = 0 ;
		double zTagged = 0 ;

		double zMassIdeal = 0 ;
		double recMassIdeal = 0 ;

		double cosThetaZDiJetIdeal = 0 ;
		double cosThetaHDiJetIdeal = 0 ;

		double ISREnergy = 0 ;
} ;

class EventReader
{
	public :
		EventReader() ;
		~EventReader() ;

		Event getEvent(Long64_t entry) ;

		void setTree(TTree* _tree) ;

		void setTreeWrite(TTree* _tree) ;


		EventReader(const EventReader &toCopy) = delete ;
		void operator=(const EventReader &toCopy) = delete ;


	protected :
		TTree* tree = nullptr ;

	protected :
		double sqrtS = 0 ;

		unsigned int nJets = 0 ;

		double zMass = 0 ;
		double recMass = 0 ;

		double cosThetaZDiJet = 0 ;
		double cosThetaHDiJet = 0 ;

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


		//MC infos
		int decayID = 0 ;

		double zPurity = 0 ;
		double zTagged = 0 ;

		double zMassIdeal = 0 ;
		double recMassIdeal = 0 ;

		double cosThetaZDiJetIdeal = 0 ;
		double cosThetaHDiJetIdeal = 0 ;

		double ISREnergy = 0 ;
} ;


#endif //Event_h
