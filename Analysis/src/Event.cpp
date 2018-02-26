#include "Event.h"


using namespace std ;

Event::Event()
{
}

EventReader::EventReader()
{
}

EventReader::~EventReader()
{
}


Event EventReader::getEvent(Long64_t entry)
{
	Event event ;
	int isOk = tree->GetEntry(entry) ;

	if ( isOk == 0 )
		return event ;

	event.sqrtS = sqrtS ;

	event.nJets = nJets ;

	event.zMass = zMass ;
	event.recMass = recMass ;

	event.cosThetaZDiJet = cosThetaZDiJet ;
	event.cosThetaHDiJet = cosThetaHDiJet ;

	event.z1e = z1e ;
	event.z2e = z2e ;
	event.h1e = h1e ;
	event.h2e = h2e ;

	event.ww12mass = ww12mass ;
	event.ww34mass = ww34mass ;
	event.zz12mass = zz12mass ;
	event.zz34mass = zz34mass ;

	event.totalEnergy = totalEnergy ;
	event.totalEnergyJets = totalEnergyJets ;


	//MC infos
	event.decayID = decayID ;

	event.zPurity = zPurity ;
	event.zTagged = zTagged ;

	event.zMassIdeal = zMassIdeal ;
	event.recMassIdeal =recMassIdeal ;

	event.cosThetaZDiJetIdeal = cosThetaZDiJetIdeal ;
	event.cosThetaHDiJetIdeal = cosThetaHDiJetIdeal ;

	event.ISREnergy = ISREnergy ;

	return event ;
}

void EventReader::setTree(TTree* _tree)
{
	tree = _tree ;

	tree->SetBranchAddress("sqrtS" , &sqrtS) ;

	tree->SetBranchAddress("nJets" , &nJets) ;

	tree->SetBranchAddress("zMass" , &zMass) ;
	tree->SetBranchAddress("recMass" , &recMass) ;

	tree->SetBranchAddress("cosThetaZDiJet" , &cosThetaZDiJet) ;
	tree->SetBranchAddress("cosThetaHDiJet" , &cosThetaHDiJet) ;

	tree->SetBranchAddress("z1e" , &z1e) ;
	tree->SetBranchAddress("z2e" , &z2e) ;
	tree->SetBranchAddress("h1e" , &h1e) ;
	tree->SetBranchAddress("h2e" , &h2e) ;

	tree->SetBranchAddress("ww12mass" , &ww12mass) ;
	tree->SetBranchAddress("ww34mass" , &ww34mass) ;
	tree->SetBranchAddress("zz12mass" , &zz12mass) ;
	tree->SetBranchAddress("zz34mass" , &zz34mass) ;

	tree->SetBranchAddress("totalEnergy" , &totalEnergy) ;
	tree->SetBranchAddress("totalEnergyJets" , &totalEnergyJets) ;

	//MC infos
	tree->SetBranchAddress("decayID" , &decayID) ;

	tree->SetBranchAddress("zPurity" , &zPurity) ;
	tree->SetBranchAddress("zTagged" , &zTagged) ;

	tree->SetBranchAddress("zMassIdeal" , &zMassIdeal) ;
	tree->SetBranchAddress("recMassIdeal" , &recMassIdeal) ;

	tree->SetBranchAddress("cosThetaZDiJetIdeal" , &cosThetaZDiJetIdeal) ;
	tree->SetBranchAddress("cosThetaHDiJetIdeal" , &cosThetaHDiJetIdeal) ;

	tree->SetBranchAddress("ISREnergy" , &ISREnergy) ;
}

void EventReader::setTreeWrite(TTree* _tree)
{
	_tree->Branch("sqrtS" , &sqrtS) ;

	_tree->Branch("nJets" , &nJets) ;

	_tree->Branch("zMass" , &zMass) ;
	_tree->Branch("recMass" , &recMass) ;

	_tree->Branch("cosThetaZDiJet" , &cosThetaZDiJet) ;
	_tree->Branch("cosThetaHDiJet" , &cosThetaHDiJet) ;

	_tree->Branch("z1e" , &z1e) ;
	_tree->Branch("z2e" , &z2e) ;
	_tree->Branch("h1e" , &h1e) ;
	_tree->Branch("h2e" , &h2e) ;

	_tree->Branch("ww12mass" , &ww12mass) ;
	_tree->Branch("ww34mass" , &ww34mass) ;
	_tree->Branch("zz12mass" , &zz12mass) ;
	_tree->Branch("zz34mass" , &zz34mass) ;

	_tree->Branch("totalEnergy" , &totalEnergy) ;
	_tree->Branch("totalEnergyJets" , &totalEnergyJets) ;

	//MC infos
	_tree->Branch("decayID" , &decayID) ;

	_tree->Branch("zPurity" , &zPurity) ;
	_tree->Branch("zTagged" , &zTagged) ;

	_tree->Branch("zMassIdeal" , &zMassIdeal) ;
	_tree->Branch("recMassIdeal" , &recMassIdeal) ;

	_tree->Branch("cosThetaZDiJetIdeal" , &cosThetaZDiJetIdeal) ;
	_tree->Branch("cosThetaHDiJetIdeal" , &cosThetaHDiJetIdeal) ;

	_tree->Branch("ISREnergy" , &ISREnergy) ;
}
