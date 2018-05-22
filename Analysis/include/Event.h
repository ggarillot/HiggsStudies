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
		Event() = default ;
		~Event() = default ;

		int processID = 0 ;

		bool goodEvent = true ;

		double sqrtS = 0 ;

		unsigned int nJets = 0 ;
		unsigned int nIsoLep = 0 ;

		double y23 = 0 ;
		double y34 = 0 ;

		double zMass = 0 ;
		double recMass = 0 ;

		double cosThetaZ = 0 ;
		double cosThetaZ12 = 0 ;

		double z1e = 0 ;
		double z2e = 0 ;

		double mass2Jet = 0 ;

		double ww12mass = 0 ;
		double ww34mass = 0 ;
		double zz12mass = 0 ;
		double zz34mass = 0 ;

		double wwMass3 = 0 ;
		double wwRecMass3 = 0 ;

		double cosThetaMiss = 0 ;

		double totalEnergy = 0 ;

		//MC infos
		int decayID = 0 ;
		int subDecayID = 0 ;
} ;

class EventReader
{
	public :
		EventReader() = default ;
		~EventReader() = default ;

		Event getEvent(Long64_t entry) ;
		void loadEvent(Event event) ;

		void setTree(TTree* _tree) ;

		void setTreeWrite(TTree* _tree) ;


		EventReader(const EventReader &toCopy) = delete ;
		void operator=(const EventReader &toCopy) = delete ;


	protected :
		TTree* tree = nullptr ;

	protected :

		bool goodEvent = true ;

		double sqrtS = 0 ;

		unsigned int nJets = 0 ;
		unsigned int nIsoLep = 0 ;

		double y23 = 0 ;
		double y34 = 0 ;

		double zMass = 0 ;
		double recMass = 0 ;

		double cosThetaZ = 0 ;
		double cosThetaZ12 = 0 ;

		double z1e = 0 ;
		double z2e = 0 ;

		double mass2Jet = 0 ;

		double ww12mass = 0 ;
		double ww34mass = 0 ;
		double zz12mass = 0 ;
		double zz34mass = 0 ;

		double wwMass3 = 0 ;
		double wwRecMass3 = 0 ;

		double cosThetaMiss = 0 ;

		double totalEnergy = 0 ;

		//MC infos
		int decayID = 0 ;
		int subDecayID = 0 ;

	public :
		int processID = 0 ;
		bool preRejected = false ;
		float BDT = 0 ;
		float weight = 0 ;
		float deltaE = 0 ;
} ;


#endif //Event_h
