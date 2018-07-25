#include <iostream>
#include <set>
#include <sstream>

#include <TFile.h>
#include <TTree.h>
#include <TEfficiency.h>

#include "Event.h"
#include "Cuts.h"
#include "Process.h"

std::array<double , 2> getEfficienciesBound(int passed , int total)
{
	constexpr double level = 0.683 ;

	double a = double( passed ) + 1 ;
	double b = double( total - passed ) + 1 ;

	double lowerBound = 0 ;
	double upperBound = 0 ;

	TEfficiency::BetaShortestInterval( level , a , b , lowerBound , upperBound ) ;

	std::array<double , 2> toReturn = {{ lowerBound , upperBound }} ;
	return toReturn ;
}

//void cat(const Event& event , std::map<std::string , std::array<int,7>>& map , bool sub = false)
//{
//	auto process = Process::getProcess(event) ;

//	if ( sub )
//		process = Process::getSubProcess(event) ;

//	map[ process ][0] ++ ;

//	if ( event.failedStep != 0 )
//		map[ process ][1] ++ ;
//	else if ( event.mass2Jet < 120 && event.nJets < 4 && event.nIsoLep == 0 )
//		map[ process ][2] ++ ;
//	else if ( std::sqrt(event.pMiss[0]*event.pMiss[0] + event.pMiss[1]*event.pMiss[1]) > 20 && event.nJets == 3 )
//		map[ process ][3] ++ ;
//	else if ( std::sqrt(event.pMiss[0]*event.pMiss[0] + event.pMiss[1]*event.pMiss[1]) > 20 && event.nJets > 3 )
//		map[ process ][4] ++ ;
//	else if ( std::sqrt(event.pMiss[0]*event.pMiss[0] + event.pMiss[1]*event.pMiss[1]) < 20 )
//		map[ process ][5] ++ ;
//	else
//		map[ process ][6] ++ ;
//}



void printMap(std::map<std::string , std::array<int,10>>& map)
{
	std::array<std::string,10> cutsName = {{"" , "all" , "fail" , "range" , "zPt" , "mVsE" , "thru" , "ww" , "zz" , "ww3"}} ;
	std::cout << std::right << std::setw(20) << "Process" << " : " ;
	for ( unsigned int i = 1 ; i < cutsName.size() ; ++i )
	{
		std::cout << std::internal << std::setw(7) << cutsName[i] << " " ;
	}
	std::cout << std::endl ;

	const auto& procZH = map.at("ZH") ;
	double effZH = 1.0*procZH[procZH.size() - 1]/procZH[0] ;

	for ( const auto& process : map )
	{
		std::vector<double> effVec ( process.second.size() - 1 , 0 ) ;
		for ( unsigned int i = 1 ; i < process.second.size() ; ++i )
			effVec[ i-1 ] = 1.0*process.second[i]/process.second[0] ;


		std::cout << std::right << std::setw(20) << process.first << " : " ;
		for ( const auto& eff : effVec )
		{
			std::cout << std::fixed << std::setprecision(2) << std::internal << std::setw(7) << 100*eff << " " ;
		}

		if ( process.first[0] == std::string("H") )
			std::cout << std::fixed << std::setprecision(2) << std::setw(7) << 100*(effVec.back()-effZH)/effZH << " " ;
		std::cout << std::endl ;
	}
}

std::map< std::string , std::array<double,2> > computeWeightsDifferentSM()
{
	std::map< std::string , std::array<double,2> > map ;

	for ( const auto& proc : higgsDecayMap )
	{
		double xSect = proc.second.xSect ;
		double xSectMore = xSect + 0.05 ;

		std::array<double , 2> weightsMore = {{ xSectMore/xSect , (1-xSectMore)/(1-xSect) }} ;
		map.insert( { proc.first , weightsMore } ) ;
	}

	return map ;
}

