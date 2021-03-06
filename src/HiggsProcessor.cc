#include "HiggsProcessor.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <complex>
#include <set>

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

#include "EventShape.h"

using namespace lcio ;
using namespace marlin ;

HiggsProcessor aHiggsProcessor ;

HiggsProcessor::HiggsProcessor()
	: Processor("HiggsProcessor") ,
	  jetDef(fastjet::ee_kt_algorithm) ,
	  originMap()
{

	registerProcessorParameter( "RootFileName" ,
								"File name for the root output",
								rootFileName ,
								std::string("test.root") ) ;

	registerProcessorParameter( "ReconstructedParticlesCollectionName" ,
								"Name of the reconstructed particles collection" ,
								recoPartColName ,
								std::string("PandoraPFOs") ) ;

	registerProcessorParameter( "IsolatedLeptonCollectionName" ,
								"Name of the isolated lepton collection" ,
								isoLepColName ,
								std::string("Isolep") ) ;

	registerProcessorParameter( "MCParticlesCollectionName" ,
								"Name of the mc particles collection" ,
								mcPartColName ,
								std::string("MCParticle") ) ;

	registerProcessorParameter( "LinkCollectionName" ,
								"Name of the mc truth link collection" ,
								linkColName ,
								std::string("RecoMCTruthLink") ) ;

	registerProcessorParameter( "ProcessID" ,
								"ProcessID" ,
								processID ,
								0 ) ;

	registerProcessorParameter( "CenterOfMassEnergy" ,
								"CenterOfMassEnergy" ,
								sqrtS ,
								250.0 ) ;

	registerProcessorParameter( "yCut" ,
								"yCut" ,
								yCut ,
								0.0 ) ;

	registerProcessorParameter( "alphaAngle" ,
								"alphaAngle" ,
								alphaAngle ,
								0.0 ) ;

	registerProcessorParameter( "valueAngle" ,
								"valueAngle" ,
								valueAngle ,
								0.0 ) ;
}

void HiggsProcessor::init()
{
	assert( processID != 0 ) ;

	std::cout << "HiggsProcessor::init()" << std::endl ;
	file = new TFile(rootFileName.c_str() , "RECREATE") ;
	tree = new TTree("tree" , "tree") ;

	tree->Branch("runNumber" , &runNumber) ;
	tree->Branch("evtNumber" , &evtNumber) ;
	tree->Branch("processID" , &processID) ;

	tree->Branch("failedStep" , &failedStep) ;

	tree->Branch("bigPhoton" , &bigPhoton) ;

	tree->Branch("sqrtS" , &sqrtS) ;

	tree->Branch("nJets" , &nJets) ;
	tree->Branch("nIsoLep" , &nIsoLep) ;

	tree->Branch("sphericity" , &sphericity) ;
	tree->Branch("cosThrust" , &cosThrust) ;
	tree->Branch("majorThrust" , &majorThrust) ;
	tree->Branch("minorThrust" , &minorThrust) ;

	tree->Branch("y23" , &y23) ;
	tree->Branch("y34" , &y34) ;

	tree->Branch("zMass" , &zMass) ;
	tree->Branch("recMass" , &recMass) ;

	tree->Branch("validVec" , &validVec) ;
	tree->Branch("zMassVec" , &zMassVec) ;
	tree->Branch("recMassVec" , &recMassVec) ;
	tree->Branch("cosThetaZVec" , &cosThetaZVec) ;
	tree->Branch("zPtVec" , &zPtVec) ;

	tree->Branch("cosThetaZ" , &cosThetaZ) ;

	tree->Branch("cosThetaZ12" , &cosThetaZ12) ;
	tree->Branch("cosThetaH12" , &cosThetaH12) ;


	tree->Branch("z1e" , &z1e) ;
	tree->Branch("z2e" , &z2e) ;
	tree->Branch("h1e" , &h1e) ;
	tree->Branch("h2e" , &h2e) ;

	tree->Branch("zPt" , &zPt) ;

	tree->Branch("mass2Jet" , &mass2Jet) ;
	tree->Branch("cosBetw2Jet" , &cosBetw2Jet) ;

	tree->Branch("ww12mass" , &ww12mass) ;
	tree->Branch("ww34mass" , &ww34mass) ;
	tree->Branch("zz12mass" , &zz12mass) ;
	tree->Branch("zz34mass" , &zz34mass) ;

	tree->Branch("wwMass3" , &wwMass3) ;
	tree->Branch("wwRecMass3" , &wwRecMass3) ;

	tree->Branch("w12pt" , &w12pt) ;
	tree->Branch("z12pt" , &z12pt) ;
	tree->Branch("w3pt" , &w3pt) ;

	tree->Branch("totalEnergy" , &totalEnergy) ;
	tree->Branch("totalEnergyJets" , &totalEnergyJets) ;

	tree->Branch("pMiss" , &pMiss) ;
	tree->Branch("pMissNorm" , &pMissNorm) ;
	tree->Branch("cosThetaMiss" , &cosThetaMiss) ;



	//MC infos
	tree->Branch("decayID" , &decayID) ;
	tree->Branch("subDecayID" , &subDecayID) ;

	tree->Branch("zPurity" , &zPurity) ;
	tree->Branch("zTagged" , &zTagged) ;

	tree->Branch("zMassIdeal" , &zMassIdeal) ;
	tree->Branch("recMassIdeal" , &recMassIdeal) ;

	tree->Branch("cosThetaZIdeal" , &cosThetaZIdeal) ;

	tree->Branch("cosThetaBetwZDiJetIdeal" , &cosThetaBetwZDiJetIdeal) ;
	tree->Branch("cosThetaBetwHDiJetIdeal" , &cosThetaBetwHDiJetIdeal) ;

	tree->Branch("neutrinoEnergy" , &neutrinoEnergy) ;
	tree->Branch("ISREnergy" , &ISREnergy) ;
}

