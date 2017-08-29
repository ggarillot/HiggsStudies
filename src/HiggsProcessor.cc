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

HiggsProcessor::HiggsProcessor()
	: Processor("HiggsProcessor") ,
	  mcPartColName() ,
	  recoPartColName() ,
	  linkColName() ,
	  recoParticles() ,
	  particles() ,
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

	tree->Branch("decayID" , &decayID) ;
	tree->Branch("totalEnergy" , &totalEnergy) ;
	tree->Branch("energy4Jets" , &energy4Jets) ;
	tree->Branch("ISREnergy" , &ISREnergy) ;
	tree->Branch("yValueVec" , &yValueVec) ;
	tree->Branch("zMass" , &zMass) ;
	tree->Branch("recMass" , &recMass) ;
	tree->Branch("recMass2" , &recMass2) ;
	tree->Branch("hDiJetMass" , &hDiJetMass) ;
	tree->Branch("zEnergy" , &zEnergy) ;
	tree->Branch("cosAngleBetweenZJets" , &cosAngleBetweenZJets) ;
	tree->Branch("cosAngleBetweenHJets" , &cosAngleBetweenHJets) ;
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

	tree->Branch("idealRecoZmass" , &idealRecoZmass) ;
	tree->Branch("idealRecoHmass" , &idealRecoHmass) ;
	tree->Branch("idealRecoRecMass" , &idealRecoRecMass) ;


	tree->Branch("zMassMC" , &zMassMC) ;
	tree->Branch("recMassMC" , &recMassMC) ;
	tree->Branch("zEnergyMC" , &zEnergyMC) ;

	tree->Branch("cosAngleBetweenZJetsMC" , &cosAngleBetweenZJetsMC) ;
	tree->Branch("cosAngleBetweenHJetsMC" , &cosAngleBetweenHJetsMC) ;
	tree->Branch("zPhiMC" , &zPhiMC) ;
	tree->Branch("zRapMC" , &zRapMC) ;
	tree->Branch("hPhiMC" , &hPhiMC) ;
	tree->Branch("hRapMC" , &hRapMC) ;

	tree->Branch("deltaP" , &deltaP) ;
	tree->Branch("pX" , &pX) ;
	tree->Branch("pY" , &pY) ;
	tree->Branch("pZ" , &pZ) ;

	_nEvt = 0 ;
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

double HiggsProcessor::getISREnergy(LCCollection* _mcCol)
{
	double toReturn = 0.0 ;
	if ( _mcCol->getNumberOfElements() < 2 )
		return toReturn ;

	toReturn += dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 0 ) )->getEnergy() ;
	toReturn += dynamic_cast<MCParticleImpl*>( _mcCol->getElementAt( 1 ) )->getEnergy() ;
	return toReturn ;
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


	//	const double* p1d = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 10 ) )->getMomentum() ;

	//	CLHEP::Hep3Vector p( p1d[0] , p1d[1] , p1d[2] ) ;


	//	double e = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 10 ) )->getEnergy()  ;

	//	int id = dynamic_cast<MCParticleImpl*>( mcCol->getElementAt( 10 ) )->getPDG() ;
	//	std::cout << id << std::endl ;
	//	if ( id !=94 )
	//		throw ;


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

	//	std::cout << jet.e() << std::endl ;
	//	for( std::map<int,float>::const_iterator jt = toReturn.begin() ; jt != toReturn.end() ; ++jt )
	//		std::cout << jt->first << " " << jt->second << std::endl ;

	return toReturn ;
}


void HiggsProcessor::processRunHeader(LCRunHeader* )
{
	_nRun++ ;
	_nEvt = 0 ;
}