int main ( int argc , char** argv )
{
	const std::set<int> signal = { 106485 , 106486 , 108015 , 108016 , 108079 , 108080 , 108031 , 108032 , 108047 , 108048 , 108063 , 108064 , 106525 , 106526 } ;

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

	//	std::vector<int> processIDVec = {106485,106551,106577,106573,106575,106607,106486,106552,106578,106574,106576,106608} ;
	std::vector<int> processIDVec = {} ;

	for ( auto it = ChannelInfoMap.begin() ; it != ChannelInfoMap.end() ; ++it )
		processIDVec.push_back(it->first) ;

	std::string dir = argv[2] ;

	std::array< std::array<double,10> , 2> nEventsSignalOrBackground = {} ;

	auto weightsDifferentSM = computeWeightsDifferentSM() ;
	std::map<std::string , double> nEventsDifferentSM = {} ;

	std::map<std::string , std::array<double,10>> nEventsProcessMap = {} ;
	std::map<std::string , std::array<int,10>> processSelecMap = {} ;

	std::map<std::string , std::array<int,10>> processSelecMapVis = {} ;
	std::map<std::string , std::array<int,10>> processSelecMapInvis = {} ;

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


		double nEventsExpected = ChannelInfoMap.at(processID).xSect*lumi ;
		nEventsExpected *= polWeightMap.at( ChannelInfoMap.at(processID).pol ) ;
		double weight = nEventsExpected/tree->GetEntries() ;


		for ( const auto& proc : higgsDecayMap )
			nEventsDifferentSM.insert( { proc.first , 0.0 } ) ;


		eventReader.setTree(tree) ;

		int iEntry = 0 ;
		while( tree->GetEntry( iEntry++ ) )
		{
			Event event = eventReader.getEvent(iEntry-1) ;

			event.processID = processID ;
			eventReader.processID = processID ;
			auto process = Process::getProcess(event) ;

			//
			if ( processID == 106485 || processID == 106486 )
				if ( event.decayID == 13 || event.decayID == 22 || event.decayID == 23 || event.decayID == 25 )
					continue ;

			if ( processID == 108063 || processID == 108064 )
				if ( event.subDecayID == 7 )
					continue ;

			auto cutVal = Cut::finalCut(event) ;

			for ( unsigned int i = 0 ; i <= cutVal ; ++i )
			{
				nEventsProcessMap[ process ][i] += weight ;
				processSelecMap[ process ][i] ++ ;

				if ( signal.count(processID) )
				{
					nEventsSignalOrBackground[0].at(i) += weight ;

					if ( i != 9 )
						continue ;
					for ( const auto& decay : higgsDecayMap )
					{
						if ( decay.first == std::string("H->ZZ->vvvv") )
						{
							if ( event.decayID == 23 && event.subDecayID == 7 )
								nEventsDifferentSM[decay.first] += weight*weightsDifferentSM.at(decay.first)[0] ;
							else
								nEventsDifferentSM[decay.first] += weight*weightsDifferentSM.at(decay.first)[1] ;
						}
						else
						{
							if ( event.decayID == decay.second.decayID )
								nEventsDifferentSM[decay.first] += weight*weightsDifferentSM.at(decay.first)[0] ;
							else
								nEventsDifferentSM[decay.first] += weight*weightsDifferentSM.at(decay.first)[1] ;
						}
					}
				}
				else
					nEventsSignalOrBackground[1].at(i) += weight ;
			}

			if ( Cut::isInvis(event) )
			{
				processSelecMapVis[ process ][0] ++ ;

				for ( unsigned int i = 0 ; i <= cutVal ; ++i )
					processSelecMapInvis[ process ][i] ++ ;
			}
			else
			{
				processSelecMapInvis[ process ][0] ++ ;

				for ( unsigned int i = 0 ; i <= cutVal ; ++i )
					processSelecMapVis[ process ][i] ++ ;
			}

		}


		if ( !signal.count(processID) )
			continue ;

		iEntry = 0 ;
		while( tree->GetEntry( iEntry++ ) )
		{
			Event event = eventReader.getEvent(iEntry-1) ;

			event.processID = processID ;
			eventReader.processID = processID ;
			auto processList = Process::getSubProcess(event) ;

			if ( processID == 106485 || processID == 106486 )
				if ( event.decayID == 13 || event.decayID == 22 || event.decayID == 23 || event.decayID == 25 )
					continue ;

			if ( processID == 108063 || processID == 108064 )
				if ( event.subDecayID == 7 )
					continue ;

			auto cutVal = Cut::finalCut(event) ;


			for ( unsigned int i = 0 ; i <= cutVal ; ++i )
			{
				for ( const auto& process : processList )
				{
					nEventsProcessMap[ process ][i] += weight ;
					processSelecMap[ process ][i] ++ ;
				}
			}

			if ( Cut::isInvis(event) )
			{
				for ( const auto& process : processList )
				{
					processSelecMapVis[ process ][0] ++ ;

					for ( unsigned int i = 0 ; i <= cutVal ; ++i )
						processSelecMapInvis[ process ][i] ++ ;
				}
			}
			else
			{
				for ( const auto& process : processList )
				{
					processSelecMapInvis[ process ][0] ++ ;

					for ( unsigned int i = 0 ; i <= cutVal ; ++i )
						processSelecMapVis[ process ][i] ++ ;
				}
			}
		}


		file->Close() ;
	}

	printMap(processSelecMap) ;
	std::cout << std::endl ;

	printMap(processSelecMapVis) ;
	std::cout << std::endl ;

	printMap(processSelecMapInvis) ;
	std::cout << std::endl ;

	const auto& procZH = nEventsProcessMap.at( std::string("ZH") ) ;
	double effZH = 1.0*procZH[9]/procZH[0] ;

	std::array<std::string,5> cutsName = {{"eff" , "effErr" , "devLower" , " dev" , "devUpper"}} ;
	std::cout << std::right << std::setw(20) << "Process" << " : " ;
	for ( unsigned int i = 0 ; i < cutsName.size() ; ++i )
	{
		std::cout << std::fixed << std::right << std::setw(7) << cutsName[i] << " " ;
	}
	std::cout << std::endl ;

	for ( const auto& process : nEventsProcessMap )
	{
		double eff = 1.0*process.second[9]/process.second[0] ;

		//		auto bounds = getEfficienciesBound(processSelecMap.at(process.first)[6] , processSelecMap.at(process.first)[0]) ;

		double err = (eff*(1-eff))/processSelecMap.at(process.first)[0] ;
		err = std::sqrt(err) ;
		std::cout << std::right << std::setw(20) << process.first << " : " ;
		//		std::cout << std::fixed << std::setprecision(2) << std::internal << std::setw(7) << 100*bounds[0] << " " ;
		std::cout << std::fixed << std::setprecision(2) << std::right << std::setw(7) << 100*eff << " " ;
		//		std::cout << std::fixed << std::setprecision(2) << std::internal << std::setw(7) << 100*bounds[1] << " " ;
		std::cout << std::fixed << std::setprecision(2) << std::right << std::setw(7) << "+- " << 100*err << " " ;

		if ( process.first[0] == std::string("H") )
		{
			std::cout << std::fixed << std::setprecision(2) << std::right << std::setw(7) << 100*(eff-err-effZH)/effZH << " " ;
			std::cout << std::fixed << std::setprecision(2) << std::right << std::setw(7) << 100*(eff-effZH)/effZH << " " ;
			std::cout << std::fixed << std::setprecision(2) << std::right << std::setw(7) << 100*(eff+err-effZH)/effZH << " " ;
		}
		std::cout << std::endl ;
	}

	for ( unsigned int i = 0 ; i < 10 ; ++i )
		std::cout << "Scinificance " << i << " : " << nEventsSignalOrBackground[0][i]/std::sqrt(nEventsSignalOrBackground[0][i] + nEventsSignalOrBackground[1][i]) << std::endl ;

	std::cout << "nEventsSignal : " << nEventsSignalOrBackground[0][9] << " , nEventsBackground : " << nEventsSignalOrBackground[1][9] << std::endl ;

	double xSect = nEventsSignalOrBackground[0][9]/(lumi*effZH) ;
	std::cout << "xSect : " << xSect << std::endl ;

	std::cout << "+5% bias : " << std::endl ;
	for ( const auto& it : nEventsDifferentSM )
	{
		double xSectMore = it.second/(lumi*effZH) ;

		std::cout << std::right << std::setw(20) << it.first << " : " ;
		//		std::cout << std::fixed << std::setprecision(2) << std::right << std::setw(7) << it.second << " " ;
		std::cout << std::fixed << std::setprecision(2) << std::right << std::setw(7) << 100*(xSectMore - xSect)/xSect << " " ;
		std::cout << std::endl ;
	}

	std::cout << "+5% th bias : " << std::endl ;
	for ( const auto& it : higgsDecayMap )
	{
		double br = it.second.xSect ;
		double brOther = 1-br ;
		double effProcess = 1.0*nEventsProcessMap.at(it.first)[9]/nEventsProcessMap.at(it.first)[0] ;
		double effOther = (effZH-effProcess*br)/brOther ;

		double effBias = (br+0.05)*effProcess + (brOther-0.05)*effOther ;


		std::cout << std::right << std::setw(20) << it.first << " : " ;
		std::cout << std::fixed << std::setprecision(2) << std::right << std::setw(7) << 100*(effBias - effZH)/effZH << " " ;
		std::cout << std::endl ;


	}

	return 0 ;
}
