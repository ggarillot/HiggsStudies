#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "Event.h"
#include "Cuts.h"
#include "Process.h"


int getCategory(const Event& event)
{
	int category = 0 ;

	if ( event.nJets <= 2 )
		category += 100 ;
	else if ( event.nJets == 3 )
		category += 200 ;
	else if ( event.nJets == 4 )
		category += 300 ;
	else
		category += 400 ;

	if ( event.totalEnergy < 110 )
		category += 10 ;
	else if ( event.totalEnergy >= 110 && event.totalEnergy < 180 )
		category += 20 ;
	else
		category += 30 ;

	if ( event.nIsoLep == 0 )
		category += 1 ;
	else if ( event.nIsoLep == 1 )
		category += 2 ;
	else
		category += 3 ;

	return category ;
}

int main ( int argc , char** argv )
{
	if ( argc == 1 )
	{
		std::cout << "ERROR : no pol provided " << std::endl ;
		return 1 ;
	}

	double ePol = 0 ;
	double pPol = 0 ;

	if ( argv[1] == std::string("RL") )
	{
		ePol = 0.8 ;
		pPol = -0.3 ;
	}
	else if ( argv[1] == std::string("LR") )
	{
		ePol = -0.8 ;
		pPol = +0.3 ;
	}
	else
	{
		std::cout << "invalid pol" << std::endl ;
		return 1 ;
	}

	constexpr double lumi = 500 ;

	double eR = 0.5*(ePol+1) ;
	double eL = 0.5*(1-ePol) ;

	double pR = 0.5*(pPol+1) ;
	double pL = 0.5*(1-pPol) ;

	std::map<Polarisation , double> polWeightMap = {} ;
	polWeightMap.insert( { LR , eL*pR } ) ;
	polWeightMap.insert( { RL , eR*pL } ) ;
	polWeightMap.insert( { LL , eL*pL } ) ;
	polWeightMap.insert( { RR , eR*pR } ) ;

	std::cout << "Luminosity : " << lumi << std::endl ;
	std::cout << "Polarisation : (" << 100*ePol << "% , " << 100*pPol << "%)" << std::endl ;

	std::cout << "weightLR : " << polWeightMap.at(LR) << std::endl ;
	std::cout << "weightRL : " << polWeightMap.at(RL) << std::endl ;
	std::cout << "weightLL : " << polWeightMap.at(LL) << std::endl ;
	std::cout << "weightRR : " << polWeightMap.at(RR) << std::endl ;

	std::vector<int> processIDVec = {106485,106551,106577,106573,106575,106607,106486,106552,106578,106574,106576,106608} ;

	TFile* outputFile = new TFile("Categorized.root" , "RECREATE") ;
	TTree* outputTree = new TTree("tree" , "tree") ;

	std::string dir = argv[2] ;

	std::map<int , std::map<std::string , Process>> processMap = {} ;

	EventReader eventReader ;

	for ( const int& processID : processIDVec )
	{
		std::cout << "Processing " << processID << "..." << std::endl ;
		std::stringstream subDir ;
		subDir << dir << "/" << processID ;

		std::stringstream fileName ;
		fileName << dir << "/" << processID << ".root" ;
		TFile* file = TFile::Open( fileName.str().c_str() ) ;
		if ( !file )
		{
			std::cerr << "ERROR : could not open data file" << std::endl ;
			return 1 ;
		}
		TTree* tree = dynamic_cast<TTree*>( file->Get("tree") ) ;
		if ( !tree )
		{
			std::cerr << "ERROR : tree not present" << std::endl ;
			return 1 ;
		}

		eventReader.setTree(tree) ;
		eventReader.setTreeWrite(outputTree) ;
		int iEntry = 0 ;
		while( tree->GetEntry( iEntry++ ) )
		{
			Event event = eventReader.getEvent(iEntry-1) ;

			event.processID = processID ;
			eventReader.processID = processID ;

			int category = getCategory(event) ;

			processMap[ category ][ Process::getProcess(event) ].nTotal[ ChannelInfoMap.at(processID).pol ] ++ ;

			if ( event.processID == 106485 || event.processID == 106486 )
				if ( event.decayID == 23 || event.decayID == 24 )
					processMap[ category ][ Process::getSubProcess(event) ].nTotal[ ChannelInfoMap.at(processID).pol ] ++ ;


			eventReader.category = category ;

			outputTree->Fill() ;
		}

		file->Close() ;
	}

	outputFile->cd() ;
	outputTree->Write() ;
	outputFile->Close() ;


	return 0 ;
}
