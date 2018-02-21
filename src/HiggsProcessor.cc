#include "HiggsProcessor.h"

#include <iostream>
#include <cassert>
#include <cmath>

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

HiggsProcessor::HiggsProcessor()
	: Processor("HiggsProcessor") ,
	  originMap() ,
	  yValueVec()
{

	registerProcessorParameter( "RootFileName" ,
								"File name for the root output",
								rootFileName ,
								std::string("test.root") ) ;

	registerProcessorParameter( "ReconstructedParticlesCollectionName" ,
								"Name of the reconstructed particles collection" ,
								recoPartColName ,
								std::string("PandoraPFOs") ) ;

	registerProcessorParameter( "MCParticlesCollectionName" ,
								"Name of the mc particles collection" ,
								mcPartColName ,
								std::string("MCParticlesSkimmed") ) ;

	registerProcessorParameter( "LinkCollectionName" ,
								"Name of the mc truth link collection" ,
								linkColName ,
								std::string("RecoMCTruthLink") ) ;

	registerProcessorParameter( "CenterOfMassEnergy" ,
								"CenterOfMassEnergy" ,
								sqrtS ,
								250.0 ) ;

	registerProcessorParameter( "dMin" ,
								"dMin" ,
								dMin ,
								375.0 ) ;
}

void HiggsProcessor::init()
{
	std::cout << "HiggsProcessor::init()" << std::endl ;
	file = new TFile(rootFileName.c_str() , "RECREATE") ;
	tree = new TTree("tree" , "tree") ;


	tree->Branch("sqrtS" , &sqrtS) ;

	tree->Branch("nJets" , &nJets) ;

	tree->Branch("zMass" , &zMass) ;
	tree->Branch("recMass" , &recMass) ;

	tree->Branch("cosThetaZDiJet" , &cosThetaZDiJet) ;
	tree->Branch("cosThetaHDiJet" , &cosThetaHDiJet) ;

	tree->Branch("z1e" , &z1e) ;
	tree->Branch("z2e" , &z2e) ;
	tree->Branch("h1e" , &h1e) ;
	tree->Branch("h2e" , &h2e) ;

	tree->Branch("ww12mass" , &ww12mass) ;
	tree->Branch("ww34mass" , &ww34mass) ;
	tree->Branch("zz12mass" , &zz12mass) ;
	tree->Branch("zz34mass" , &zz34mass) ;

	tree->Branch("totalEnergy" , &totalEnergy) ;
	tree->Branch("totalEnergyJets" , &totalEnergyJets) ;

	//MC infos
	tree->Branch("decayID" , &decayID) ;

	tree->Branch("zPurity" , &zPurity) ;
	tree->Branch("zTagged" , &zTagged) ;

	tree->Branch("zMassIdeal" , &zMassIdeal) ;
	tree->Branch("recMassIdeal" , &recMassIdeal) ;

	tree->Branch("cosThetaZDiJetIdeal" , &cosThetaZDiJetIdeal) ;
	tree->Branch("cosThetaHDiJetIdeal" , &cosThetaHDiJetIdeal) ;

	tree->Branch("ISREnergy" , &ISREnergy) ;
}


std::vector<int> HiggsProcessor::findDecayMode(LCCollection* _mcCol)
{
	//	std::cout << "HiggsProcessor::findDecayMode(LCCollection* mcCol)" << std::endl ;

	std::vector<int> toReturn ;
	if ( _mcCol->getNumberOfElements() < 10 )
		return toReturn ;

	MCParticleImpl* part = dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 9 ) ) ;

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

ISR HiggsProcessor::processISR()
{
	ISR isr ;

	MCParticleImpl* gamma0 = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 0 ) ) ;
	MCParticleImpl* gamma1 = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 1 ) ) ;

	CLHEP::HepLorentzVector vec0(gamma0->getMomentum()[0] , gamma0->getMomentum()[1] , gamma0->getMomentum()[2] , gamma0->getEnergy() ) ;
	CLHEP::HepLorentzVector vec1(gamma1->getMomentum()[0] , gamma1->getMomentum()[1] , gamma1->getMomentum()[2] , gamma1->getEnergy() ) ;

	isr.mcFourVec = {vec0 , vec1} ;

	return isr ;
}