void HiggsProcessor::cleanEvent()
{
	recoParticles.clear() ;

	particles.clear() ;
	zParticles.clear() ;
	hParticles.clear() ;
	allExceptHParticles.clear() ;

	for ( auto& it : fixedJets )
		it.clear() ;

	originMap.clear() ;

	failedStep = 0 ;

	bigPhoton = false ;

	nJets = 0 ;
	nIsoLep = 0 ;

	sphericity = 0 ;
	cosThrust = 0 ;
	majorThrust = 0 ;
	minorThrust = 0 ;

	y23 = 0 ;
	y34 = 0 ;

	zMass = 0 ;
	recMass = 0 ;

	validVec.clear() ;
	zMassVec.clear() ;
	recMassVec.clear() ;
	cosThetaZVec.clear() ;
	zPtVec.clear() ;

	cosThetaZ = 0 ;

	cosThetaZ12 = 0 ;
	cosThetaH12 = 0 ;

	z1e = 0 ;
	z2e = 0 ;
	h1e = 0 ;
	h2e = 0 ;

	zPt = 0 ;

	mass2Jet = 0 ;
	cosBetw2Jet = 0 ;
	ww12mass = 0 ;
	ww34mass = 0 ;
	zz12mass = 0 ;
	zz34mass = 0 ;

	wwMass3 = 0 ;
	wwRecMass3 = 0 ;

	w12pt = 0 ;
	z12pt = 0 ;
	w3pt = 0 ;

	totalEnergy = 0 ;
	totalEnergyJets = 0 ;

	pMiss.clear() ;
	pMissNorm = 0 ;
	cosThetaMiss = 0 ;

	decayID = 0 ;
	subDecayID = 0 ;

	zPurity = 0 ;
	zTagged = 0 ;

	zMassIdeal = 0 ;
	recMassIdeal = 0 ;

	cosThetaZIdeal = 0 ;

	cosThetaBetwZDiJetIdeal = 0 ;
	cosThetaBetwHDiJetIdeal = 0 ;

	neutrinoEnergy = 0 ;
	ISREnergy = 0 ;

	zMassMC = 0 ;
	recMassMC = 0 ;
}

