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

}

void HiggsProcessor::init()
{
	std::cout << "HiggsProcessor::init()" << std::endl ;
	file = new TFile(rootFileName.c_str() , "RECREATE") ;
	tree = new TTree("tree" , "tree") ;

	tree->Branch("eventNumber" , &eventNumber) ;

	tree->Branch("decayID" , &decayID) ;
	tree->Branch("totalEnergy" , &totalEnergy) ;
	tree->Branch("energy4Jets" , &energy4Jets) ;
	tree->Branch("ISREnergy" , &ISREnergy) ;
	tree->Branch("energyISRVec" , &energyISRVec) ;
	tree->Branch("etaISRVec" , &etaISRVec) ;

	tree->Branch("yValueVec" , &yValueVec) ;
	tree->Branch("zMass" , &zMass) ;
	tree->Branch("recMass" , &recMass) ;
	tree->Branch("recMass2" , &recMass2) ;

	tree->Branch("invRecMass" , &invRecMass) ;

	tree->Branch("hDiJetMass" , &hDiJetMass) ;
	tree->Branch("zEnergy" , &zEnergy) ;
	tree->Branch("cosAngleBetweenZJets" , &cosAngleBetweenZJets) ;
	tree->Branch("cosAngleBetweenHJets" , &cosAngleBetweenHJets) ;

	tree->Branch("maxCosBetweenTwoJets" , &maxCosBetweenTwoJets) ;

	tree->Branch("zPhi" , &zPhi) ;
	tree->Branch("zRap" , &zRap) ;
	tree->Branch("hPhi" , &hPhi) ;
	tree->Branch("hRap" , &hRap) ;

	tree->Branch("ww12mass" , &ww12mass) ;
	tree->Branch("ww34mass" , &ww34mass) ;
	tree->Branch("zz12mass" , &zz12mass) ;
	tree->Branch("zz34mass" , &zz34mass) ;

	tree->Branch("zPurity" , &zPurity) ;
	tree->Branch("hCont" , &hCont) ;

	tree->Branch("zPurityJets" , &zPurityJets) ;

	tree->Branch("idealRecoZmass" , &idealRecoZmass) ;
	tree->Branch("idealRecoHmass" , &idealRecoHmass) ;
	tree->Branch("idealRecoRecMass" , &idealRecoRecMass) ;

	tree->Branch("phiVec" , &phiVec) ;
	tree->Branch("thetaVec" , &thetaVec) ;
	tree->Branch("jetVec" , &jetVec) ;

	tree->Branch("zMassMC" , &zMassMC) ;
	tree->Branch("recMassMC" , &recMassMC) ;
	tree->Branch("zEnergyMC" , &zEnergyMC) ;

	tree->Branch("cosAngleBetweenZJetsMC" , &cosAngleBetweenZJetsMC) ;
	tree->Branch("cosAngleBetweenHJetsMC" , &cosAngleBetweenHJetsMC) ;
	tree->Branch("zPhiMC" , &zPhiMC) ;
	tree->Branch("zRapMC" , &zRapMC) ;
	tree->Branch("hPhiMC" , &hPhiMC) ;
	tree->Branch("hRapMC" , &hRapMC) ;


	tree->Branch("inclRecMass" , &inclRecMass) ;
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

		originMap[recoPart].insert( std::make_pair(mcPartOrigin, recoPart->getEnergy()*link->getWeight()) ) ;
	}
}