CLHEP::HepLorentzVector HiggsProcessor::getZLorentzVectorMC(LCCollection* _mcCol)
{
	if ( _mcCol->getNumberOfElements() < 13 )
	{
		return CLHEP::HepLorentzVector(0,0,0,0) ;
	}

	const double* p1d = dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 11 ) )->getMomentum() ;
	const double* p2d = dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 12 ) )->getMomentum() ;

	CLHEP::Hep3Vector p1( p1d[0] , p1d[1] , p1d[2] ) ;
	CLHEP::Hep3Vector p2( p2d[0] , p2d[1] , p2d[2] ) ;
	CLHEP::Hep3Vector p = p1+p2 ;

	double e = dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 11 ) )->getEnergy() + dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 12 ) )->getEnergy() ;

	return CLHEP::HepLorentzVector(p , e) ;
}

CLHEP::HepLorentzVector HiggsProcessor::getHLorentzVectorMC(LCCollection* _mcCol)
{
	if ( _mcCol->getNumberOfElements() < 5 )
	{
		return CLHEP::HepLorentzVector(0,0,0,0) ;
	}

	const double* p1d = dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 4 ) )->getMomentum() ;

	CLHEP::Hep3Vector p( p1d[0] , p1d[1] , p1d[2] ) ;

	double e = dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 4 ) )->getEnergy()  ;

	int id = dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 4 ) )->getPDG() ;
	if ( id != 25 )
	{
		std::cout << "Error in HiggsProcessor::findDecayMode : Not a Higgs : " << id << std::endl ;
		//		throw ;
	}

	return CLHEP::HepLorentzVector(p , e) ;
}

double HiggsProcessor::getZEnergyMC()
{
	MCParticleImpl* part1 = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 11 ) )  ;
	MCParticleImpl* part2 = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 12 ) )  ;

	return part1->getEnergy() + part2->getEnergy() ;
}

double HiggsProcessor::getCosAngleBetweenZJetsMC()
{
	MCParticleImpl* part1 = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 11 ) )  ;
	MCParticleImpl* part2 = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 12 ) )  ;

	CLHEP::Hep3Vector p1( part1->getMomentum()[0] , part1->getMomentum()[1] , part1->getMomentum()[2] ) ;
	CLHEP::Hep3Vector p2( part2->getMomentum()[0] , part2->getMomentum()[1] , part2->getMomentum()[2] ) ;

	return ( p1.dot(p2) )/(p1.mag()*p2.mag()) ;
}


void HiggsProcessor::computeOriginMap()
{
	originMap.clear() ;
	for( int i = 0 ; i < linkCol->getNumberOfElements() ; ++i )
	{
		LCRelationImpl* link = dynamic_cast<LCRelationImpl*>( linkCol->getElementAt(i) ) ;
		ReconstructedParticleImpl* recoPart = dynamic_cast<ReconstructedParticleImpl*>( link->getFrom() ) ;
		MCParticleImpl* mcPart = dynamic_cast<MCParticleImpl*>( link->getTo() ) ;


		MCParticleImpl* mcPartParent = mcPart ;
		int mcPartOrigin = mcPartParent->getPDG() ;

		while ( true )
		{
			MCParticleVec parentVec = mcPartParent->getParents() ;

			if ( parentVec.size() )
			{
				mcPartParent = dynamic_cast<MCParticleImpl*>( parentVec.at(0) ) ;
				mcPartOrigin = mcPartParent->getPDG() ;
			}
			else
				break ;
		}

		originMap[recoPart].insert( { mcPartOrigin , recoPart->getEnergy()*link->getWeight() } ) ;
	}
}


std::map<int,float> HiggsProcessor::mcOriginOfParticle(ReconstructedParticleImpl* recoPart)
{
	std::map<int,float> toReturn ;

	auto it = originMap.find(recoPart) ;

	if ( it != originMap.end() )
		return it->second ;
	else
	{
		std::cout << "no mc origin for particle" << std::endl ;
		return toReturn ;
	}
}


std::map<int,float> HiggsProcessor::mcOriginOfJet(const fastjet::PseudoJet& jet , fastjet::ClusterSequence& cs)
{
	std::map<int,float> toReturn ;

	std::vector<fastjet::PseudoJet> particlesInJet = cs.constituents(jet) ;

	for( std::vector<fastjet::PseudoJet>::const_iterator it = particlesInJet.begin() ; it != particlesInJet.end() ; ++it )
	{
		std::map<int,float> mcOrigin = mcOriginOfParticle( recoParticles.at( it->user_index() ) ) ;

		for( std::map<int,float>::const_iterator jt = mcOrigin.begin() ; jt != mcOrigin.end() ; ++jt )
			toReturn[jt->first] += jt->second ;
	}

	return toReturn ;
}