std::pair<int,int> HiggsProcessor::findDecayModeSignal(LCCollection* _mcCol)
{
	std::pair<int,int> toReturn ;
	int decay1 = 0 ;
	int decay2 = 0 ;

	if ( _mcCol->getNumberOfElements() < 10 )
		return {0,0} ;

	MCParticleImpl* part = dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 9 ) ) ;

	if ( part->getPDG() != 25 )
	{
		std::cout << "Error in HiggsProcessor::findDecayMode : Not a Higgs : " << part->getPDG() << std::endl ;
		return toReturn ;
	}

	MCParticleVec vec = part->getDaughters() ;

	assert ( vec.size() == 2 ) ;

	toReturn = { std::abs( vec.at(0)->getPDG() ) , std::abs( vec.at(1)->getPDG() ) } ;

	decay1 = toReturn.first ;


	if ( toReturn.first != 22 && toReturn.first != 23 )
		assert( toReturn.first == toReturn.second ) ;


	if ( ( toReturn.first == 24 || toReturn.first == 23 ) && toReturn.first == toReturn.second )
	{
		auto vec0 = vec[0]->getDaughters() ;
		auto vec1 = vec[1]->getDaughters() ;

		assert( vec0.size() == 2 && vec1.size() == 2 ) ;

		std::array<int,4> subDecay = {{ std::abs(vec0[0]->getPDG()) , std::abs(vec0[1]->getPDG()) , std::abs(vec1[0]->getPDG()) , std::abs(vec1[1]->getPDG()) }} ;

		std::sort( subDecay.begin() , subDecay.end() ) ;

		if ( subDecay[1] < 10 ) //qq--
		{
			if ( subDecay[3] < 10 ) //qqqq
				decay2 = 1 ;
			else if ( subDecay[2]%2 != 0 && subDecay[3]%2 != 0 ) //qqll
			{
				if ( subDecay[2] == 11 )
					decay2 = 21 ;
				else if ( subDecay[2] == 13 )
					decay2 = 22 ;
				else if ( subDecay[2] == 15 )
					decay2 = 23 ;
				else
					throw std::logic_error("weird qqll decay") ;
			}
			else if ( subDecay[2]%2 == 0 && subDecay[3]%2 == 0 ) //qqvv
			{
				decay2 = 4 ;
			}
			else //qqlv
			{
				if ( subDecay[2] == 11 )
					decay2 = 31 ;
				else if ( subDecay[2] == 13 )
					decay2 = 32 ;
				else if ( subDecay[2] == 15 )
					decay2 = 33 ;
				else
					throw std::logic_error("weird qqlv decay") ;
			}
		}
		else
		{
			int nbNu = 0 ;
			for ( const auto& i : subDecay )
			{
				if ( i%2 == 0 )
					nbNu++ ;
			}

			if ( nbNu == 0 ) //llll
			{
				decay2 = 500 ;
				if ( subDecay[0] == 11 )
					decay2 += 10 ;
				else if ( subDecay[0] == 13 )
					decay2 += 20 ;
				else if ( subDecay[0] == 15 )
					decay2 += 30 ;
				else
					throw std::logic_error("weird llll decay") ;

				if ( subDecay[2] == 11 )
					decay2 += 1 ;
				else if ( subDecay[2] == 13 )
					decay2 += 2 ;
				else if ( subDecay[2] == 15 )
					decay2 += 3 ;
				else
					throw std::logic_error("weird llll decay") ;
			}
			else if ( nbNu == 2 ) //llvv
			{
				decay2 = 600 ;
				std::vector<int> temp = {} ;
				for ( const auto& i : subDecay )
				{
					if ( i%2 != 0 )
					{
						if ( i == 11 )
							temp.push_back(1) ;
						else if ( i == 13 )
							temp.push_back(2) ;
						else if ( i == 15 )
							temp.push_back(3) ;
						else
							throw std::logic_error("weird llvv decay") ;
					}
				}

				if ( temp.size() != 2 )
					throw std::logic_error("weird llvv decay") ;

				decay2 = decay2 + 10*temp[0] + temp[1] ;
			}
			else //vvvv
			{
				decay2 = 7 ;
			}
		}
	}


	if ( toReturn.first != toReturn.second )
		decay1 = 25 ;

	toReturn = {decay1 , decay2} ;
	return toReturn ;
}

std::pair<int,int> HiggsProcessor::findDecayModeWWqqqq(LCCollection* _mcCol)
{
	return {0,0} ;
}

std::pair<int,int> HiggsProcessor::findDecayModeWWqqlv(LCCollection* _mcCol)
{
	if ( _mcCol->getNumberOfElements() < 10 )
		return {0,0} ;

	std::vector<int> part = {} ;
	for ( int i = 2 ; i < 6 ; ++i )
		part.push_back( std::abs( dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( i ) )->getPDG() ) ) ;

	std::sort( part.begin() , part.end() ) ;

	return { part.back()-1 , 0 } ;
}

std::pair<int,int> HiggsProcessor::findDecayModeZZqqqq(LCCollection* _mcCol)
{
	return {0,0} ;
}

