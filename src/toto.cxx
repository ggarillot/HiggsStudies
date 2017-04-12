#include <iostream>
#include <limits>
#include <sstream>

#include <CLHEP/Vector/ThreeVector.h>
#include <fastjet/ClusterSequence.hh>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TStyle.h>

using namespace fastjet ;

int main(int argc , char** argv)
{
	TH1D* higgsHisto = new TH1D("higgs","", 400 , 0 , 200) ;
	TH1D* zHisto = new TH1D("z","", 400 , 0 , 200) ;
	higgsHisto->SetLineColor(kBlue-4) ;
	zHisto->SetLineColor(kRed-4) ;


	TH1D* y45 = new TH1D("y45","", 50 , 0 , 5) ;
	TH1D* y56 = new TH1D("y56","", 50 , 0 , 5) ;
	y45->SetLineColor(kBlue-4) ;
	y56->SetLineColor(kRed-4) ;

	TH2D* dHisto = new TH2D("2dHisto","", 400 , 0 , 200 , 400 , 0 , 200) ;

	for ( int num = 1 ; num < 26 ; ++num )
	{
		std::stringstream toto ;
		toto << "/home/garillot/files/Higgs/mILD_o2_v05.E250-TDR_ws.Pqqh.Gwhizard-1_95.eL.pR.I106485.001.p" ;

		if ( num < 10 )
			toto << "00" << num ;
		else
			toto << "0" << num ;
		toto << ".root" ;

		TFile* file = TFile::Open( toto.str().c_str() ) ;
		if ( !file )
			continue ;

		TTree* tree = dynamic_cast<TTree*>( file->Get("PfoAnalysisTree") ) ;
		if ( !tree )
		{
			std::cout << "Error : PfoAnalysisTree not found" << std::endl ;
			continue ;
		}



		int nPfos ;
		std::vector<double>* pfoEnergies = NULL ;
		std::vector<double>* pfoPx = NULL ;
		std::vector<double>* pfoPy = NULL ;
		std::vector<double>* pfoPz = NULL ;

						tree->SetBranchAddress("nPfosTotal" , &nPfos) ;
						tree->SetBranchAddress("pfoEnergies" , &pfoEnergies) ;
						tree->SetBranchAddress("pfoPx" , &pfoPx) ;
						tree->SetBranchAddress("pfoPy" , &pfoPy) ;
						tree->SetBranchAddress("pfoPz" , &pfoPz) ;

//		tree->SetBranchAddress("nPfoTargetsTotal" , &nPfos) ;
//		tree->SetBranchAddress("pfoTargetEnergies" , &pfoEnergies) ;
//		tree->SetBranchAddress("pfoTargetPx" , &pfoPx) ;
//		tree->SetBranchAddress("pfoTargetPy" , &pfoPy) ;
//		tree->SetBranchAddress("pfoTargetPz" , &pfoPz) ;


		int iEntry = 1 ;
		while ( tree->GetEntry( iEntry++ ) )
		{

			if ( static_cast<unsigned int>( nPfos ) != pfoEnergies->size()
				 || static_cast<unsigned int>( nPfos ) != pfoPx->size()
				 || static_cast<unsigned int>( nPfos ) != pfoPy->size()
				 || static_cast<unsigned int>( nPfos ) != pfoPz->size() )
				std::cout << "error sizes" << std::endl ;


			std::vector< fastjet::PseudoJet > particles ;

			for ( unsigned int i = 0 ; i < static_cast<unsigned int>( nPfos ) ; ++i )
				particles.push_back( fastjet::PseudoJet( pfoPx->at(i) , pfoPy->at(i) , pfoPz->at(i) , pfoEnergies->at(i) ) ) ;


			// choose a jet definition
			double R = 3.14159265/2.0;
			JetDefinition jet_def(kt_algorithm , R) ;

			// run the clustering, extract the jets
			ClusterSequence cs(particles, jet_def) ;
			std::vector<PseudoJet> jets = sorted_by_pt( cs.exclusive_jets(4) ) ;
			//	std::vector<PseudoJet> jets2 = sorted_by_pt( cs.inclusive_jets() ) ;

			y45->Fill( -std::log10( cs.exclusive_ymerge(3) ) ) ;
			y56->Fill( -std::log10( cs.exclusive_ymerge(4) ) ) ;

			if ( -std::log10( cs.exclusive_ymerge(4) ) > 3.5 )
				continue ;


			if  ( jets.size() != 4 )
			{
				std::cout << "Error : not 4 jets" << std::endl ;
				return 3 ;
			}

			//			std::vector< std::pair<PseudoJet,PseudoJet> > pairs ;
			//			pairs.push_back( std::make_pair( jets.at(0)+jets.at(1) , jets.at(2)+jets.at(3) ) ) ;
			//			pairs.push_back( std::make_pair( jets.at(0)+jets.at(2) , jets.at(1)+jets.at(3) ) ) ;
			//			pairs.push_back( std::make_pair( jets.at(0)+jets.at(3) , jets.at(1)+jets.at(2) ) ) ;
			//			pairs.push_back( std::make_pair( jets.at(2)+jets.at(3) , jets.at(0)+jets.at(1) ) ) ;
			//			pairs.push_back( std::make_pair( jets.at(1)+jets.at(3) , jets.at(0)+jets.at(2) ) ) ;
			//			pairs.push_back( std::make_pair( jets.at(1)+jets.at(2) , jets.at(0)+jets.at(3) ) ) ;

			std::vector< std::pair< std::pair<PseudoJet,PseudoJet> ,std::pair<PseudoJet,PseudoJet> > > pairs ;
			pairs.push_back( std::make_pair( std::make_pair(jets.at(0) , jets.at(1) ) , std::make_pair(jets.at(2) , jets.at(3) ) ) ) ;
			pairs.push_back( std::make_pair( std::make_pair(jets.at(0) , jets.at(2) ) , std::make_pair(jets.at(1) , jets.at(3) ) ) ) ;
			pairs.push_back( std::make_pair( std::make_pair(jets.at(0) , jets.at(3) ) , std::make_pair(jets.at(1) , jets.at(2) ) ) ) ;
			pairs.push_back( std::make_pair( std::make_pair(jets.at(2) , jets.at(3) ) , std::make_pair(jets.at(0) , jets.at(1) ) ) ) ;
			pairs.push_back( std::make_pair( std::make_pair(jets.at(1) , jets.at(3) ) , std::make_pair(jets.at(0) , jets.at(2) ) ) ) ;
			pairs.push_back( std::make_pair( std::make_pair(jets.at(1) , jets.at(2) ) , std::make_pair(jets.at(0) , jets.at(3) ) ) ) ;





			double chi2 = std::numeric_limits<double>::max() ;

			//			std::vector< std::pair<PseudoJet,PseudoJet> >::iterator goodPairIt = pairs.end() ;
			std::vector< std::pair< std::pair<PseudoJet,PseudoJet> ,std::pair<PseudoJet,PseudoJet> > >::iterator goodPairIt = pairs.end() ;

			//			for ( std::vector< std::pair<PseudoJet,PseudoJet> >::iterator it = pairs.begin() ; it != pairs.end() ; ++it )
			for ( std::vector< std::pair< std::pair<PseudoJet,PseudoJet> , std::pair<PseudoJet,PseudoJet> > >::iterator it = pairs.begin() ; it != pairs.end() ; ++it )
			{

//				double tempchi2 = std::abs( (it->first.first + it->first.second).m() - 91.2 ) ;
				double tempchi2 = std::abs( (it->first.first + it->first.second).m() - 91.2 ) + std::abs( (it->second.first + it->second.second).m() - 125.0 ) ;


				if (tempchi2 < chi2)
				{
					chi2 = tempchi2 ;
					goodPairIt = it ;
				}
			}


			// print out some infos
			//	std::cout << "Clustering with " << jet_def.description() << std::endl;

			// print the jets
			//	std::cout <<   "        e  m" << std::endl ;
			//	for ( unsigned int i = 0 ; i < jets.size() ; i++ )
			//		std::cout << "jet " << i << ": "<< jets.at(i).e() << " " << jets.at(i).m() << std::endl ;


			//		std::cout << " h  z " << std::endl ;

			PseudoJet qqDiJet = goodPairIt->first.first + goodPairIt->first.second ;
			double pqq = qqDiJet.pt()*qqDiJet.pt() + qqDiJet.pz()*qqDiJet.pz() ;

			double mrec = (250.0 - qqDiJet.e() )*(250.0 - qqDiJet.e() ) - pqq ;

			higgsHisto->Fill( (goodPairIt->second.first + goodPairIt->second.second).m() ) ;
//			higgsHisto->Fill( std::sqrt(mrec) ) ;
			zHisto->Fill( qqDiJet.m() ) ;

			dHisto->Fill( qqDiJet.m() , (goodPairIt->second.first + goodPairIt->second.second).m() ) ;


		}
		file->Close() ;
	}

	TApplication* app = new TApplication("app" , 0 , 0) ;
	TCanvas* c1 = new TCanvas("c1","c1",900,900) ;
	c1->cd() ;
	//	gStyle->SetOptStat(0) ;

	THStack* stack = new THStack("stack" , "") ;
	stack->Add(higgsHisto) ;
	stack->Add(zHisto) ;
	stack->Draw("nostack") ;

	TCanvas* c2 = new TCanvas("c2","c2",900,900) ;
	c2->cd() ;
	//	gStyle->SetOptStat(0) ;

	dHisto->Draw("colz") ;



	app->Run() ;



	return 0 ;
}
