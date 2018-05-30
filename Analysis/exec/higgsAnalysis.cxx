#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include "json.hpp"

#include "Event.h"
#include "ChannelInfo.h"
#include "Cuts.h"

#include "TFile.h"
#include "TTree.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "THStack.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TText.h"
#include "TLatex.h"
#include "THStack.h"

struct Process
{
		std::string name ;
		Color_t color ;
} ;

static const std::map<std::string , Process> processMap =
{
	{"ZH" , {"ZH" , kBlack} } ,
	{"Z->qq" , {"Z->qq" , kPink} },
	{"WW->qqqq" , {"WW->qqqq" , kMagenta+2} },
	{"WW->qqlv" , {"WW->qqlv" , kMagenta-4} },
	{"ZZ->qqqq" , {"ZZ->qqqq" , kGreen+2} } ,
	{"ZZ->qqll" , {"ZZ->qqll" , kGreen-3} } ,
	{"ZZ->qqvv" , {"ZZ->qqvv" , kCyan+2} }
} ;

Process getProcess(const Event& event)
{
	if ( event.processID == 106485 || event.processID == 106486 )
		return processMap.at("ZH") ;
	else if ( event.processID == 106551 || event.processID == 106552 )
		return processMap.at("WW->qqqq") ;
	else if ( event.processID == 106577 || event.processID == 106578 )
		return processMap.at("WW->qqlv") ;
	else if ( event.processID == 106573 || event.processID == 106574 )
		return processMap.at("ZZ->qqqq") ;
	else if ( event.processID == 106607 || event.processID == 106608 )
		return processMap.at("Z->qq") ;
	else if ( event.processID == 106575 || event.processID == 106576 )
	{
		if ( event.decayID%2 == 0 )
			return processMap.at("ZZ->qqvv") ;
		else
			return processMap.at("ZZ->qqll") ;
	}
	else
		throw ;
}

std::string getSubProcess(const Event& event)
{
	if ( event.processID != 106485 && event.processID != 106486 )
		throw ;

	if ( event.decayID != 24 )
		throw ;

	if ( event.subDecayID == 1 )
		return std::string("WW->qqqq") ;
	else if ( event.subDecayID > 30 || event.subDecayID < 34 )
		return std::string("WW->qqlv") ;
	else if ( event.subDecayID > 600 )
		return std::string("WW->lvlv") ;
	else
		throw ;
}

void setStyle(TH1* histo)
{
	histo->SetLineWidth(2) ;
	histo->GetXaxis()->SetLabelSize(0.025f) ;
	histo->GetYaxis()->SetLabelSize(0.025f) ;
}
void setStyle(THStack* histo)
{
	histo->GetXaxis()->SetLabelSize(0.025f) ;
	histo->GetYaxis()->SetLabelSize(0.025f) ;
}

void setStyle(TCanvas* canvas)
{
	canvas->SetTicks() ;
	canvas->SetTopMargin(0.05f) ;
	canvas->SetRightMargin(0.05f) ;
}

void addWIP(TCanvas* canvas)
{
	canvas->cd() ;
	TText* t = new TText(.95,.96,"Work in progress") ;
	t->SetNDC() ;
	t->SetTextColor(kGray) ;
	t->SetTextAlign(31);
	t->SetTextSize(0.03f) ;
	t->Draw() ;
}

void addPolText(TCanvas* canvas , std::string pol)
{
	canvas->cd() ;
	TLatex* t = new TLatex(.10 , .96 , pol.c_str()) ;
	t->SetNDC() ;
	t->SetTextAlign(11) ;
	t->SetTextSize(0.03f) ;
	t->Draw() ;
}

void addLumiText(TCanvas* canvas)
{
	canvas->cd() ;
	TLatex* latex = new TLatex(.525 , .96 ,"L_{int} = 500 fb^{-1}") ;
	latex->SetNDC() ;
	latex->SetTextSize(0.03f) ;
	latex->SetTextAlign(21) ;
	latex->Draw() ;
}