std::pair<int,int> HiggsProcessor::findDecayModeZZqqll(LCCollection* _mcCol)
{
	if ( _mcCol->getNumberOfElements() < 10 )
		return {0,0} ;

	std::vector<int> part = {} ;
	for ( int i = 2 ; i < 6 ; ++i )
		part.push_back( std::abs( dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( i ) )->getPDG() ) ) ;

	std::sort( part.begin() , part.end() ) ;

	return { part.back() , 0 } ;
}



std::pair<int, int> HiggsProcessor::findDecayMode(LCCollection* _mcCol)
{
	std::set<int> signal = { 106485 , 106486 , 108015 , 108016 , 108079 , 108080 , 108031 , 108032 , 108047 , 108048 , 108063 , 108064 , 106525 , 106526 } ;

	if ( signal.count(processID) )
		return findDecayModeSignal(_mcCol) ;
	else if ( processID == 106551 || processID == 106552 )
		return findDecayModeWWqqqq(_mcCol) ;
	else if ( processID == 106577 || processID == 106578 )
		return findDecayModeWWqqlv(_mcCol) ;
	else if ( processID == 106573 || processID == 106574 )
		return findDecayModeZZqqqq(_mcCol) ;
	else if ( processID == 106575 || processID == 106576 )
		return findDecayModeZZqqll(_mcCol) ;
	else
		return {0,0} ;
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

double HiggsProcessor::totalNeutrinoEnergy()
{
	double toReturn = 0 ;
	for ( int i = 0 ; i < mcCol->getNumberOfElements() ; ++i )
	{
		MCParticleImpl* mcPart = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt(i) ) ;

		if ( std::abs(mcPart->getPDG()) == 12 || std::abs(mcPart->getPDG()) == 14 || std::abs(mcPart->getPDG()) == 16 )
			toReturn += mcPart->getEnergy() ;
	}
	return toReturn ;
}


