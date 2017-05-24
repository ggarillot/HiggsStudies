#include "HiggsProcessor.h"



#include <iostream>

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <EVENT/LCParameters.h>
#include <UTIL/CellIDDecoder.h>

#include <IMPL/ReconstructedParticleImpl.h>
#include <IMPL/MCParticleImpl.h>

#include <IMPL/LCRelationImpl.h>
#include <UTIL/LCRelationNavigator.h>


#include "marlin/VerbosityLevels.h"
#include <string.h>


using namespace lcio ;
using namespace marlin ;

HiggsProcessor aHiggsProcessor ;

HiggsProcessor::HiggsProcessor() : Processor("HiggsProcessor")
{

	registerProcessorParameter( "RootFileName" ,
								"File name for the root output",
								rootFileName ,
								std::string("test.root") ) ;

	registerProcessorParameter( "ReconstructedParticlesCollectionName" ,
								"Name of the reconstructed particles collection" ,
								recoPartColName ,
								std::string("PandoraPFOs") ) ;

	registerProcessorParameter( "ReconstructedParticlesCollectionName" ,
								"Name of the reconstructed particles collection" ,
								mcPartColName ,
								std::string("MCParticlesSkimmed") ) ;

	registerProcessorParameter( "ReconstructedParticlesCollectionName" ,
								"Name of the reconstructed particles collection" ,
								linkColName ,
								std::string("RecoMCTruthLink") ) ;

}

void HiggsProcessor::init()
{
	std::cout << "HiggsProcessor::init()" << std::endl ;
	file = new TFile(rootFileName.c_str() , "RECREATE") ;
	tree = new TTree("tree" , "tree") ;

	tree->Branch("decayID" , &decayID) ;
	tree->Branch("energy4Jets" , &energy4Jets) ;
	tree->Branch("ISREnergy" , &ISREnergy) ;
	tree->Branch("yValueVec" , &yValueVec) ;
	tree->Branch("zMass" , &zMass) ;
	tree->Branch("recMass" , &recMass) ;
	tree->Branch("recMass2" , &recMass2) ;

	tree->Branch("ww12mass" , &ww12mass) ;
	tree->Branch("ww34mass" , &ww34mass) ;
	tree->Branch("zz12mass" , &zz12mass) ;
	tree->Branch("zz34mass" , &zz34mass) ;

	tree->Branch("zMassMC" , &zMassMC) ;
	tree->Branch("recMassMC" , &recMassMC) ;
	tree->Branch("deltaP" , &deltaP) ;
	tree->Branch("pX" , &pX) ;
	tree->Branch("pY" , &pY) ;
	tree->Branch("pZ" , &pZ) ;

	_nEvt = 0 ;
}


std::vector<int> HiggsProcessor::findDecayMode(LCCollection* mcCol)
{
//	std::cout << "HiggsProcessor::findDecayMode(LCCollection* mcCol)" << std::endl ;

	std::vector<int> toReturn ;
	if ( mcCol->getNumberOfElements() < 10 )
		return toReturn ;

	MCParticleImpl* part = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 9 ) ) ;

	if ( part->getPDG() != 25 )
	{
		std::cout << "Error in HiggsProcessor::findDecayMode : Not a Higgs : " << part->getPDG() << std::endl ;
//		throw ;
	}

	MCParticleVec vec = part->getDaughters() ;


	for ( unsigned int i = 0 ; i < vec.size() ; ++i )
		toReturn.push_back( vec.at(i)->getPDG() ) ;

	return toReturn ;
}

double HiggsProcessor::getISREnergy(LCCollection* mcCol)
{
	double toReturn = 0.0 ;
	if ( mcCol->getNumberOfElements() < 2 )
		return toReturn ;

	toReturn += dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 0 ) )->getEnergy() ;
	toReturn += dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 1 ) )->getEnergy() ;
	return toReturn ;
}

CLHEP::HepLorentzVector HiggsProcessor::getZLorentzVectorMC(LCCollection* mcCol)
{
	if ( mcCol->getNumberOfElements() < 13 )
	{
		return CLHEP::HepLorentzVector(0,0,0,0) ;
	}

	const double* p1d = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 2 ) )->getMomentum() ;
	const double* p2d = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 3 ) )->getMomentum() ;

	CLHEP::Hep3Vector p1( p1d[0] , p1d[1] , p1d[2] ) ;
	CLHEP::Hep3Vector p2( p2d[0] , p2d[1] , p2d[2] ) ;
	CLHEP::Hep3Vector p = p1+p2 ;

	double e = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 2 ) )->getEnergy() + dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 3 ) )->getEnergy() ;


	//	const double* p1d = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 10 ) )->getMomentum() ;

	//	CLHEP::Hep3Vector p( p1d[0] , p1d[1] , p1d[2] ) ;


	//	double e = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 10 ) )->getEnergy()  ;

	//	int id = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 10 ) )->getPDG() ;
	//	std::cout << id << std::endl ;
	//	if ( id !=94 )
	//		throw ;


	return CLHEP::HepLorentzVector(p , e) ;
}