int main(int argc , char** argv)
{
	gStyle->SetOptStat(0) ;

	if ( argc < 2 )
		std::cout << "Error : no file provided" << std::endl ;

	std::string polText = "" ;
	if ( argc == 3 )
	{
		if ( argv[2] == std::string("RL") )
			polText = "P(e^{-},e^{+}) = (+0.8 , -0.3)" ;
		else if ( argv[2] == std::string("LR") )
			polText = "P(e^{-},e^{+}) = (-0.8 , +0.3)" ;
	}
	else
	{
		std::cout << "WARNING : no polarisation given" << std::endl ;
	}

	TFile* file = TFile::Open( argv[1] ) ;
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

	EventReader eventReader ;
	eventReader.setTree(tree) ;

	tree->SetBranchAddress("processID" , &eventReader.processID) ;
	tree->SetBranchAddress("preRejected" , &eventReader.preRejected) ;
	tree->SetBranchAddress("BDT" , &eventReader.BDT) ;
	tree->SetBranchAddress("weight" , &eventReader.weight) ;

	std::map<std::string , std::array<double,3> > nEventsMap = {} ;
	std::map<std::string , TH1D*> zMassHistoMap = {} ;
	std::map<std::string , TH1D*> recMassHistoMap = {} ;
	std::map<std::string , TH1D*> cosThetaZHistoMap = {} ;
	std::map<std::string , TH1D*> thetaZ12HistoMap = {} ;
	std::map<std::string , TH1D*> y23HistoMap = {} ;
	std::map<std::string , TH1D*> y34HistoMap = {} ;

	std::map<std::string , TH1D*> zPtHistoMap = {} ;
	std::map<std::string , TH1D*> mass2JetHistoMap = {} ;

	std::map<std::string , TH1D*> cosThetaMissHistoMap = {} ;

	std::map<std::string , TH1D*> recMassFinalHistoMap = {} ;

	std::array<TH2D* , 2> zMassVsRecMassVec = {} ;
	{
		TH2D* histoS = new TH2D("histo2dS" , ";m_{Z} (GeV);m_{recoil}" , 200 , 70 , 110 , 200 , 100 , 200) ;
		TH2D* histoB = new TH2D("histo2dB" , ";m_{Z} (GeV);m_{recoil}" , 200 , 70 , 110 , 200 , 100 , 200) ;
		histoS->SetDirectory(0) ;
		histoB->SetDirectory(0) ;
		setStyle(histoS) ;
		setStyle(histoB) ;
		zMassVsRecMassVec[0] = histoS ;
		zMassVsRecMassVec[1] = histoB ;
	}

	std::map<int , std::array<double,7> > nEventsDecayMap = {} ;

	for ( const auto& process : processMap )
	{
		std::stringstream toto ; toto << process.first << "_zMass" ;
		TH1D* histoZMass = new TH1D( toto.str().c_str() , ";m_{z} (GeV);#normalized events" , 150 , 30 , 150 ) ;
		histoZMass->SetDirectory(0) ;
		histoZMass->SetLineColor(process.second.color) ;

		std::stringstream toto2 ; toto2 << process.first << "_recMass" ;
		TH1D* histoRecMass = new TH1D( toto2.str().c_str() , ";m_{recoil} (GeV);#normalized events" , 150 , 0 , 220 ) ;
		histoRecMass->SetDirectory(0) ;
		histoRecMass->SetLineColor(process.second.color) ;

		std::stringstream toto3 ; toto3 << process.first << "_cosThetaZ" ;
		TH1D* histoCosThetaZ = new TH1D( toto3.str().c_str() , ";cos_{#theta};#normalized events" , 100 , -1 , 1 ) ;
		histoCosThetaZ->SetDirectory(0) ;
		histoCosThetaZ->SetLineColor(process.second.color) ;

		std::stringstream toto4 ; toto4 << process.first << "_ThetaZ12" ;
		TH1D* histoThetaZ12 = new TH1D( toto4.str().c_str() , ";#theta_{12};#normalized events" , 100 , 0 , 3.1415926 ) ;
		histoThetaZ12->SetDirectory(0) ;
		histoThetaZ12->SetLineColor(process.second.color) ;

		std::stringstream toto5 ; toto5 << process.first << "_y23" ;
		TH1D* histoy23 = new TH1D( toto5.str().c_str() , ";#-log10(y_{23});#normalized events" , 100 , 0 , 5 ) ;
		histoy23->SetDirectory(0) ;
		histoy23->SetLineColor(process.second.color) ;

		std::stringstream toto6 ; toto6 << process.first << "_y34" ;
		TH1D* histoy34 = new TH1D( toto6.str().c_str() , ";#-log10(y_{34});#normalized events" , 100 , 0 , 5 ) ;
		histoy34->SetDirectory(0) ;
		histoy34->SetLineColor(process.second.color) ;

		std::stringstream totozPt ; totozPt << process.first << "_zPt" ;
		TH1D* histozPt = new TH1D( totozPt.str().c_str() , ";Z_{P_{T}} (GeV);#normalized events" , 100 , 0 , 150 ) ;
		histozPt->SetDirectory(0) ;
		histozPt->SetLineColor(process.second.color) ;

		std::stringstream toto2Jm ; toto2Jm << process.first << "_2jm" ;
		TH1D* histoMass2Jet = new TH1D( toto2Jm.str().c_str() , ";m_{2jets} (GeV);#normalized events" , 100 , 0 , 250 ) ;
		histoMass2Jet->SetDirectory(0) ;
		histoMass2Jet->SetLineColor(process.second.color) ;

		std::stringstream totocm ; totocm << process.first << "_cm" ;
		TH1D* histoCosThetaMiss = new TH1D( totocm.str().c_str() , ";cos#theta_{miss};#normalized events" , 100 , -1 , 1 ) ;
		histoCosThetaMiss->SetDirectory(0) ;
		histoCosThetaMiss->SetLineColor(process.second.color) ;

		std::stringstream toto7 ; toto7 << process.first << "_recMass" ;
		TH1D* histoRecMassFinal = new TH1D( toto7.str().c_str() , ";m_{recoil} (GeV);n events" , 30 , 100 , 160 ) ;
		histoRecMassFinal->SetDirectory(0) ;

		if ( process.first == std::string("ZH") )
		{
			histoRecMassFinal->SetLineColor(process.second.color) ;
		}
		else
		{
			histoRecMassFinal->SetFillColor(process.second.color) ;
			histoRecMassFinal->SetLineColor(kBlack) ;
		}

		setStyle(histoZMass) ;
		setStyle(histoRecMass) ;
		setStyle(histoCosThetaZ) ;
		setStyle(histoThetaZ12) ;
		setStyle(histoy23) ;
		setStyle(histoy34) ;
		setStyle(histozPt) ;
		setStyle(histoMass2Jet) ;
		setStyle(histoCosThetaMiss) ;
		setStyle(histoRecMassFinal) ;

		zMassHistoMap.insert( {process.first , histoZMass} ) ;
		recMassHistoMap.insert( {process.first , histoRecMass} ) ;
		cosThetaZHistoMap.insert( {process.first , histoCosThetaZ} ) ;
		thetaZ12HistoMap.insert( {process.first , histoThetaZ12} ) ;
		y23HistoMap.insert( {process.first , histoy23} ) ;
		y34HistoMap.insert( {process.first , histoy34} ) ;
		zPtHistoMap.insert( {process.first , histozPt} ) ;
		mass2JetHistoMap.insert( {process.first , histoMass2Jet} ) ;
		cosThetaMissHistoMap.insert( {process.first , histoCosThetaMiss} ) ;
		recMassFinalHistoMap.insert( {process.first , histoRecMassFinal} ) ;

		nEventsMap.insert( { process.first , {{0,0,0}} } ) ;
	}

	long int nEntries = tree->GetEntries() ;
	int iEntry = 0 ;
	while( tree->GetEntry( iEntry++ ) )
	{
		if ( iEntry%10000 == 0 )
			std::cout << 100.0*iEntry/nEntries << " %" << std::endl ;

		Event event = eventReader.getEvent(iEntry-1) ;


		nEventsMap.at( getProcess(event).name )[0] += eventReader.weight ;

		if ( eventReader.processID == 106485 || eventReader.processID == 106486 )
		{
			nEventsDecayMap[event.decayID][0] += eventReader.weight ;
			if ( event.decayID == 24 || event.decayID == 23 )
				nEventsDecayMap[event.decayID*10+event.subDecayID][0] += eventReader.weight ;
		}

		if ( !event.goodEvent )
			continue ;

		if ( eventReader.processID == 106485 || eventReader.processID == 106486 )
			zMassVsRecMassVec[0]->Fill(event.zMass , event.recMass , eventReader.weight) ;
		else
			zMassVsRecMassVec[1]->Fill(event.zMass , event.recMass , eventReader.weight) ;

		zMassHistoMap.at( getProcess(event).name )->Fill(event.zMass , eventReader.weight) ;
		recMassHistoMap.at( getProcess(event).name )->Fill(event.recMass , eventReader.weight) ;

		y23HistoMap.at( getProcess(event).name )->Fill(event.y23 , eventReader.weight) ;
		y34HistoMap.at( getProcess(event).name )->Fill(event.y34 , eventReader.weight) ;

		zPtHistoMap.at( getProcess(event).name )->Fill(event.zPt , eventReader.weight) ;

		mass2JetHistoMap.at( getProcess(event).name )->Fill(event.mass2Jet , eventReader.weight) ;

		cosThetaMissHistoMap.at( getProcess(event).name )->Fill(event.cosThetaMiss , eventReader.weight) ;

		if ( !Cut::WW_h(event) )
			continue ;
		if ( eventReader.processID == 106485 || eventReader.processID == 106486 )
			nEventsDecayMap[event.decayID][1] += eventReader.weight ;

		if ( !Cut::ZZ_h(event) )
			continue ;
		if ( eventReader.processID == 106485 || eventReader.processID == 106486 )
			nEventsDecayMap[event.decayID][2] += eventReader.weight ;

		if ( !Cut::WW_sl(event) )
			continue ;
		if ( eventReader.processID == 106485 || eventReader.processID == 106486 )
			nEventsDecayMap[event.decayID][3] += eventReader.weight ;

		if ( !Cut::range(event) )
			continue ;
		if ( eventReader.processID == 106485 || eventReader.processID == 106486 )
			nEventsDecayMap[event.decayID][4] += eventReader.weight ;

		if ( !Cut::mass2Jet(event) )
			continue ;

		if ( !Cut::angle(event) )
			continue ;
		if ( eventReader.processID == 106485 || eventReader.processID == 106486 )
		{
			nEventsDecayMap[event.decayID][5] += eventReader.weight ;
			if ( event.decayID == 24 || event.decayID == 23 )
				nEventsDecayMap[event.decayID*10+event.subDecayID][5] += eventReader.weight ;
		}

		nEventsMap.at( getProcess(event).name )[1] += eventReader.weight ;


		cosThetaZHistoMap.at( getProcess(event).name )->Fill(event.cosThetaZ , eventReader.weight) ;
		thetaZ12HistoMap.at( getProcess(event).name )->Fill(event.cosThetaZ12 , eventReader.weight) ;


		if ( eventReader.BDT < -0.033 )
			continue ;

		if ( event.recMass < 100 || event.recMass > 160 )
			continue ;

		recMassFinalHistoMap.at( getProcess(event).name )->Fill(event.recMass , eventReader.weight) ;

		nEventsMap.at( getProcess(event).name )[2] += eventReader.weight ;

		if ( eventReader.processID == 106485 || eventReader.processID == 106486 )
		{
			nEventsDecayMap[event.decayID][6] += eventReader.weight ;
			if ( event.decayID == 24 || event.decayID == 23 )
				nEventsDecayMap[event.decayID*10+event.subDecayID][6] += eventReader.weight ;
		}
	}

	file->Close() ;


	TCanvas* c1 = new TCanvas("c1" , "c1" , 1000 , 1000) ;
	c1->cd() ;
	setStyle(c1) ;

	double maximum = std::numeric_limits<double>::min() ;
	TH1D* maxHisto = nullptr ;

	for (const auto& histo : zMassHistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : zMassHistoMap)
		histo.second->DrawNormalized("HIST same") ;
	zMassHistoMap.at("ZH")->DrawNormalized("HIST same") ;


	//legend
	TLegend* leg = new TLegend(0.65,0.65,0.88,0.88) ;
	leg->SetBorderSize(0) ;
	TLegendEntry* le = leg->AddEntry(zMassHistoMap.at("ZH") , "ZH" , "f") ;
	le->SetTextColor( processMap.at("ZH").color ) ;
	for ( const auto& histo : zMassHistoMap )
	{
		if ( histo.first.c_str() == std::string("ZH") )
			continue ;
		le = leg->AddEntry(histo.second , histo.first.c_str() , "f") ;
		le->SetTextColor( processMap.at(histo.first).color ) ;
	}
	leg->Draw() ;
	addWIP(c1) ;
	addPolText(c1,polText) ;


	TCanvas* c2 = new TCanvas("c2" , "c2" , 1000 , 1000) ;
	c2->cd() ;
	setStyle(c2) ;

	maximum = std::numeric_limits<double>::min() ;
	maxHisto = nullptr ;

	for (const auto& histo : recMassHistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : recMassHistoMap)
		histo.second->DrawNormalized("HIST same") ;
	recMassHistoMap.at("ZH")->DrawNormalized("HIST same") ;

	leg->Draw() ;
	addWIP(c2) ;
	addPolText(c2,polText) ;


	TCanvas* c3 = new TCanvas("c3" , "c3" , 1000 , 1000) ;
	c3->cd() ;
	setStyle(c3) ;

	maximum = std::numeric_limits<double>::min() ;
	maxHisto = nullptr ;

	for (const auto& histo : cosThetaZHistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : cosThetaZHistoMap)
		histo.second->DrawNormalized("HIST same") ;
	cosThetaZHistoMap.at("ZH")->DrawNormalized("HIST same") ;

	leg->Draw() ;
	addWIP(c3) ;
	addPolText(c3,polText) ;


	TCanvas* c4 = new TCanvas("c4" , "c4" , 1000 , 1000) ;
	c4->cd() ;
	setStyle(c4) ;

	maximum = std::numeric_limits<double>::min() ;
	maxHisto = nullptr ;

	for (const auto& histo : thetaZ12HistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : thetaZ12HistoMap)
		histo.second->DrawNormalized("HIST same") ;
	thetaZ12HistoMap.at("ZH")->DrawNormalized("HIST same") ;

	leg->Draw() ;
	addWIP(c4) ;
	addPolText(c4,polText) ;


	TCanvas* c5 = new TCanvas("c5" , "c5" , 1000 , 1000) ;
	c5->cd() ;
	setStyle(c5) ;

	maximum = std::numeric_limits<double>::min() ;
	maxHisto = nullptr ;

	for (const auto& histo : y23HistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : y23HistoMap)
		histo.second->DrawNormalized("HIST same") ;
	y23HistoMap.at("ZH")->DrawNormalized("HIST same") ;

	leg->Draw() ;
	addWIP(c5) ;
	addPolText(c5,polText) ;


	TCanvas* c6 = new TCanvas("c6" , "c6" , 1000 , 1000) ;
	c6->cd() ;
	setStyle(c6) ;

	maximum = std::numeric_limits<double>::min() ;
	maxHisto = nullptr ;

	for (const auto& histo : y34HistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : y34HistoMap)
		histo.second->DrawNormalized("HIST same") ;
	y34HistoMap.at("ZH")->DrawNormalized("HIST same") ;

	leg->Draw() ;
	addWIP(c6) ;
	addPolText(c6,polText) ;


	TCanvas* czPt = new TCanvas("czPt" , "czPt" , 1000 , 1000) ;
	czPt->cd() ;
	setStyle(czPt) ;

	maximum = std::numeric_limits<double>::min() ;
	maxHisto = nullptr ;

	for (const auto& histo : zPtHistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : zPtHistoMap)
		histo.second->DrawNormalized("HIST same") ;
	zPtHistoMap.at("ZH")->DrawNormalized("HIST same") ;

	leg->Draw() ;
	addWIP(czPt) ;
	addPolText(czPt,polText) ;


	TCanvas* czM2j = new TCanvas("czM2j" , "czM2j" , 1000 , 1000) ;
	czM2j->cd() ;
	setStyle(czM2j) ;

	maximum = std::numeric_limits<double>::min() ;
	maxHisto = nullptr ;

	for (const auto& histo : mass2JetHistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : mass2JetHistoMap)
		histo.second->DrawNormalized("HIST same") ;
	mass2JetHistoMap.at("ZH")->DrawNormalized("HIST same") ;

	leg->Draw() ;
	addWIP(czM2j) ;
	addPolText(czM2j,polText) ;


	TCanvas* cCTM = new TCanvas("cCTM" , "cCTM" , 1000 , 1000) ;
	cCTM->cd() ;
	setStyle(cCTM) ;

	maximum = std::numeric_limits<double>::min() ;
	maxHisto = nullptr ;

	for (const auto& histo : cosThetaMissHistoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	for (const auto& histo : cosThetaMissHistoMap)
		histo.second->DrawNormalized("HIST same") ;
	cosThetaMissHistoMap.at("ZH")->DrawNormalized("HIST same") ;

	leg->Draw() ;
	addWIP(cCTM) ;
	addPolText(cCTM,polText) ;


	TCanvas* c7 = new TCanvas("c7" , "c7" , 1000 , 1000) ;
	c7->cd() ;
	setStyle(c7) ;

	THStack* hs = new THStack("hs",";m_{recoil} (GeV);nEvents/2GeV") ;

	for ( const auto& histo : recMassFinalHistoMap )
	{
		if ( histo.first == std::string("ZH") || histo.first == std::string("Z->qq"))
			continue ;
		hs->Add(histo.second) ;
	}
	hs->Add( recMassFinalHistoMap.at("Z->qq") ) ;
	hs->Add( recMassFinalHistoMap.at("ZH") ) ;
	hs->Draw("hist") ;
	hs->GetXaxis()->SetLabelSize(0.025f) ;
	hs->GetYaxis()->SetLabelSize(0.025f) ;
	c7->Modified() ;

	//legend
	TLegend* leg2 = new TLegend(0.65,0.65,0.88,0.88) ;
	leg2->SetBorderSize(0) ;
	TLegendEntry* le2 = leg2->AddEntry(recMassFinalHistoMap.at("ZH") , "ZH" , "f") ;
	le2->SetTextColor( processMap.at("ZH").color ) ;
	for ( const auto& histo : recMassFinalHistoMap )
	{
		if ( histo.first.c_str() == std::string("ZH") )
			continue ;
		le2 = leg2->AddEntry(histo.second , histo.first.c_str() , "f") ;
		le2->SetTextColor( processMap.at(histo.first).color ) ;
	}
	leg2->Draw() ;

	addWIP(c7) ;
	addPolText(c7,polText) ;
	addLumiText(c7) ;


	TCanvas* c2DS = new TCanvas("c2DS" , "c2DS" , 1000 , 1000) ;
	c2DS->cd() ;
	setStyle(c2DS) ;

	zMassVsRecMassVec.at(0)->Draw("col") ;

	addWIP(c2DS) ;
	addPolText(c2DS,polText) ;


	TCanvas* c2DB = new TCanvas("c2DB" , "c2DB" , 1000 , 1000) ;
	c2DB->cd() ;
	setStyle(c2DB) ;

	zMassVsRecMassVec.at(1)->Draw("col") ;

	addWIP(c2DB) ;
	addPolText(c2DB,polText) ;


	TFile* outputFile = new TFile("test.root" , "RECREATE") ;
	outputFile->cd() ;


	c1->Write("c1") ;
	c2->Write("c2") ;
	c3->Write("c3") ;
	c4->Write("c4") ;
	c5->Write("c5") ;
	c6->Write("c6") ;
	c7->Write("c7") ;
	czPt->Write("czPt") ;
	czM2j->Write("czM2j") ;
	c2DS->Write("c2DS") ;
	c2DB->Write("c2DB") ;
	cCTM->Write("cCTM") ;

	outputFile->Close() ;


	double nEventsBg = 0 ;
	double nEventsSignal = 0 ;
	for ( const auto& name : nEventsMap )
	{
		double effPreCut = name.second[1]/name.second[0] ;
		double effFinal = name.second[2]/name.second[0] ;

		std::cout << "eff " << name.first << " : " << 100.0*effPreCut << " % , " << 100*effFinal << " %" << std::endl ;

		if ( name.first == std::string("ZH") )
			nEventsSignal += name.second[2] ;
		else
			nEventsBg += name.second[2] ;
	}

	std::cout << std::endl ;

	for ( const auto& decayID : nEventsDecayMap )
	{
		std::array<double,7> effVec = {} ;
		for ( unsigned int i = 1 ; i < 7 ; ++i )
		{
			effVec.at(i) = decayID.second.at(i)/decayID.second[0] ;
		}
		double effPreCut = decayID.second[5]/decayID.second[0] ;
		double effFinal = decayID.second[6]/decayID.second[0] ;

		std::cout << "eff " << decayID.first << " : " << 100.0*effPreCut << " % , " << 100*effFinal << " %" << std::endl ;
		//		std::cout << "eff " << decayID.first << " : " << 100.0*effVec.at(1) << " % , "
		//				  << 100*effVec.at(2) << " % , "
		//				  << 100*effVec.at(3) << " % , "
		//				  << 100*effVec.at(4) << " % , "
		//				  << 100*effVec.at(5) << " % , "
		//				  << 100*effVec.at(6) << " % , " << std::endl ;
	}

	std::cout << "Scinificance : " << nEventsSignal/std::sqrt(nEventsSignal+nEventsBg) << std::endl ;
	return 0 ;
}