std::map<int,float> HiggsProcessor::mcOriginOfParticle(ReconstructedParticleImpl* recoPart)
{
	std::map<int,float> toReturn ;

	std::map<ReconstructedParticleImpl* , std::map<int,float> >::iterator it = originMap.find(recoPart) ;

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

template<typename T>
std::vector<fastjet::PseudoJet> HiggsProcessor::exclusiveJets(T value)
{
	fastjet::JetDefinition jet_def(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence cs(particles, jet_def) ;

	auto fastJets = sorted_by_pt( cs.exclusive_jets(value) ) ;

	std::vector<fastjet::PseudoJet> jets ;

	for ( auto& fastJet : fastJets )
	{
		JetInfo* info = new JetInfo ;
		info->setMcOrigin( mcOriginOfJet(fastJet , cs) ) ;
		fastJet.set_user_info(info) ;
		jets.push_back(fastJet) ;
	}

	return jets ;
}


//std::vector<fastjet::PseudoJet> HiggsProcessor::exclusiveJets(double dcut)
//{
//	fastjet::JetDefinition jet_def(fastjet::ee_kt_algorithm) ;
//	fastjet::ClusterSequence cs(particles, jet_def) ;

//	auto fastJets = sorted_by_pt( cs.exclusive_jets(dcut) ) ;

//	std::vector<fastjet::PseudoJet> jets ;

//	for ( auto& fastJet : fastJets )
//	{
//		JetInfo* info = new JetInfo ;
//		info->setMcOrigin( mcOriginOfJet(fastJet , cs) ) ;
//		fastJet.set_user_info(info) ;
//		jets.push_back(fastJet) ;
//	}

//	return jets ;
//}

//std::vector<fastjet::PseudoJet> HiggsProcessor::exclusiveJets(int nJets)
//{
//	fastjet::JetDefinition jet_def(fastjet::ee_kt_algorithm) ;
//	fastjet::ClusterSequence cs(particles, jet_def) ;

//	auto fastJets = sorted_by_pt( cs.exclusive_jets(nJets) ) ;

//	std::vector<fastjet::PseudoJet> jets ;

//	for ( auto& fastJet : fastJets )
//	{
//		JetInfo* info = new JetInfo ;
//		info->setMcOrigin( mcOriginOfJet(fastJet , cs) ) ;
//		fastJet.set_user_info(info) ;
//		jets.push_back(fastJet) ;
//	}

//	return jets ;
//}

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



	thetaVec.clear() ;
	phiVec.clear() ;
	jetVec.clear() ;

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

		thetaVec.push_back( toto.pseudorapidity() ) ;
		phiVec.push_back( toto.phi() ) ;
	}


	totalEnergy = 0 ;

	for ( const auto particle : particles )
	{
		totalEnergy += particle.E() ;
		if ( std::abs( particle.user_info<ParticleInfo>().origin() ) <= 6 )
			zParticles.push_back(particle) ;

		if ( particle.user_info<ParticleInfo>().origin() == 25 )
			hParticles.push_back(particle) ;
		else
			allExceptHParticles.push_back(particle) ;
	}



	//compute perfect Z and H jets

	fastjet::JetDefinition jetDefZ(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csZ(zParticles, jetDefZ) ;

	int targetNjetsZ = zParticles.size() ;
	if ( targetNjetsZ > 2 )
		targetNjetsZ = 2 ;

	//jets of Z using only Z particles -> (theorically perfect Z jets)
	auto jetsZ = exclusiveJets(2) ;

	fastjet::JetDefinition jetDefH(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csH(hParticles, jetDefH) ;

	//jets of H using only H particles -> (theorically perfect H jets)
	auto jetsH = exclusiveJets(2) ;



	if ( jetsH.size() < 2 )
		cosAngleBetweenHJetsMC = 1 ;
	else
	{
		CLHEP::Hep3Vector p1( jetsH.at(0).px() , jetsH.at(0).py() , jetsH.at(0).pz() ) ;
		CLHEP::Hep3Vector p2( jetsH.at(1).px() , jetsH.at(1).py() , jetsH.at(1).pz() ) ;

		cosAngleBetweenHJetsMC = ( p1.dot(p2) )/(p1.mag()*p2.mag()) ;
	}



	//jets for invisible higgs
	fastjet::JetDefinition jetDefNoH(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csNoH(allExceptHParticles, jetDefNoH) ;

	auto jetsNoH = exclusiveJets(2) ;



	auto idealZDiJet = DiJet( jetsZ.at(0) , jetsZ.at(1) ) ;
	auto idealHDiJet = DiJet( jetsH.at(0) , jetsH.at(1) ) ;

	zPhiMC = idealZDiJet.diJet().phi() ;
	zRapMC = idealZDiJet.diJet().pseudorapidity() ;

	hPhiMC = idealZDiJet.diJet().phi() ;
	hRapMC = idealZDiJet.diJet().pseudorapidity() ;


	zEnergyMC = getZEnergyMC() ;
	cosAngleBetweenZJetsMC = getCosAngleBetweenZJetsMC() ;


	double pt = 2*std::sqrt(sqrtS*sqrtS/4.0 - 511e-6*511e-6)*std::sin(7e-3) ;

	CLHEP::Hep3Vector totP(pt,0,0) ;


	idealRecoZmass = idealZDiJet.diJet().m() ;
	idealRecoHmass = idealHDiJet.diJet().m() ;
	CLHEP::Hep3Vector idealPZrec( idealZDiJet.diJet().px() , idealZDiJet.diJet().py() , idealZDiJet.diJet().pz() ) ;

	idealRecoRecMass = std::sqrt( (sqrtS - idealZDiJet.diJet().e())*(sqrtS - idealZDiJet.diJet().e()) - ( idealPZrec.mag2() + totP.mag2() -2*idealPZrec.dot(totP) ) ) ;


	using fastjet::PseudoJet ;
	std::vector<int> vec = findDecayMode( evt->getCollection(mcPartColName) ) ;

	//	std::cout << std::endl ;
	//	for ( unsigned int i = 0 ; i < vec.size() ; ++i )
	//		std::cout << vec.at(i) << " " ;
	//	std::cout << std::endl ;

	ISR isr = processISR() ;

	ISREnergy = 0.0 ;
	etaISRVec.clear() ;
	energyISRVec.clear() ;
	for ( const auto& i : isr.mcFourVec )
	{
		ISREnergy += i.e() ;
		energyISRVec.push_back(i.e()) ;
		etaISRVec.push_back(i.pseudoRapidity()) ;
	}

	CLHEP::HepLorentzVector zVec = getZLorentzVectorMC( evt->getCollection(mcPartColName) ) ;
	CLHEP::HepLorentzVector hVec = getHLorentzVectorMC( evt->getCollection(mcPartColName) ) ;

	CLHEP::Hep3Vector zP = zVec.getV() ;
	CLHEP::Hep3Vector zH = hVec.getV() ;



	zMassMC = zVec.m() ;

	recMassMC = std::sqrt( (sqrtS - zVec.e())*(sqrtS - zVec.e()) - ( zP.mag2() + totP.mag2() -2*zP.dot(totP) ) ) ;



	fastjet::JetDefinition jet_def(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence cs(particles, jet_def) ;


	auto jets = exclusiveJets(4) ;

	energy4Jets = 0.0 ;
	for ( std::vector<PseudoJet>::const_iterator it = jets.begin() ; it != jets.end() ; ++it )
		energy4Jets += it->e() ;

	yValueVec.clear() ;
	yValueVec.push_back(0) ;

	for ( int i = 1 ; i < 6 ; ++i )
		yValueVec.push_back( -std::log10( cs.exclusive_ymerge(i) ) ) ;


	assert ( jets.size() == 4 ) ;

	maxCosBetweenTwoJets = -1.0 ;
	for ( int a = 0 ; a < 3 ; ++a )
	{
		for ( int b = a+1 ; b < 3 ; ++b )
		{
			DiJet toto{jets[a] , jets[b]} ;
			maxCosBetweenTwoJets = std::max(maxCosBetweenTwoJets , toto.getCosAngleBetweenJets()) ;
		}
	}

	auto jets4 = exclusiveJets(4) ;

	//WW and ZZ pair
	auto wwPair = choosePairOfWDiJets(jets4) ;
	auto zzPair = choosePairOfZDiJets(jets4) ;

	ww12mass = wwPair.first.diJet().m() ;
	ww34mass = wwPair.second.diJet().m() ;

	zz12mass = zzPair.first.diJet().m() ;
	zz34mass = zzPair.second.diJet().m() ;


	std::vector<fastjet::PseudoJet> remainingJets ;
	auto zDiJet = chooseZDiJet(jets4 , remainingJets) ;

	assert( remainingJets.size() == 2 ) ;
	auto hDiJet = DiJet(remainingJets.at(0) , remainingJets.at(1)) ;


	std::vector<double> totalEnergyJets(4 , 0.0) ;

	double zTotEnergy = 0 ;
	double hTotEnergy = 0 ;

	for ( const auto& jet : jets)
	{
		for ( const auto& origin : jet.user_info<JetInfo>().mcOrigin() )
		{
			if ( std::abs(origin.first) <= 6 )
				zTotEnergy += origin.second ;
			if ( std::abs(origin.first) == 25 )
				hTotEnergy += origin.second ;
		}
	}


	double zTaggedEnergy = 0 ;
	double hTaggedEnergy = 0 ;

	zPurityJets = std::vector<double>(4,0.0) ;


	for ( const auto& origin : zDiJet.jet1().user_info<JetInfo>().mcOrigin() )
	{
		if ( std::abs(origin.first) <= 6 )
		{
			zTaggedEnergy += origin.second ;
			zPurityJets.at(0) += origin.second ;
		}
		if ( std::abs(origin.first) == 25 )
			hTaggedEnergy += origin.second ;

		totalEnergyJets.at(0) += origin.second ;
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

		totalEnergyJets.at(1) += origin.second ;
	}


	for ( const auto& origin : hDiJet.jet1().user_info<JetInfo>().mcOrigin() )
	{
		if ( std::abs(origin.first) <= 6 )
			zPurityJets.at(2) += origin.second ;

		totalEnergyJets.at(2) += origin.second ;
	}

	for ( const auto& origin : hDiJet.jet2().user_info<JetInfo>().mcOrigin() )
	{
		if ( std::abs(origin.first) <= 6 )
			zPurityJets.at(3) += origin.second ;

		totalEnergyJets.at(3) += origin.second ;
	}


	for ( unsigned int i = 0 ; i < 4 ; ++i)
		zPurityJets.at(i) /= totalEnergyJets.at(i) ;


	cosAngleBetweenZJets = zDiJet.getCosAngleBetweenJets() ;
	cosAngleBetweenHJets = hDiJet.getCosAngleBetweenJets() ;

	zEnergy = zDiJet.diJet().E() ;

	zPurity = zTaggedEnergy/zTotEnergy ;
	hCont = hTaggedEnergy/hTotEnergy ;

	zPhi = zDiJet.diJet().phi() ;
	zRap = zDiJet.diJet().pseudorapidity() ;

	hPhi = hDiJet.diJet().phi() ;
	hRap = hDiJet.diJet().pseudorapidity() ;



	double pqq = zDiJet.diJet().pt()*zDiJet.diJet().pt() + zDiJet.diJet().pz()*zDiJet.diJet().pz() ;

	CLHEP::Hep3Vector pZrec( zDiJet.diJet().px() , zDiJet.diJet().py() , zDiJet.diJet().pz() ) ;


	double mrec = (sqrtS - zDiJet.diJet().e() )*(sqrtS - zDiJet.diJet().e() ) - pqq ;


	zMass = zDiJet.diJet().m() ;


	recMass2 = std::sqrt( (sqrtS - zDiJet.diJet().e())*(sqrtS - zDiJet.diJet().e()) - ( pZrec.mag2() + totP.mag2() -2*pZrec.dot(totP) ) ) ;

	hDiJetMass = hDiJet.diJet().m() ;

	//	pZrec = CLHEP::Hep3Vector( zDiJet.diJet().px() , zDiJet.diJet().py() , zDiJet.diJet().pz() ) ;

	recMass = std::sqrt(mrec) ;

	//	for ( unsigned int i = 0 ; i < vec.size() ; ++i )
	//		std::cout << vec.at(i) << " " ;

	//	std::cout << std::endl ;

	//invisible recoilMass
	DiJet invDiJet( jetsNoH.at(0) , jetsNoH.at(1) ) ;

	double invPqq = invDiJet.diJet().pt()*invDiJet.diJet().pt() + invDiJet.diJet().pz()*invDiJet.diJet().pz() ;
	invRecMass = std::sqrt( (sqrtS - invDiJet.diJet().e() )*(sqrtS - invDiJet.diJet().e() ) - invPqq ) ;


	//inclusive jets
	//	DiJet inclDiJet = inclusiveZDiJet(375) ;
	//	double inclPqq = inclDiJet.diJet().pt()*inclDiJet.diJet().pt() + inclDiJet.diJet().pz()*inclDiJet.diJet().pz() ;
	//	inclRecMass = std::sqrt( (sqrtS - inclDiJet.diJet().e() )*(sqrtS - inclDiJet.diJet().e() ) - inclPqq ) ;

	decayID = vec.at(0) ;
	tree->Fill() ;


	std::cout << "Event " << evt->getEventNumber() << std::endl ;

	eventNumber++ ;
}


void HiggsProcessor::check(LCEvent* )
{
}


void HiggsProcessor::end()
{
	tree->Write() ;
	file->Close() ;
}
