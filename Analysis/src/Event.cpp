#include "Event.h"


using namespace std ;

Event EventReader::getEvent(Long64_t entry)
{
	Event event ;
	int isOk = tree->GetEntry(entry) ;

	if ( isOk == 0 )
		return event ;

	event.processID = processID ;

	event.goodEvent = goodEvent ;
	event.sqrtS = sqrtS ;

	event.nJets = nJets ;
	event.nIsoLep = nIsoLep ;

	event.y23 = y23 ;
	event.y34 = y34 ;

	event.zMass = zMass ;
	event.recMass = recMass ;

	event.cosThetaZ = cosThetaZ ;
	event.cosThetaZ12 = cosThetaZ12 ;

	event.z1e = z1e ;
	event.z2e = z2e ;

	event.mass2Jet = mass2Jet ;

	event.ww12mass = ww12mass ;
	event.ww34mass = ww34mass ;
	event.zz12mass = zz12mass ;
	event.zz34mass = zz34mass ;

	event.wwMass3 = wwMass3 ;
	event.wwRecMass3 = wwRecMass3 ;

	event.totalEnergy = totalEnergy ;

	//MC infos
	event.decayID = decayID ;
	event.subDecayID = subDecayID ;

	return event ;
}

void EventReader::loadEvent(Event event)
{
	processID = event.processID ;

	goodEvent = event.goodEvent ;

	sqrtS = event.sqrtS ;

	nJets = event.nJets ;
	nIsoLep = event.nIsoLep ;

	y23 = event.y23 ;
	y34 = event.y34 ;

	zMass = event.zMass ;
	recMass = event.recMass ;

	cosThetaZ = event.cosThetaZ ;
	cosThetaZ12 = event.cosThetaZ12 ;

	z1e = event.z1e ;
	z2e = event.z2e ;

	mass2Jet = event.mass2Jet ;

	ww12mass = event.ww12mass ;
	ww34mass = event.ww34mass ;
	zz12mass = event.zz12mass ;
	zz34mass = event.zz34mass ;

	wwMass3 = event.wwMass3 ;
	wwRecMass3 = event.wwRecMass3 ;

	totalEnergy = event.totalEnergy ;

	//MC infos
	decayID = event.decayID ;
	subDecayID = event.subDecayID ;
}

void EventReader::setTree(TTree* _tree)
{
	tree = _tree ;

	tree->SetBranchAddress("goodEvent" , &goodEvent) ;

	tree->SetBranchAddress("sqrtS" , &sqrtS) ;

	tree->SetBranchAddress("nJets" , &nJets) ;
	tree->SetBranchAddress("nIsoLep" , &nIsoLep) ;

	tree->SetBranchAddress("y23" , &y23) ;
	tree->SetBranchAddress("y34" , &y34) ;

	tree->SetBranchAddress("zMass" , &zMass) ;
	tree->SetBranchAddress("recMass" , &recMass) ;

	tree->SetBranchAddress("cosThetaZ" , &cosThetaZ) ;
	tree->SetBranchAddress("cosThetaZ12" , &cosThetaZ12) ;

	tree->SetBranchAddress("z1e" , &z1e) ;
	tree->SetBranchAddress("z2e" , &z2e) ;

	tree->SetBranchAddress("mass2Jet" , &mass2Jet) ;

	tree->SetBranchAddress("ww12mass" , &ww12mass) ;
	tree->SetBranchAddress("ww34mass" , &ww34mass) ;
	tree->SetBranchAddress("zz12mass" , &zz12mass) ;
	tree->SetBranchAddress("zz34mass" , &zz34mass) ;

	tree->SetBranchAddress("wwMass3" , &wwMass3) ;
	tree->SetBranchAddress("wwRecMass3" , &wwRecMass3) ;

	tree->SetBranchAddress("totalEnergy" , &totalEnergy) ;

	//MC infos
	tree->SetBranchAddress("decayID" , &decayID) ;
	tree->SetBranchAddress("subDecayID" , &subDecayID) ;
}

void EventReader::setTreeWrite(TTree* _tree)
{
	_tree->Branch("processID" , &processID) ;

	_tree->Branch("goodEvent" , &goodEvent) ;

	_tree->Branch("sqrtS" , &sqrtS) ;

	_tree->Branch("nJets" , &nJets) ;
	_tree->Branch("nIsoLep" , &nIsoLep) ;

	_tree->Branch("y23" , &y23) ;
	_tree->Branch("y34" , &y34) ;

	_tree->Branch("zMass" , &zMass) ;
	_tree->Branch("recMass" , &recMass) ;

	_tree->Branch("cosThetaZ" , &cosThetaZ) ;
	_tree->Branch("cosThetaZ12" , &cosThetaZ12) ;

	_tree->Branch("z1e" , &z1e) ;
	_tree->Branch("z2e" , &z2e) ;

	_tree->Branch("mass2Jet" , &mass2Jet) ;

	_tree->Branch("ww12mass" , &ww12mass) ;
	_tree->Branch("ww34mass" , &ww34mass) ;
	_tree->Branch("zz12mass" , &zz12mass) ;
	_tree->Branch("zz34mass" , &zz34mass) ;

	_tree->Branch("wwMass3" , &wwMass3) ;
	_tree->Branch("wwRecMass3" , &wwRecMass3) ;

	_tree->Branch("totalEnergy" , &totalEnergy) ;

	//MC infos
	_tree->Branch("decayID" , &decayID) ;
	_tree->Branch("subDecayID" , &subDecayID) ;

	_tree->Branch("preRejected" , &preRejected) ;
	_tree->Branch("BDT" , &BDT) ;
	_tree->Branch("weight" , &weight) ;
	_tree->Branch("deltaE" , &deltaE) ;
}