void HiggsProcessor::computeOriginMap()
{
	LCRelationNavigator navi(linkCol) ;

	std::map<ReconstructedParticleImpl* , float> weightMap = {} ;

	for ( int i = 0 ; i < recoCol->getNumberOfElements() ; ++i )
	{
		ReconstructedParticleImpl* recoPart = dynamic_cast<ReconstructedParticleImpl*>( recoCol->getElementAt(i) ) ;

		for ( const auto& mc : navi.getRelatedToWeights(recoPart) )
			weightMap[recoPart] += mc ;
	}

	originMap.clear() ;
	for( int i = 0 ; i < linkCol->getNumberOfElements() ; ++i )
	{
		LCRelationImpl* link = dynamic_cast<LCRelationImpl*>( linkCol->getElementAt(i) ) ;
		ReconstructedParticleImpl* recoPart = dynamic_cast<ReconstructedParticleImpl*>( link->getFrom() ) ;
		MCParticleImpl* mcPart = dynamic_cast<MCParticleImpl*>( link->getTo() ) ;

		//		double weight = 0 ;

		//		//Hence: trackwgt = (int(wgt)%10000)/1000. and  clusterwgt = (int(wgt)/10000)/1000.

		//		if ( mcPart->getCharge() == 0 )
		//			weight = ( int( link->getWeight() )/10000)/1000.0 ;
		//		else
		//			weight = ( int( link->getWeight() )%10000)/1000.0 ;

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

		//originMap[recoPart].insert( { mcPartOrigin , recoPart->getEnergy()*link->getWeight() } ) ;
		originMap[recoPart].insert( { mcPartOrigin , recoPart->getEnergy() } ) ;
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


std::map<int,float> HiggsProcessor::mcOriginOfJet(const fastjet::PseudoJet& jet)
{
	auto cs = jet.validated_cluster_sequence() ;
	std::map<int,float> toReturn ;

	std::vector<fastjet::PseudoJet> particlesInJet = cs->constituents(jet) ;

	for( std::vector<fastjet::PseudoJet>::const_iterator it = particlesInJet.begin() ; it != particlesInJet.end() ; ++it )
	{
		std::map<int,float> mcOrigin = mcOriginOfParticle( recoParticles.at( it->user_index() ) ) ;

		for( std::map<int,float>::const_iterator jt = mcOrigin.begin() ; jt != mcOrigin.end() ; ++jt )
			toReturn[jt->first] += jt->second ;
	}

	return toReturn ;
}

Eigen::Matrix3d HiggsProcessor::computeSphericityTensor( const std::vector<fastjet::PseudoJet>& particleVec )
{
	Eigen::Matrix3d tensor ;

	for ( unsigned int i = 0 ; i < 3 ; ++i )
	{
		for ( unsigned int j = 0 ; j < 3 ; ++j )
		{
			double num = 0 ;
			double denom = 0 ;

			for ( const auto& particle : particleVec )
			{
				num += particle.four_mom()[i]*particle.four_mom()[j] ;
				denom += particle.modp2() ;
			}
			tensor(i,j) = num/denom ;
		}
	}

	return tensor ;
}

double HiggsProcessor::computeSphericity( const std::vector<fastjet::PseudoJet>& particleVec )
{
	auto tensor = computeSphericityTensor( particleVec ) ;

	auto eigenVal = tensor.eigenvalues() ;

	std::array<double,3> val = {{ std::norm(eigenVal(0)) , std::norm(eigenVal(1)) , std::norm(eigenVal(2)) }} ;
	std::sort( val.begin() , val.end() ) ;

	//    std::cout << val[0] << " " << val[1] << " " << val[2] << std::endl ;

	return 1.5*(val[0]+val[1]) ;
}

std::array<double,3> HiggsProcessor::computeThrust( const std::vector<fastjet::PseudoJet>& particleVec )
{
	EventShape shape ;
	shape.setPartList(particleVec) ;

	return {{ std::abs( shape.thrustAxis().cosTheta() ) , shape.majorThrust() , shape.minorThrust() }} ;
}


DiJet HiggsProcessor::chooseZDiJet(const std::vector<fastjet::PseudoJet>& jets , std::vector<fastjet::PseudoJet>& remainingJets)
{
	remainingJets.clear() ;

	auto cs = jets.at(0).associated_cluster_sequence() ;

	std::vector<fastjet::PseudoJet> okJets = {} ;

	for (const auto& jet : jets)
	{
		int nb = 0 ;
		std::vector<fastjet::PseudoJet> constituants = cs->constituents(jet) ;
		for ( const auto& part : constituants )
		{
			auto info = dynamic_cast<const ParticleInfo*>( part.user_info_ptr() ) ;

			if ( std::abs( info->recoParticle()->getCharge() ) > std::numeric_limits<float>::epsilon() )
				nb++ ;

		}
		if ( nb > 2 )
			okJets.push_back(jet) ;
	}

	DiJet goodDiJet ;

	if ( okJets.size() < 2 )
		throw std::logic_error("not enough jets") ;

	bool pairFound = false ;
	double chi2 = std::numeric_limits<double>::max() ;

	std::pair<unsigned int , unsigned int> goodPair ;

	for ( unsigned int i = 0 ; i < okJets.size() ; ++i )
	{
		for ( unsigned int j = i+1 ; j < okJets.size() ; ++j )
		{
			DiJet tempDiJet( okJets.at(i) , okJets.at(j) ) ;

			double tempChi2 = ( tempDiJet.diJet().m() - zMassRef )*( tempDiJet.diJet().m() - zMassRef ) ;

			if ( tempDiJet.getCosAngleBetweenJets() > valueAngle )
				tempChi2 += alphaAngle*(tempDiJet.getCosAngleBetweenJets() - valueAngle)*(tempDiJet.getCosAngleBetweenJets() - valueAngle) ;

			if ( tempChi2 < chi2 )
			{
				double pZ_ = tempDiJet.diJet().modp2() ;
				double recMassSq = (sqrtS - tempDiJet.diJet().e() )*(sqrtS - tempDiJet.diJet().e() ) - pZ_ ;

				if ( recMassSq > 0 )
				{
					chi2 = tempChi2 ;
					goodDiJet = tempDiJet ;
					goodPair = {i,j} ;
					pairFound = true ;
				}
			}
		}
	}

	if ( !pairFound )
		throw std::logic_error("no valid Z di-jet") ;

	for ( unsigned int i = 0 ; i < jets.size() ; ++i )
	{
		if ( i == goodPair.first || i == goodPair.second )
			continue ;

		remainingJets.push_back( jets.at(i) ) ;
	}

	return goodDiJet ;
}

DiJet HiggsProcessor::chooseDiJet(const std::vector<fastjet::PseudoJet>& jets , const double& targetMass)
{
	auto nJets_ = jets.size() ;
	assert( nJets_ >= 2 ) ;

	double chi2 = std::numeric_limits<double>::max() ;

	DiJet goodDiJet ;

	for ( unsigned int i = 0 ; i < nJets_ ; ++i )
	{
		for ( unsigned int j = i+1 ; j < nJets_ ; ++j )
		{
			DiJet diJet( jets[i] , jets[j] ) ;
			double tempChi2 = (diJet.diJet().m() - targetMass)*(diJet.diJet().m() - targetMass) ;

			if ( tempChi2 < chi2 )
			{
				chi2 = tempChi2 ;
				goodDiJet = diJet ;
			}
		}
	}

	return goodDiJet ;
}

std::pair<DiJet,DiJet> HiggsProcessor::choosePairDiJets(const std::vector<fastjet::PseudoJet>& jets , const double& targetMass)
{
	if ( jets.size() != 4 )
		throw std::logic_error("Error in HiggsProcessor::choosePairDiJets : not 4 jets") ;

	std::vector<unsigned int> okJets = {0,1,2,3} ;

	double chi2 = std::numeric_limits<double>::max() ;

	std::vector<unsigned int> goodPair = {} ;

	for ( unsigned int i = 0 ; i < okJets.size() ; ++i )
	{
		for ( unsigned int j = i+1 ; j < okJets.size() ; ++j )
		{
			DiJet tempDiJet( jets.at(i) , jets.at(j) ) ;

			double tempChi2 = ( tempDiJet.diJet().m() - targetMass )*( tempDiJet.diJet().m() - targetMass ) ;

			if ( tempChi2 < chi2 )
			{
				chi2 = tempChi2 ;
				goodPair = {i,j} ;
			}
		}
	}

	std::vector<unsigned int> other = {} ;

	for ( unsigned int i = 0 ; i < 4 ; ++i )
	{
		if ( !std::count(goodPair.begin() , goodPair.end() , i) )
			other.push_back(i) ;
	}

	return { DiJet( jets.at(goodPair[0]) , jets.at(goodPair[1]) ) , DiJet( jets.at(other[0]) , jets.at(other[1]) ) } ;
}

std::pair<DiJet,DiJet> HiggsProcessor::choosePairOfZDiJets(const std::vector<fastjet::PseudoJet>& jets)
{
	return choosePairDiJets(jets , zMassRef) ;
}

std::pair<DiJet,DiJet> HiggsProcessor::choosePairOfWDiJets(const std::vector<fastjet::PseudoJet>& jets)
{
	return choosePairDiJets(jets , wMassRef) ;
}


void HiggsProcessor::processEvent(LCEvent* evt)
{
	cleanEvent() ;
	currentEvt = evt ;

	runNumber = evt->getRunNumber() ;
	evtNumber = evt->getEventNumber() ;

	mcCol = currentEvt->getCollection( mcPartColName ) ;
	linkCol = currentEvt->getCollection( linkColName ) ;
	recoCol = currentEvt->getCollection( recoPartColName ) ;

	computeOriginMap() ;

	neutrinoEnergy = totalNeutrinoEnergy() ;

	auto isoLepCol = currentEvt->getCollection( isoLepColName ) ;

	nIsoLep = 0 ;
	for ( int i = 0 ; i < isoLepCol->getNumberOfElements() ; ++i )
	{
		auto recoPart = dynamic_cast<ReconstructedParticleImpl*>( isoLepCol->getElementAt(i) ) ;
		if ( recoPart->getEnergy() > 10 )
			nIsoLep++ ;
	}

	//get all the reconstructed particles
	for ( int i = 0 ; i < recoCol->getNumberOfElements() ; ++i )
	{
		ParticleInfo* partInfo = new ParticleInfo ;
		ReconstructedParticleImpl* recoPart = dynamic_cast<ReconstructedParticleImpl*>( recoCol->getElementAt(i) ) ;
		partInfo->setRecoParticle( recoPart ) ;
		partInfo->setId(i) ;

		if ( recoPart->getType() == 22 )
		{
			if ( recoPart->getEnergy() > 80 )
				bigPhoton = true ;
		}

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

	sphericity = computeSphericity(particles) ;

	auto thrust = computeThrust(particles) ;
	cosThrust = thrust[0] ;
	majorThrust = thrust[1] ;
	minorThrust = thrust[2] ;

	totalEnergy = 0 ;
	double totalZEnergy = 0.0 ;

	pMiss = {0,0,0} ;

	for ( const auto particle : particles )
	{
		totalEnergy += particle.E() ;

		pMiss[0] += particle.px() ;
		pMiss[1] += particle.py() ;
		pMiss[2] += particle.pz() ;

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

	CLHEP::Hep3Vector pMissVec(pMiss[0],  pMiss[1] , pMiss[2]) ;
	cosThetaMiss = pMissVec.cosTheta() ;


	pMissNorm = 0 ;
	for ( const auto& i : pMiss )
		pMissNorm += i*i ;

	pMissNorm = std::sqrt(pMissNorm) ;


	auto decay = findDecayMode( evt->getCollection(mcPartColName) ) ;
	decayID = decay.first ;
	subDecayID = decay.second ;

	ISR isr = processISR() ;

	ISREnergy = 0.0 ;
	for ( const auto& i : isr.mcFourVec )
		ISREnergy += i.e() ;

	for ( auto& jetVec : fixedJets )
		jetVec.clear() ;

	//jet clustering
	fastjet::ClusterSequence csFixed(particles , jetDef) ;

	for ( unsigned int i = 1 ; i < 7 ; ++i )
	{
		if ( particles.size() < i )
			break ;
		fixedJets[i] = csFixed.exclusive_jets(static_cast<int>(i)) ;
	}

	y23 = -std::log10( csFixed.exclusive_ymerge(2) ) ;
	y34 = -std::log10( csFixed.exclusive_ymerge(3) ) ;


	if ( fixedJets[4].size() != 4 )
	{
		std::cout << "failed at 1" << std::endl ;
		failedStep = 1 ;
		tree->Fill() ;
		return ;
	}

	//WW and ZZ pair (background study)
	std::pair<DiJet,DiJet> wwPair = {} ;
	std::pair<DiJet,DiJet> zzPair = {} ;
	try
	{
		wwPair = choosePairOfWDiJets(fixedJets[4]) ;
		zzPair = choosePairOfZDiJets(fixedJets[4]) ;
	}
	catch( std::logic_error &e )
	{
		std::cout << "failed at 2" << std::endl ;
		failedStep = 2 ;
		tree->Fill() ;
		return ;
	}

	ww12mass = wwPair.first.diJet().m() ;
	ww34mass = wwPair.second.diJet().m() ;

	zz12mass = zzPair.first.diJet().m() ;
	zz34mass = zzPair.second.diJet().m() ;

	w12pt = wwPair.first.diJet().pt() ;
	z12pt = zzPair.first.diJet().pt() ;

	//WW->qqlv
	auto qqlvDiJet = chooseDiJet(fixedJets[3] , wMassRef) ;
	wwMass3 = qqlvDiJet.diJet().m() ;
	wwRecMass3 = std::sqrt( (sqrtS - qqlvDiJet.diJet().E())*(sqrtS - qqlvDiJet.diJet().E()) - qqlvDiJet.diJet().modp2() ) ;

	w3pt = qqlvDiJet.diJet().pt() ;

	//2jet reconstruction
	auto jets2 = fixedJets[2] ;
	DiJet dijet2 = DiJet( jets2[0] , jets2[1] ) ;
	mass2Jet = dijet2.diJet().m() ;
	cosBetw2Jet =  dijet2.jet1().pz() / dijet2.jet1().modp() ;


	//nJets ZH clustering
	validVec = std::vector<bool>(7 , true) ;
	zMassVec = std::vector<double>(7 , -1.0) ;
	recMassVec = std::vector<double>(7 , -1.0) ;
	cosThetaZVec = std::vector<double>(7 , 0) ;
	zPtVec = std::vector<double>(7 , 0) ;

	for ( unsigned int i = 2 ; i <= 6 ; ++i )
	{
		std::vector<fastjet::PseudoJet> remainingJets ;

		double& _zMass = zMassVec[i] ;
		double& _recMass = recMassVec[i] ;
		DiJet zDiJet ;
		try
		{
			zDiJet = chooseZDiJet(fixedJets[i] , remainingJets) ;
		}
		catch( std::logic_error &e )
		{
			validVec[i] = false ;
			continue ;
		}

		_zMass = zDiJet.diJet().m() ;
		double pZ = zDiJet.diJet().modp2() ;
		double recMassSq = (sqrtS - zDiJet.diJet().e() )*(sqrtS - zDiJet.diJet().e() ) - pZ ;

		if ( recMassSq < 0 )
		{
			validVec[i] = false ;
			continue ;
		}

		_recMass = std::sqrt( recMassSq ) ;
		zPtVec[i] = zDiJet.diJet().pt() ;
		cosThetaZVec[i] = zDiJet.diJet().pz() / zDiJet.diJet().modp() ;
	}




	//main ZH clustering
	fastjet::JetDefinition jDZH(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csZH(particles , jDZH) ;

	std::vector<fastjet::PseudoJet> jets = {} ;
	if ( yCut <= std::numeric_limits<double>::epsilon() )
		jets = sorted_by_pt( csZH.exclusive_jets(4) ) ;
	else
		jets = sorted_by_pt( csZH.exclusive_jets_ycut(yCut) ) ;

	if ( ! (jets.size() > 1) )
		jets = sorted_by_pt( csZH.exclusive_jets(2) ) ;

	if ( jets.size() > 6 )
		jets = sorted_by_pt( csZH.exclusive_jets(6) ) ;

	for ( auto& jet : jets )
	{
		JetInfo* info = new JetInfo ;
		info->setMcOrigin( mcOriginOfJet(jet) ) ;
		jet.set_user_info(info) ;
	}


	assert ( jets.size() > 1 ) ;

	totalEnergyJets = 0.0 ;
	for ( const auto& jet : jets )
		totalEnergyJets += jet.e() ;


	nJets = jets.size() ;

	std::vector<fastjet::PseudoJet> remainingJets ;

	zMass = 0 ;
	recMass = 0 ;
	DiJet zDiJet ;
	try
	{
		zDiJet = chooseZDiJet(jets , remainingJets) ;
	}
	catch( std::logic_error &e )
	{
		std::cout << "failed at 3" << std::endl ;
		failedStep = 3 ;
		tree->Fill() ;
		return ;
	}

	std::vector<fastjet::PseudoJet> remainingParticles ;
	for ( const auto& jet : remainingJets )
	{
		for ( const auto& particle : jet.constituents() )
			remainingParticles.push_back(particle) ;
	}


	zMass = zDiJet.diJet().m() ;
	double pZ = zDiJet.diJet().modp2() ;

	zPt = zDiJet.diJet().pt() ;

	double recMassSq = (sqrtS - zDiJet.diJet().e() )*(sqrtS - zDiJet.diJet().e() ) - pZ ;
	recMass = std::sqrt( recMassSq ) ;


	int targetNJetsH = std::min(static_cast<int>(remainingParticles.size()) , 2 ) ;

	fastjet::JetDefinition jDH(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence csH(remainingParticles , jDH) ;
	auto hJets = sorted_by_pt( csH.exclusive_jets(targetNJetsH) ) ;

	cosThetaZ = zDiJet.diJet().pz() / zDiJet.diJet().modp() ;


	cosThetaZ12 = std::acos(zDiJet.getCosAngleBetweenJets()) ;

	z1e = zDiJet.jet1().e() ;
	z2e = zDiJet.jet2().e() ;

	if ( hJets.size() == 2 )
	{
		auto hDiJet = DiJet(hJets.at(0) , hJets.at(1)) ;
		cosThetaH12 = hDiJet.getCosAngleBetweenJets() ;
		h1e = hDiJet.jet1().e() ;
		h2e = hDiJet.jet2().e() ;
	}
	else if ( hJets.size() == 1 )
	{
		cosThetaH12 = 1.0 ;
		h1e = hJets.at(0).e() ;
		h2e = 0.0 ;
	}
	else
	{
		cosThetaH12 = 0.0 ;
		h1e = 0.0 ;
		h2e = 0.0 ;
	}

	std::vector<double> totalEnergyJetsVec(nJets , 0.0) ;

	double zTaggedEnergy = 0 ;
	double hTaggedEnergy = 0 ;

	for ( const auto& origin : zDiJet.jet1().user_info<JetInfo>().mcOrigin() )
	{
		if ( std::abs(origin.first) <= 6 )
		{
			zTaggedEnergy += origin.second ;
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
		}
		if ( std::abs(origin.first) == 25 )
			hTaggedEnergy += origin.second ;

		totalEnergyJetsVec.at(1) += origin.second ;
	}

	for ( unsigned int i = 0 ; i < remainingJets.size() ; ++i )
	{
		for ( const auto& origin : remainingJets.at(i).user_info<JetInfo>().mcOrigin() )
		{
			totalEnergyJetsVec.at(2+i) += origin.second ;
		}
	}


	zTagged = zTaggedEnergy/totalZEnergy ;
	zPurity = zTaggedEnergy/(zDiJet.jet1().e() + zDiJet.jet2().e()) ;

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