CLHEP::HepLorentzVector HiggsProcessor::getHLorentzVectorMC(LCCollection* mcCol)
{
	if ( mcCol->getNumberOfElements() < 5 )
	{
		return CLHEP::HepLorentzVector(0,0,0,0) ;
	}

	const double* p1d = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 4 ) )->getMomentum() ;

	CLHEP::Hep3Vector p( p1d[0] , p1d[1] , p1d[2] ) ;

	double e = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 4 ) )->getEnergy()  ;

	int id = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 4 ) )->getPDG() ;
	if ( id != 25 )
	{
		std::cout << "Error in HiggsProcessor::findDecayMode : Not a Higgs : " << id << std::endl ;
//		throw ;
	}

	return CLHEP::HepLorentzVector(p , e) ;
}


void HiggsProcessor::getLinkTest(LCCollection* linkCol)
{
	UTIL::LCRelationNavigator relNav(linkCol) ;

//	std::cout << "from type : " << relNav.getFromType() << std::endl ;
//	std::cout << "  to type : " << relNav.getToType() << std::endl ;

//	std::cout << linkCol->getNumberOfElements() << std::endl ;

	LCRelationImpl* link = dynamic_cast<LCRelationImpl*>( linkCol->getElementAt(0) ) ;
	ReconstructedParticleImpl* part = dynamic_cast<ReconstructedParticleImpl*>( link->getFrom() ) ;

	std::cout << relNav.getRelatedFromObjects(part).size() << std::endl ;
//	std::cout << dynamic_cast<MCParticleImpl*>( relNav.getRelatedFromObjects(part).at(0) )->getPDG() << std::endl ;
}


std::map<int,float> HiggsProcessor::mcOriginOfJet(fastjet::PseudoJet& jet)
{
	std::map<int,float> toReturn ;
//	std::vector<fastjet::PseudoJet> particlesOfJet = jet.constituents() ;

	LCCollection* linkCol = currentEvt->getCollection( linkColName ) ;

	LCRelationImpl* link = dynamic_cast<LCRelationImpl*>( linkCol->getElementAt(0) ) ;
	ReconstructedParticleImpl* recoPart = dynamic_cast<ReconstructedParticleImpl*>( link->getFrom() ) ;
	MCParticleImpl* mcPart = dynamic_cast<MCParticleImpl*>( link->getTo() ) ;


	std::cout << recoPart->getEnergy() << std::endl ;



	return toReturn ;

}


void HiggsProcessor::processRunHeader(LCRunHeader* run)
{
	_nRun++ ;
	_nEvt = 0 ;
}