void HiggsProcessor::processEvent(LCEvent* evt)
{
	currentEvt = evt ;
	mcCol = currentEvt->getCollection( mcPartColName ) ;
	linkCol = currentEvt->getCollection( linkColName ) ;
	recoCol = currentEvt->getCollection( recoPartColName ) ;

	computeOriginMap() ;

	particles.clear() ;
	recoParticles.clear() ;
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

	std::vector<fastjet::PseudoJet> zParticles ;
	std::vector<fastjet::PseudoJet> hParticles ;

	totalEnergy = 0 ;

	for ( std::vector<fastjet::PseudoJet>::const_iterator it = particles.begin() ; it != particles.end() ; ++it )
	{
		totalEnergy += it->E() ;
		if ( std::abs( it->user_info<ParticleInfo>().origin() ) <= 6 )
			zParticles.push_back(*it) ;

		if ( it->user_info<ParticleInfo>().origin() == 25 )
			hParticles.push_back(*it) ;
	}


	//	double rZ = 3.14159265/2.0 ;
	double rZ = 6 ;

	fastjet::JetDefinition jetDefZ(fastjet::kt_algorithm , rZ) ;
	fastjet::ClusterSequence csZ(zParticles, jetDefZ) ;

	int targetNjetsZ = zParticles.size() ;
	if ( targetNjetsZ > 2 )
		targetNjetsZ = 2 ;

	std::vector<fastjet::PseudoJet> jetsZ ;
	std::vector<fastjet::PseudoJet> jetsH ;

	fastjet::PseudoJet zAll ;
	for ( std::vector<fastjet::PseudoJet>::const_iterator it = zParticles.begin() ; it != zParticles.end() ; ++it )
		zAll += *it ;
	jetsZ.push_back(zAll) ;


	//	jetsZ = sorted_by_pt( csZ.exclusive_jets(2) ) ;
	//	jetsZ = sorted_by_pt( csZ.inclusive_jets() ) ;


	fastjet::JetDefinition jetDefH(fastjet::kt_algorithm , 6) ;
	fastjet::ClusterSequence csH(hParticles, jetDefH) ;



	fastjet::PseudoJet zAllH ;

	for ( std::vector<fastjet::PseudoJet>::const_iterator it = hParticles.begin() ; it != hParticles.end() ; ++it )
		zAllH += *it ;
	jetsH.push_back(zAllH) ;

	jetsH = sorted_by_pt( csH.exclusive_jets_up_to(2) ) ;

	if ( jetsH.size() < 2 )
		cosAngleBetweenHJetsMC = 1 ;
	else
	{
		CLHEP::Hep3Vector p1( jetsH.at(0).px() , jetsH.at(0).py() , jetsH.at(0).pz() ) ;
		CLHEP::Hep3Vector p2( jetsH.at(1).px() , jetsH.at(1).py() , jetsH.at(1).pz() ) ;

		cosAngleBetweenHJetsMC = ( p1.dot(p2) )/(p1.mag()*p2.mag()) ;
	}



	fastjet::PseudoJet idealZDiJet = fastjet::PseudoJet(0,0,0,0) ;
	fastjet::PseudoJet idealHDiJet = fastjet::PseudoJet(0,0,0,0) ;

	for ( unsigned int i = 0 ; i < jetsZ.size() ; ++i )
		idealZDiJet += jetsZ.at(i) ;

	for ( unsigned int i = 0 ; i < jetsH.size() ; ++i )
		idealHDiJet += jetsH.at(i) ;

	zPhiMC = idealZDiJet.phi() ;
	zRapMC = idealZDiJet.pseudorapidity() ;

	hPhiMC = idealHDiJet.phi() ;
	hRapMC = idealHDiJet.pseudorapidity() ;

	/*
	double etotz = 0 ;
	for ( std::vector<fastjet::PseudoJet>::const_iterator it = zParticles.begin() ; it != zParticles.end() ; ++it )
		etotz += it->E() ;

	std::cout << "energy tot Z : " << etotz << std::endl ;
	std::cout << "energy jet Z : " << idealZDiJet.E() << std::endl ;

	std::cout << "nParticlesTOT : " << zParticles.size() << std::endl ;
	std::cout << "nParticlesInCS : " << csZ.n_particles() << std::endl ;



	std::vector<fastjet::PseudoJet> zParticlesInJet ;
	for ( unsigned int i = 0 ; i < jetsZ.size() ; ++i )
	{
		std::vector<fastjet::PseudoJet> jet = csZ.constituents( jetsZ.at(i) ) ;
		zParticlesInJet.insert( zParticlesInJet.end() , jet.begin() , jet.end() ) ;
	}

	std::cout << "nParticleJet : " << zParticlesInJet.size() << std::endl ;
	std::cout << "nParticlesNotInJet : " << csZ.unclustered_particles().size() << std::endl ;

	std::cout << "zParticles : " << std::endl ;
	for( std::vector<fastjet::PseudoJet>::const_iterator it = zParticles.begin() ; it != zParticles.end() ; ++it )
	{
		bool found = false ;
		int id = it->user_index() ;

		for( std::vector<fastjet::PseudoJet>::const_iterator jt = zParticlesInJet.begin() ; jt != zParticlesInJet.end() ; ++jt )
		{
			if ( jt->user_index() == id )
			{
				found = true ;
				break ;
			}
		}

		if ( found )
			std::cout << "\e[1m" ;

		std::cout << it->user_index() << " " << it->E() << " (" << it->px() << " , " << it->py() << " , " << it->pz() << ")"  ;

		if ( found )
			std::cout << "\e[0m" ;

		std::cout << std::endl ;
	}
*/

	zEnergyMC = getZEnergyMC() ;
	cosAngleBetweenZJetsMC = getCosAngleBetweenZJetsMC() ;


	double pt = 2*std::sqrt(sqrtS*sqrtS/4.0 - 511e-6*511e-6)*std::sin(7e-3) ;

	CLHEP::Hep3Vector totP(pt,0,0) ;


	idealRecoZmass = idealZDiJet.m() ;
	idealRecoHmass = idealHDiJet.m() ;
	CLHEP::Hep3Vector idealPZrec( idealZDiJet.px() , idealZDiJet.py() , idealZDiJet.pz() ) ;

	idealRecoRecMass = std::sqrt( (sqrtS - idealZDiJet.e())*(sqrtS - idealZDiJet.e()) - ( idealPZrec.mag2() + totP.mag2() -2*idealPZrec.dot(totP) ) ) ;


	using fastjet::PseudoJet ;
	std::vector<int> vec = findDecayMode( evt->getCollection(mcPartColName) ) ;

	//	std::cout << std::endl ;
	//	for ( unsigned int i = 0 ; i < vec.size() ; ++i )
	//		std::cout << vec.at(i) << " " ;
	//	std::cout << std::endl ;

	ISREnergy = getISREnergy( evt->getCollection(mcPartColName) ) ;

	CLHEP::HepLorentzVector zVec = getZLorentzVectorMC( evt->getCollection(mcPartColName) ) ;
	CLHEP::HepLorentzVector hVec = getHLorentzVectorMC( evt->getCollection(mcPartColName) ) ;

	CLHEP::Hep3Vector zP = zVec.getV() ;
	CLHEP::Hep3Vector zH = hVec.getV() ;



	zMassMC = zVec.m() ;

	recMassMC = std::sqrt( (sqrtS - zVec.e())*(sqrtS - zVec.e()) - ( zP.mag2() + totP.mag2() -2*zP.dot(totP) ) ) ;


	deltaP = hVec.perp2() + hVec.py()*hVec.py() - zVec.perp2() - zVec.py()*zVec.py() ;

	pX = hVec.px() + zVec.px() ;
	pY = hVec.py() + zVec.py() ;
	pZ = hVec.pz() + zVec.pz() ;





	//	double R = 3.14159265/2.0;
	double R = M_PI/2.0 ;
	//	double R = 0.7 ;
	fastjet::JetDefinition jet_def(fastjet::kt_algorithm , R) ;
	//	fastjet::JetDefinition jet_def(fastjet::antikt_algorithm , R) ;
	//	fastjet::JetDefinition jet_def(fastjet::ee_kt_algorithm) ;
	fastjet::ClusterSequence cs(particles, jet_def) ;

	//	std::cout << cs.inclusive_jets().size() << " jets" << std::endl ;
	//	std::cout << "decay : " << vec.at(0) << std::endl ;
	std::vector<PseudoJet> jets = sorted_by_pt( cs.exclusive_jets(4) ) ;
	std::vector<PseudoJet> inclusiveJets = sorted_by_E( cs.exclusive_jets(0.006) ) ;


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


	std::vector< std::pair<DiJet,DiJet> > diJets ;
	diJets.push_back( std::make_pair( DiJet(jets.at(0) , jets.at(1)) , DiJet(jets.at(2) , jets.at(3)) ) ) ;
	diJets.push_back( std::make_pair( DiJet(jets.at(0) , jets.at(2)) , DiJet(jets.at(1) , jets.at(3)) ) ) ;
	diJets.push_back( std::make_pair( DiJet(jets.at(0) , jets.at(3)) , DiJet(jets.at(1) , jets.at(2)) ) ) ;
	diJets.push_back( std::make_pair( DiJet(jets.at(2) , jets.at(3)) , DiJet(jets.at(0) , jets.at(1)) ) ) ;
	diJets.push_back( std::make_pair( DiJet(jets.at(1) , jets.at(3)) , DiJet(jets.at(0) , jets.at(2)) ) ) ;
	diJets.push_back( std::make_pair( DiJet(jets.at(1) , jets.at(2)) , DiJet(jets.at(0) , jets.at(3)) ) ) ;


	double chi2ZH = std::numeric_limits<double>::max() ;
	double chi2ZZ = std::numeric_limits<double>::max() ;
	double chi2WW = std::numeric_limits<double>::max() ;

	std::vector< std::pair<DiJet,DiJet> >::iterator goodPairItZH = diJets.end() ;
	std::vector< std::pair<DiJet,DiJet> >::iterator goodPairItZZ = diJets.end() ;
	std::vector< std::pair<DiJet,DiJet> >::iterator goodPairItWW = diJets.end() ;

	for ( std::vector< std::pair<DiJet,DiJet> >::iterator it = diJets.begin() ; it != diJets.end() ; ++it )
	{
		double tempchi2WW = ( it->first.diJet().m()*1.04 - 80.39 )*( it->first.diJet().m()*1.04 - 80.39 ) + ( it->second.diJet().m()*1.04 - 80.39 )*( it->second.diJet().m()*1.04 - 80.39 ) ;
		double tempchi2ZZ = ( it->first.diJet().m()*1.04 - 91.2 )*( it->first.diJet().m()*1.04 - 91.2 ) + ( it->second.diJet().m()*1.04 - 91.2 )*( it->second.diJet().m()*1.04 - 91.2 ) ;
		//		double tempchi2ZH = ( it->first.diJet().m()*1.04 - 91.2 )*( it->first.diJet().m()*1.04 - 91.2 ) ;
		double tempchi2ZH = ( it->first.diJet().m() - 91.2 )*( it->first.diJet().m() - 91.2 ) ;

		if ( tempchi2ZH < chi2ZH )
		{
			chi2ZH = tempchi2ZH ;
			goodPairItZH = it ;
		}
		if ( tempchi2ZZ < chi2ZZ )
		{
			chi2ZZ = tempchi2ZZ ;
			goodPairItZZ = it ;
		}
		if ( tempchi2WW < chi2WW )
		{
			chi2WW = tempchi2WW ;
			goodPairItWW = it ;
		}
	}


	double chi2Z = std::numeric_limits<double>::max() ;
	DiJet zDiJet ;

	for ( unsigned int i = 0 ; i < inclusiveJets.size() ; ++i )
	{
		for ( unsigned int j = i+1 ; j < inclusiveJets.size() ; ++j )
		{
			DiJet diJet( inclusiveJets.at(i) , inclusiveJets.at(j) ) ;

			double tempchi2Z = ( diJet.diJet().m()*1.04 - 91.2 )*( diJet.diJet().m()*1.04 - 91.2 ) ;

			if ( tempchi2Z < chi2Z )
			{
				chi2Z = tempchi2Z ;
				zDiJet = diJet ;
			}

		}
	}


	ww12mass = goodPairItWW->first.diJet().m() ;
	ww34mass = goodPairItWW->second.diJet().m() ;

	zz12mass = goodPairItZZ->first.diJet().m() ;
	zz34mass = goodPairItZZ->second.diJet().m() ;

	const DiJet& qqDiJet = goodPairItZH->first ;
	const DiJet& hDiJet = goodPairItZH->second ;


	double zTotEnergy = 0 ;
	double hTotEnergy = 0 ;

	for( unsigned int i = 0 ; i < jets.size() ; ++i )
	{
		std::map<int,float> yuyu = mcOriginOfJet(jets.at(i) , cs) ;

		for ( std::map<int,float>::const_iterator it = yuyu.begin() ; it != yuyu.end() ; ++it )
		{
			if ( std::abs(it->first) <= 6 )
				zTotEnergy += it->second ;
			if ( std::abs(it->first) == 25 )
				hTotEnergy += it->second ;
		}
	}

	double zTaggedEnergy = 0 ;
	double hTaggedEnergy = 0 ;

	std::map<int,float> yuyu = mcOriginOfJet(qqDiJet.jet1() , cs) ;
	for ( std::map<int,float>::const_iterator it = yuyu.begin() ; it != yuyu.end() ; ++it )
	{
		if ( std::abs(it->first) <= 6 )
			zTaggedEnergy += it->second ;
		if ( std::abs(it->first) == 25 )
			hTaggedEnergy += it->second ;
	}

	yuyu = mcOriginOfJet(qqDiJet.jet2() , cs) ;
	for ( std::map<int,float>::const_iterator it = yuyu.begin() ; it != yuyu.end() ; ++it )
	{
		if ( std::abs(it->first) <= 6 )
			zTaggedEnergy += it->second ;
		if ( std::abs(it->first) == 25 )
			hTaggedEnergy += it->second ;
	}

	cosAngleBetweenZJets = qqDiJet.getCosAngleBetweenJets() ;
	cosAngleBetweenHJets = hDiJet.getCosAngleBetweenJets() ;

	zEnergy = qqDiJet.diJet().E() ;

	zPurity = zTaggedEnergy/zTotEnergy ;
	hCont = hTaggedEnergy/hTotEnergy ;

	zPhi = qqDiJet.diJet().phi() ;
	zRap = qqDiJet.diJet().pseudorapidity() ;

	hPhi = hDiJet.diJet().phi() ;
	hRap = hDiJet.diJet().pseudorapidity() ;



	double pqq = qqDiJet.diJet().pt()*qqDiJet.diJet().pt() + qqDiJet.diJet().pz()*qqDiJet.diJet().pz() ;

	CLHEP::Hep3Vector pZrec( qqDiJet.diJet().px() , qqDiJet.diJet().py() , qqDiJet.diJet().pz() ) ;


	double mrec = (sqrtS - qqDiJet.diJet().e() )*(sqrtS - qqDiJet.diJet().e() ) - pqq ;


	//	pZrec *= 1.04 ;

	zMass = qqDiJet.diJet().m() ;

	double a = 91.2/zMass ;

	//	recMass2 = std::sqrt( (sqrtS - qqDiJet.diJet().e())*(sqrtS - qqDiJet.diJet().e()) - ( pZrec.mag2() + totP.mag2() -2*pZrec.dot(totP) ) ) ;
	recMass2 = std::sqrt( (sqrtS - qqDiJet.diJet().e()*a)*(sqrtS - qqDiJet.diJet().e()*a) - ( pZrec.mag2()*a*a + totP.mag2() - 2*pZrec.dot(totP)*a ) ) ;

	hDiJetMass = hDiJet.diJet().m() ;



	//	zMass = zDiJet.diJet().m() ;
	//	pZrec = CLHEP::Hep3Vector( zDiJet.diJet().px() , zDiJet.diJet().py() , zDiJet.diJet().pz() ) ;

	//	recMass2 = std::sqrt( (sqrtS - zDiJet.diJet().e())*(sqrtS - zDiJet.diJet().e()) - ( pZrec.mag2() + totP.mag2() -2*pZrec.dot(totP) ) ) ;

	recMass = std::sqrt(mrec) ;

	//	for ( unsigned int i = 0 ; i < vec.size() ; ++i )
	//		std::cout << vec.at(i) << " " ;

	//	std::cout << std::endl ;

	decayID = vec.at(0) ;
	tree->Fill() ;


	std::cout << "Event " << evt->getEventNumber() << std::endl ;

	_nEvt++ ;

}


void HiggsProcessor::check(LCEvent* )
{
}


void HiggsProcessor::end()
{
	tree->Write() ;
	file->Close() ;
}