DiJet HiggsProcessor::chooseZDiJet(const std::vector<fastjet::PseudoJet>& jets , std::vector<fastjet::PseudoJet>& remainingJets)
{
	remainingJets.clear() ;

	DiJet goodDiJet ;

	assert( jets.size() > 1 ) ;

	double chi2 = std::numeric_limits<double>::max() ;

	std::pair<unsigned int , unsigned int> goodPair ;

	for ( unsigned int i = 0 ; i < jets.size() ; ++i )
	{
		for ( unsigned int j = i+1 ; j < jets.size() ; ++j )
		{
			DiJet tempDiJet( jets.at(i) , jets.at(j) ) ;

			double tempChi2 = ( tempDiJet.diJet().m() - zMassRef )*( tempDiJet.diJet().m() - zMassRef ) ;

			if ( tempChi2 < chi2 )
			{
				chi2 = tempChi2 ;
				goodDiJet = tempDiJet ;
				goodPair = {i,j} ;
			}
		}
	}

	for ( unsigned int i = 0 ; i < jets.size() ; ++i )
	{
		if ( i == goodPair.first || i == goodPair.second )
			continue ;

		remainingJets.push_back( jets.at(i) ) ;
	}

	return goodDiJet ;
}

std::pair<DiJet,DiJet> HiggsProcessor::choosePairDiJets(const std::vector<fastjet::PseudoJet>& jets , const double& targetMass)
{
	assert( jets.size() == 4 ) ;
	double chi2 = std::numeric_limits<double>::max() ;

	std::pair<DiJet , DiJet> goodPair ;

	std::vector< std::array<unsigned int,4> > vec ;
	vec.push_back({{0,1,2,3}}) ;
	vec.push_back({{0,2,1,3}}) ;
	vec.push_back({{0,3,1,2}}) ;
	vec.push_back({{2,3,0,1}}) ;
	vec.push_back({{1,3,0,2}}) ;
	vec.push_back({{1,2,0,3}}) ;

	for ( const auto& i : vec )
	{
		std::pair<DiJet , DiJet> pair = { DiJet(jets.at(i.at(0)) , jets.at(i.at(1))) , DiJet(jets.at(i.at(2)) , jets.at(i.at(3))) } ;

		double a = (pair.first.diJet().m() - targetMass) ;
		double b = (pair.second.diJet().m() - targetMass) ;
		double tempChi2 = a*a + b*b ;

		if ( tempChi2 < chi2 )
		{
			chi2 = tempChi2 ;
			goodPair = pair ;
		}
	}

	return goodPair ;
}

std::pair<DiJet,DiJet> HiggsProcessor::choosePairOfZDiJets(const std::vector<fastjet::PseudoJet>& jets)
{
	assert( jets.size() == 4 ) ;
	return choosePairDiJets(jets , zMassRef) ;
}

std::pair<DiJet,DiJet> HiggsProcessor::choosePairOfWDiJets(const std::vector<fastjet::PseudoJet>& jets)
{
	assert( jets.size() == 4 ) ;
	return choosePairDiJets(jets , wMassRef) ;
}


