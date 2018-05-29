#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TDirectoryFile.h>
#include <iostream>
#include <vector>
#include <map>
#include <TStyle.h>
#include <TLegend.h>

#include <TCanvas.h>

int test()
{
	gStyle->SetOptStat(0) ;
	TFile* file = new TFile("/home/garillot/Code/HiggsStudies/ClassifiedLR2.root" , "READ") ;
	TTree* tree = dynamic_cast<TTree*>( file->Get("tree") ) ;

	float weight ;
	float BDT ;
	int processID ;
	bool goodEvent ;
	bool preRejected ;
	int decayID ;

	tree->SetBranchAddress("processID" , &processID) ;
	tree->SetBranchAddress("weight" , &weight) ;
	tree->SetBranchAddress("BDT" , &BDT) ;
	tree->SetBranchAddress("goodEvent" , &goodEvent) ;
	tree->SetBranchAddress("preRejected" , &preRejected) ;
	tree->SetBranchAddress("decayID" , &decayID) ;

	TGraph* graphSignal = new TGraph() ;
	TGraph* graphBkg = new TGraph() ;
	TGraph* graphSc = new TGraph() ;
	
	TH1D* BDTS = new TH1D("bdts",";BDT_{score};#normalized events",100,-0.25,0.1) ;
	TH1D* BDTB = new TH1D("bdtb",";BDT_{score};#normalized events",100,-0.25,0.1) ;
	BDTB->SetLineWidth(2) ;
	BDTS->SetLineWidth(2) ;
	BDTB->SetLineColor(kRed) ;
	
	graphSc->SetMarkerStyle(20) ;
	graphSignal->SetMarkerStyle(20) ;
	graphSignal->SetMarkerColor(kRed) ;
	graphBkg->SetMarkerStyle(20) ;

	double BDTmin = -0.2 ;
	double BDTmax = 0.2 ;
	double step = 0.0005 ;

	int nSteps = (BDTmax-BDTmin)/step ;
	std::vector<double> signalVec(nSteps , 0) ;
	std::vector<double> bkgVec(nSteps , 0) ;
	
	std::map<int,double> nEventsDecayMap = {} ;
	std::map<int , std::vector<double>> nSelDecayMap = {} ;

	std::cout << tree->GetEntries() << " entries" << std::endl ;
	int iEntry = 0 ;
	while ( tree->GetEntry(iEntry++) ) 
	{
		int ca = (BDT-BDTmin)/step ;

		if ( ca > nSteps-1 )
			ca = nSteps-1 ;

		if ( ca < 0 )
			continue ;


		if ( processID == 106485 || processID == 106486 )
				nEventsDecayMap[decayID] += weight ;


		if ( !goodEvent || preRejected )
			continue ;

		if ( processID == 106485 || processID == 106486 )
			BDTS->Fill(BDT,weight) ;
		else
			BDTB->Fill(BDT,weight) ;



		if ( processID == 106485 || processID == 106486 )
		{
			for ( int i = 0 ; i < ca ; ++i )
			{
				signalVec.at(i) += weight ;
				
				if ( !nSelDecayMap.count(decayID) )
					nSelDecayMap[decayID] = std::vector<double>(nSteps , 0) ;
				
				nSelDecayMap[decayID].at(i) += weight ;
			}
		}
		else
		{
			for ( int i = 0 ; i < ca ; ++i )
				bkgVec.at(i) += weight ;
		}

		if ( iEntry%10000 == 0 )
			std::cout << iEntry << " entries processed" << std::endl ;
	}
/*
	double nS = 0 ;
	double nB = 0 ;
	iEntry = 0 ;
	int compteur = 0 ;
	while ( tree->GetEntry(iEntry++) ) 
	{
		if ( !goodEvent || preRejected )
			continue ;

		if ( BDT<0 )
			continue ;

		if ( processID == 106485 || processID == 106486 )
		{
			nS += weight ;
			compteur++ ;
		}
		else
		{
			nB += weight ;
		}

		if ( iEntry%10000 == 0 )
			std::cout << iEntry << " entries processed" << std::endl ;
	}
	
	std::cout << nS << " , " << nB << "  " << 1.0*nS/(std::sqrt(nS+nB)) << std::endl ;
	std::cout << "compteur : " << compteur << std::endl ;
*/

	for (unsigned int i = 0 ; i < nSteps ; ++i)
	{
		graphSignal->SetPoint(graphSignal->GetN() , BDTmin+(i*step) , signalVec.at(i)) ;
		graphBkg->SetPoint(graphBkg->GetN() ,  BDTmin+(i*step) , bkgVec.at(i)) ;

		double scin = signalVec.at(i)/std::sqrt(signalVec.at(i)+bkgVec.at(i)) ;
		graphSc->SetPoint(graphSc->GetN() , BDTmin+(i*step) , scin) ;
	}



	//graphBkg->Draw("AP") ;
	//graphSignal->Draw("P same") ;
	graphSc->Draw("AP") ;
	
		
	TCanvas* c2 = new TCanvas("c2","c2" , 800 , 800) ;
	c2->cd() ;
	BDTS->DrawNormalized("hist") ;
	BDTB->DrawNormalized("hist same") ;
	
		//legend
	TLegend* leg = new TLegend(0.65,0.65,0.88,0.88) ;
	leg->SetBorderSize(0) ;
	leg->AddEntry(BDTS , "Signal" , "f") ;
	leg->AddEntry(BDTB , "Background" , "f") ;
	leg->Draw() ;

/*
	TCanvas* c2 = new TCanvas("c2","c2" , 800 , 800) ;
	c2->cd() ;

	TGraph* effTau = new TGraph() ;
	effTau->SetMarkerStyle(20) ;

	TGraph* effb = new TGraph() ;
	effb->SetMarkerStyle(20) ;
	effb->SetMarkerColor(kRed) ;
	
	for (unsigned int i = 0 ; i < nSteps ; ++i)
	{
		effTau->SetPoint(effTau->GetN() , BDTmin+(i*step) , nSelDecayMap[15].at(i)/nEventsDecayMap[15]) ;
		effb->SetPoint(effb->GetN() , BDTmin+(i*step) , nSelDecayMap[5].at(i)/nEventsDecayMap[5]) ;
		
		std::cout << (nSelDecayMap[5].at(i)/nEventsDecayMap[5])/(nSelDecayMap[15].at(i)/nEventsDecayMap[15]) << std::endl ;
	}

	effTau->Draw("AP") ;
	effb->Draw("P same") ;
*/	
	return 0 ;
}