void HiggsProcessor::processEvent(LCEvent* evt)
{
	currentEvt = evt ;



//	getLinkTest( evt->getCollection("RecoMCTruthLink") ) ;
//	std::cout << "HiggsProcessor::processEvent(LCEvent * evt)" << std::endl ;
	using fastjet::PseudoJet ;
	std::vector<int> vec = findDecayMode( evt->getCollection(mcPartColName) ) ;

	ISREnergy = getISREnergy( evt->getCollection(mcPartColName) ) ;

	CLHEP::HepLorentzVector zVec = getZLorentzVectorMC( evt->getCollection(mcPartColName) ) ;
	CLHEP::HepLorentzVector hVec = getHLorentzVectorMC( evt->getCollection(mcPartColName) ) ;

	CLHEP::Hep3Vector zP = zVec.getV() ;
	CLHEP::Hep3Vector zH = hVec.getV() ;

	double sqrtS = 250 ;
	double pt = 2*std::sqrt(sqrtS*sqrtS/4.0 - 511e-6*511e-6)*std::sin(7e-3) ;

	CLHEP::Hep3Vector totP(pt,0,0) ;

	zMassMC = zVec.m() ;

	recMassMC = std::sqrt( (250 - zVec.e())*(250 - zVec.e()) - ( zP.mag2() + totP.mag2() -2*zP.dot(totP) ) ) ;


	deltaP = hVec.perp2() + hVec.py()*hVec.py() - zVec.perp2() - zVec.py()*zVec.py() ;

	pX = hVec.px() + zVec.px() ;
	pY = hVec.py() + zVec.py() ;
	pZ = hVec.pz() + zVec.pz() ;

	LCCollection* recoCol = evt->getCollection("PandoraPFOs") ;

	particles.clear() ;
	for ( int i = 0 ; i < recoCol->getNumberOfElements() ; ++i )
	{
		ReconstructedParticleImpl* part = dynamic_cast<ReconstructedParticleImpl*>( recoCol->getElementAt(i) ) ;
		particles.push_back( PseudoJet( part->getMomentum()[0] , part->getMomentum()[1] , part->getMomentum()[2] , part->getEnergy() ) ) ;
	}


	double R = 3.14159265/2.0;
	fastjet::JetDefinition jet_def(fastjet::kt_algorithm , R) ;
	fastjet::ClusterSequence cs(particles, jet_def) ;

	std::vector<PseudoJet> jets = sorted_by_pt( cs.exclusive_jets(4) ) ;

	mcOriginOfJet(jets.at(0) ) ;

	energy4Jets = 0.0 ;
	for ( std::vector<PseudoJet>::const_iterator it = jets.begin() ; it != jets.end() ; ++it )
		energy4Jets += it->e() ;

	yValueVec.clear() ;
	yValueVec.push_back(0) ;

	for ( int i = 1 ; i < 6 ; ++i )
		yValueVec.push_back( -std::log10( cs.exclusive_ymerge(i) ) ) ;


	if  ( jets.size() != 4 )
	{
		std::cout << "Error : not 4 jets" << std::endl ;
		throw ;
	}

	std::vector< std::pair<PseudoJet , PseudoJet> > pairs ;
	pairs.push_back( std::make_pair( jets.at(0)+jets.at(1) , jets.at(2)+jets.at(3) ) ) ;
	pairs.push_back( std::make_pair( jets.at(0)+jets.at(2) , jets.at(1)+jets.at(3) ) ) ;
	pairs.push_back( std::make_pair( jets.at(0)+jets.at(3) , jets.at(1)+jets.at(2) ) ) ;
	pairs.push_back( std::make_pair( jets.at(2)+jets.at(3) , jets.at(0)+jets.at(1) ) ) ;
	pairs.push_back( std::make_pair( jets.at(1)+jets.at(3) , jets.at(0)+jets.at(2) ) ) ;
	pairs.push_back( std::make_pair( jets.at(1)+jets.at(2) , jets.at(0)+jets.at(3) ) ) ;


	//WW ======
	double chi2 = std::numeric_limits<double>::max() ;
	std::vector< std::pair<PseudoJet,PseudoJet> >::iterator goodPairIt = pairs.end() ;
	for ( std::vector< std::pair<PseudoJet,PseudoJet> >::iterator it = pairs.begin() ; it != pairs.end() ; ++it )
	{
		double tempchi2 = ( it->first.m()*1.04 - 80.39 )*( it->first.m()*1.04 - 80.39 ) + ( it->second.m()*1.04 - 80.39 )*( it->second.m()*1.04 - 80.39 ) ;

		if (tempchi2 < chi2)
		{
			chi2 = tempchi2 ;
			goodPairIt = it ;
		}
	}
	ww12mass = goodPairIt->first.m() ;
	ww34mass = goodPairIt->second.m() ;

	//ZZ ======
	chi2 = std::numeric_limits<double>::max() ;
	goodPairIt = pairs.end() ;
	for ( std::vector< std::pair<PseudoJet,PseudoJet> >::iterator it = pairs.begin() ; it != pairs.end() ; ++it )
	{
		double tempchi2 = ( it->first.m()*1.04 - 91.2 )*( it->first.m()*1.04 - 91.2 ) + ( it->second.m()*1.04 - 91.2 )*( it->second.m()*1.04 - 91.2 ) ;

		if (tempchi2 < chi2)
		{
			chi2 = tempchi2 ;
			goodPairIt = it ;
		}
	}
	zz12mass = goodPairIt->first.m() ;
	zz34mass = goodPairIt->second.m() ;




	//Signal
	chi2 = std::numeric_limits<double>::max() ;
	goodPairIt = pairs.end() ;


	for ( std::vector< std::pair<PseudoJet,PseudoJet> >::iterator it = pairs.begin() ; it != pairs.end() ; ++it )
	{
//		double tempchi2 = std::abs( it->first.m() - 91.2 ) ;
		double tempchi2 = ( it->first.m()*1.04 - 91.2 )*( it->first.m()*1.04 - 91.2 ) ;

		if (tempchi2 < chi2)
		{
			chi2 = tempchi2 ;
			goodPairIt = it ;
		}
	}


	const PseudoJet& qqDiJet = goodPairIt->first ;

	double pqq = qqDiJet.pt()*qqDiJet.pt() + qqDiJet.pz()*qqDiJet.pz() ;

	CLHEP::Hep3Vector pZrec( qqDiJet.px() , qqDiJet.py() , qqDiJet.pz() ) ;


	double mrec = (250.0 - qqDiJet.e() )*(250.0 - qqDiJet.e() ) - pqq ;

//	recMass2 = std::sqrt( (250 - qqDiJet.e())*(250 - qqDiJet.e()) - ( pZrec.mag2() + totP.mag2() -2*pZrec.dot(totP) ) ) ;

	pZrec *= 1.04 ;
	recMass2 = std::sqrt( (250 - qqDiJet.e()*1.04)*(250 - qqDiJet.e()*1.04) - ( pZrec.mag2() + totP.mag2() -2*pZrec.dot(totP) ) ) ;

	zMass = qqDiJet.m()*1.04 ;
//	zMass = qqDiJet.m() ;


	recMass = std::sqrt(mrec) ;

	//	for ( unsigned int i = 0 ; i < vec.size() ; ++i )
	//		std::cout << vec.at(i) << " " ;

	//	std::cout << std::endl ;

	decayID = vec.at(0) ;
	tree->Fill() ;


	std::cout << "Event " << evt->getEventNumber() << std::endl ;

	_nEvt++ ;

}


void HiggsProcessor::check(LCEvent * evt)
{
}


void HiggsProcessor::end()
{
	tree->Write() ;
	file->Close() ;
}