void HiggsProcessor::processEvent(LCEvent* evt)
{
	currentEvt = evt ;
	mcCol = currentEvt->getCollection( mcPartColName ) ;
	linkCol = currentEvt->getCollection( linkColName ) ;
	recoCol = currentEvt->getCollection( recoPartColName ) ;

	computeOriginMap() ;

	particles.clear() ;
	zParticles.clear() ;
	hParticles.clear() ;
	allExceptHParticles.clear() ;

	recoParticles.clear() ;


	//get all the reconstructed particles
	for ( int i = 0 ; i < recoCol->getNumberOfElements() ; ++i )
	{
		ParticleInfo* partInfo = new ParticleInfo ;
		ReconstructedParticleImpl* recoPart = dynamic_cast<ReconstructedParticleImpl*>( recoCol->getElementAt(i) ) ;
		partInfo->setRecoParticle( recoPart ) ;
		partInfo->setId(i) ;

		std::map<int,float> mcorig = mcOriginOfParticle( partInfo->recoParticle() ) ;

		double temp = 0 ;
		int id = 0 ;
		for ( std::map<int,float>::const_iterator jt = mcorig.begin() ; jt != mcorig.end() ; ++jt )
		{
			if ( jt->second > temp )
			{
				temp = jt->second ;
				id = jt->first ;
			}
		}
		partInfo->setOrigin(id) ;

		fastjet::PseudoJet toto( recoPart->getMomentum()[0] , recoPart->getMomentum()[1] , recoPart->getMomentum()[2] , recoPart->getEnergy() ) ;
		toto.set_user_index(i) ;
		toto.set_user_info(partInfo) ;
		particles.push_back( toto ) ;
		recoParticles.push_back( recoPart ) ;
	}


	totalEnergy = 0 ;
	double totalZEnergy = 0.0 ;

	for ( const auto particle : particles )
	{
		totalEnergy += particle.E() ;
		if ( std::abs( particle.user_info<ParticleInfo>().origin() ) <= 6 )
		{
			zParticles.push_back(particle) ;
			totalZEnergy += particle.E() ;
		}

		if ( particle.user_info<ParticleInfo>().origin() == 25 )
			hParticles.push_back(particle) ;
		else
			allExceptHParticles.push_back(particle) ;
	}



	//compute perfect Z and H jets
	assert(zParticles.size() > 1) ;

	//jets of Z using only Z particles -> (theorically perfect Z jets)
	fastjet::JetDefinition jDZideal(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csZideal(zParticles , jDZideal) ;
	auto jetsZ = sorted_by_pt( csZideal.exclusive_jets(2) ) ;

	//jets of H using only H particles -> (theorically perfect H jets)
	int targetNJetsHideal = std::min(static_cast<int>(hParticles.size()) , 2 ) ;

	fastjet::JetDefinition jDHideal(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csHideal(hParticles , jDHideal) ;
	auto jetsH = sorted_by_pt( csHideal.exclusive_jets(targetNJetsHideal) ) ;

	auto idealZDiJet = DiJet( jetsZ.at(0) , jetsZ.at(1) ) ;

	cosThetaZDiJetIdeal = idealZDiJet.getCosAngleBetweenJets() ;
	if ( jetsH.size() > 1 )
	{
		auto idealHDiJet = DiJet( jetsH.at(0) , jetsH.at(1) ) ;
		cosThetaHDiJetIdeal = idealHDiJet.getCosAngleBetweenJets() ;
	}
	else
		cosThetaHDiJetIdeal = 1.0 ;


	zMassIdeal = idealZDiJet.diJet().m() ;

	double idealpZ = idealZDiJet.diJet().pt()*idealZDiJet.diJet().pt() + idealZDiJet.diJet().pz()*idealZDiJet.diJet().pz() ;
	recMassIdeal = std::sqrt( (sqrtS - idealZDiJet.diJet().e() )*(sqrtS - idealZDiJet.diJet().e() ) - idealpZ ) ;


	std::vector<int> vec = findDecayMode( evt->getCollection(mcPartColName) ) ;


	ISR isr = processISR() ;

	ISREnergy = 0.0 ;
	for ( const auto& i : isr.mcFourVec )
		ISREnergy += i.e() ;


	//WW and ZZ pair (background study)
	fastjet::JetDefinition jD4(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence cs4(particles , jD4) ;
	auto jets4 = sorted_by_pt( cs4.exclusive_jets(4) ) ;

	auto wwPair = choosePairOfWDiJets(jets4) ;
	auto zzPair = choosePairOfZDiJets(jets4) ;

	ww12mass = wwPair.first.diJet().m() ;
	ww34mass = wwPair.second.diJet().m() ;

	zz12mass = zzPair.first.diJet().m() ;
	zz34mass = zzPair.second.diJet().m() ;


	//main ZH clustering
	fastjet::JetDefinition jDZH(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csZH(particles , jDZH) ;
	auto jets = sorted_by_pt( csZH.exclusive_jets(4) ) ;
//	auto jets = sorted_by_pt( csZH.exclusive_jets(dMin) ) ;

	for ( auto& jet : jets )
	{
		JetInfo* info = new JetInfo ;
		info->setMcOrigin( mcOriginOfJet(jet , csZH) ) ;
		jet.set_user_info(info) ;
	}

	assert ( jets.size() > 1 ) ;

	totalEnergyJets = 0.0 ;
	for ( const auto& jet : jets )
		totalEnergyJets += jet.e() ;

	nJets = jets.size() ;

	std::vector<fastjet::PseudoJet> remainingJets ;
	auto zDiJet = chooseZDiJet(jets , remainingJets) ;

	std::vector<fastjet::PseudoJet> remainingParticles ;
	for ( const auto& jet : remainingJets )
	{
		for ( const auto& particle : jet.constituents() )
			remainingParticles.push_back(particle) ;
	}


	zMass = zDiJet.diJet().m() ;
	double pZ = zDiJet.diJet().pt()*zDiJet.diJet().pt() + zDiJet.diJet().pz()*zDiJet.diJet().pz() ;
	recMass = std::sqrt( (sqrtS - zDiJet.diJet().e() )*(sqrtS - zDiJet.diJet().e() ) - pZ ) ;


	int targetNJetsH = std::min(static_cast<int>(remainingParticles.size()) , 2 ) ;

	fastjet::JetDefinition jDH(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csH(remainingParticles , jDH) ;
	auto hJets = sorted_by_pt( csH.exclusive_jets(targetNJetsH) ) ;

	cosThetaZDiJet = zDiJet.getCosAngleBetweenJets() ;
	z1e = zDiJet.jet1().e() ;
	z2e = zDiJet.jet2().e() ;

	if ( hJets.size() == 2 )
	{
		auto hDiJet = DiJet(hJets.at(0) , hJets.at(1)) ;
		cosThetaHDiJet = hDiJet.getCosAngleBetweenJets() ;
		h1e = hDiJet.jet1().e() ;
		h2e = hDiJet.jet2().e() ;
	}
	else if ( hJets.size() == 1 )
	{
		cosThetaHDiJet = 1.0 ;
		h1e = hJets.at(0).e() ;
		h2e = 0.0 ;
	}
	else
	{
		cosThetaHDiJet = 0.0 ;
		h1e = 0.0 ;
		h2e = 0.0 ;
	}

	std::vector<double> totalEnergyJetsVec(nJets , 0.0) ;

	double zTaggedEnergy = 0 ;
	double hTaggedEnergy = 0 ;

	zPurityJets = std::vector<double>(nJets , 0.0) ;

	for ( const auto& origin : zDiJet.jet1().user_info<JetInfo>().mcOrigin() )
	{
		if ( std::abs(origin.first) <= 6 )
		{
			zTaggedEnergy += origin.second ;
			zPurityJets.at(0) += origin.second ;
		}
		if ( std::abs(origin.first) == 25 )
			hTaggedEnergy += origin.second ;

		totalEnergyJetsVec.at(0) += origin.second ;
	}

	for ( const auto& origin : zDiJet.jet2().user_info<JetInfo>().mcOrigin() )
	{
		if ( std::abs(origin.first) <= 6 )
		{
			zTaggedEnergy += origin.second ;
			zPurityJets.at(1) += origin.second ;
		}
		if ( std::abs(origin.first) == 25 )
			hTaggedEnergy += origin.second ;

		totalEnergyJetsVec.at(1) += origin.second ;
	}

	for ( unsigned int i = 0 ; i < remainingJets.size() ; ++i )
	{
		for ( const auto& origin : remainingJets.at(i).user_info<JetInfo>().mcOrigin() )
		{
			if ( std::abs(origin.first) <= 6 )
				zPurityJets.at(2+i) += origin.second ;

			totalEnergyJetsVec.at(2+i) += origin.second ;
		}
	}


	for ( unsigned int i = 0 ; i < nJets ; ++i)
		zPurityJets.at(i) /= totalEnergyJetsVec.at(i) ;

	zTagged = zTaggedEnergy/totalZEnergy ;
	zPurity = zTaggedEnergy/(zDiJet.jet1().e() + zDiJet.jet2().e()) ;

	decayID = vec.at(0) ;
	tree->Fill() ;

	std::cout << "Event " << evt->getEventNumber() << std::endl ;
}


void HiggsProcessor::check(LCEvent* )
{
}


void HiggsProcessor::end()
{
	tree->Write() ;
	file->Close() ;
}
