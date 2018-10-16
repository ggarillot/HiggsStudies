#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include "json.hpp"

#include "Event.h"
#include "Process.h"
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
#include "TLine.h"
#include "TEllipse.h"

static const std::map<std::string , Color_t> processMap =
{
	{"ZH"               ,  kBlack } ,
	{"q#bar{q}"         ,  kRed-4 } ,
	{"q#bar{q}q#bar{q}" ,  kRed-7 } ,
	{"q#bar{q}l#nu"     ,  kCyan-3} ,
	{"q#bar{q}ll"       ,  kCyan-8} ,
	{"q#bar{q}#nu#nu"   ,  kCyan+2}
} ;

static const std::map<std::string , Color_t> subProcessMap =
{
	{ "ZH"                 , kBlack } ,
	{ "H->q#bar{q}/gg"     , kPink-6 } ,
	//	{ "H->bb"      , kGray+3 } ,
	//	{ "H->cc"      , kGray+2 } ,
	//	{ "H->gg"      , kCyan+2 } ,
	{ "H->#tau#tau"        , kGreen+1 } ,
	{ "H->#mu#mu"          , kGreen-6 } ,
	{ "H->WW"              , kBlue-4 } ,
	{ "H->ZZ"              , kRed-4 } ,
	{ "H->#gamma#gamma"    , kMagenta+2 } ,
	{ "H->Z#gamma"         , kMagenta-6 } ,
	{ "H->ZZ->#nu#nu#nu#nu", kBlack }
} ;


void setStyle(TH1* histo)
{
	histo->SetLineWidth(2) ;
	histo->GetXaxis()->SetLabelSize(0.025f) ;
	histo->GetYaxis()->SetLabelSize(0.025f) ;

	histo->GetYaxis()->SetTitleOffset(1.5f) ;

	histo->GetXaxis()->SetTitleFont(62) ;
	histo->GetYaxis()->SetTitleFont(62) ;
}
void setStyle(THStack* histo)
{
	histo->GetXaxis()->SetLabelSize(0.025f) ;
	histo->GetYaxis()->SetLabelSize(0.025f) ;

	histo->GetYaxis()->SetTitleOffset(1.55f) ;

	histo->GetXaxis()->SetTitleFont(62) ;
	histo->GetYaxis()->SetTitleFont(62) ;
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

void addEnergyText(TCanvas* canvas , double energy)
{
	canvas->cd() ;
	std::stringstream toto ;
	toto << "#sqrt{s} = " << energy << " GeV" ;
	TLatex* t = new TLatex(.525 , .96 , toto.str().c_str()) ;
	t->SetNDC() ;
	t->SetTextAlign(21) ;
	t->SetTextSize(0.03f) ;
	t->Draw() ;
}

void addLumiText(TCanvas* canvas)
{
	canvas->cd() ;
	TLatex* latex = new TLatex(.12 , .90 ,"L_{int} = 500 fb^{-1}") ;
	latex->SetNDC() ;
	latex->SetTextSize(0.03f) ;
	latex->SetTextAlign(11) ;
	latex->Draw() ;
}

TCanvas* drawCanvas(std::string canvasName , TH2D* histo , std::string polText , double energy)
{
	TCanvas* canvas = new TCanvas(canvasName.c_str() , canvasName.c_str() , 1000 , 1000) ;
	canvas->cd() ;
	setStyle(canvas) ;

	histo->Draw("col") ;

	addWIP(canvas) ;
	addPolText(canvas,polText) ;
	addEnergyText(canvas , energy) ;

	return canvas ;
}

TCanvas* drawCanvas(std::string canvasName , std::map<std::string , TH1D*> histoMap , std::string polText , double energy , bool ZH = true , std::vector<double> lines = {})
{
	TCanvas* canvas = new TCanvas(canvasName.c_str() , canvasName.c_str() , 1000 , 1000) ;
	canvas->cd() ;
	setStyle(canvas) ;

	double maximum = std::numeric_limits<double>::min() ;
	TH1D* maxHisto = nullptr ;

	for (const auto& histo : histoMap)
	{
		double max = histo.second->GetBinContent( histo.second->GetMaximumBin() )/histo.second->Integral() ;
		if ( max > maximum )
		{
			maximum = max ;
			maxHisto = histo.second ;
		}
	}

	maxHisto->DrawNormalized("HIST") ;
	histoMap.at("ZH")->DrawNormalized("HIST same") ;
	for ( const auto& histo : histoMap )
	{
		if ( histo.first == std::string("ZH") )
			continue ;
		histo.second->DrawNormalized("HIST same") ;
	}
	if ( ZH )
		histoMap.at("ZH")->DrawNormalized("HIST same") ;

	//legend
	TLegend* leg = new TLegend(0.65,0.65,0.88,0.88) ;
	leg->SetBorderSize(0) ;

	TLegendEntry* le = nullptr ;

	le = leg->AddEntry(histoMap.at("ZH") , "ZH" , "f") ;
	le->SetTextColor( histoMap.at("ZH")->GetLineColor() ) ;

	for ( const auto& histo : histoMap )
	{
		if ( histo.first.c_str() == std::string("ZH") )
			continue ;
		le = leg->AddEntry(histo.second , histo.first.c_str() , "f") ;
		le->SetTextColor( histo.second->GetLineColor() ) ;
	}
	leg->Draw() ;
	addWIP(canvas) ;
	addPolText(canvas,polText) ;
	addEnergyText(canvas , energy) ;

	std::array<double,4> bords = {} ;
	bords[0] = canvas->GetLeftMargin() ;
	bords[1] = canvas->GetBottomMargin() ;
	bords[2] = 1.0 - canvas->GetRightMargin() ;
	bords[3] = 1.0 - canvas->GetTopMargin() ;

	for ( const auto& val : lines )
	{
		double x = ( val - maxHisto->GetXaxis()->GetXmin() ) / (maxHisto->GetXaxis()->GetXmax() - maxHisto->GetXaxis()->GetXmin() ) ;
		x = x*(bords[2]-bords[0]) + bords[0] ;

		TLine* line = new TLine(x,bords[1],x,bords[3]) ;
		line->SetNDC() ;
		line->SetLineColor(kMagenta+2) ;
		line->SetLineWidth(2) ;
		line->SetLineStyle(7) ;
		line->Draw() ;
	}

	canvas->RedrawAxis() ;

	return canvas ;
}

void drawSquare( TCanvas* canvas , double x1 , double y1 , double x2 , double y2 )
{
	canvas->cd() ;
	TLine* line = new TLine(x1,y1,x2,y1) ;
	line->SetLineColor(kGreen-4) ;
	line->SetLineWidth(2) ;

	line->Draw() ;

	line = new TLine(x1,y2,x2,y2) ;
	line->SetLineColor(kGreen-4) ;
	line->SetLineWidth(2) ;

	line->Draw() ;

	line = new TLine(x1,y1,x1,y2) ;
	line->SetLineColor(kGreen-4) ;
	line->SetLineWidth(2) ;

	line->Draw() ;

	line = new TLine(x2,y1,x2,y2) ;
	line->SetLineColor(kGreen-4) ;
	line->SetLineWidth(2) ;

	line->Draw() ;
}

void drawEllipse(TCanvas* canvas , double x , double y , double r)
{
	canvas->cd() ;

	TEllipse* ellipse = new TEllipse(x,y,r) ;
	ellipse->SetLineColor(kGreen-4) ;
	ellipse->SetLineWidth(2) ;
	ellipse->SetFillStyle(0) ;

	ellipse->Draw() ;
}

int main(int argc , char** argv)
{
	//	gStyle->SetPalette(99) ;
	const std::set<int> signal = { 106485 , 106486 , 108015 , 108016 , 108079 , 108080 , 108031 , 108032 , 108047 , 108048 , 108063 , 108064 , 106525 , 106526 } ;
	const std::set<int> qqList = { 106607 , 106608 } ;
	const std::set<int> qqlvList = { 106577 , 106578 , 106564 , 106566 , 106563 , 106565 } ;

	const std::set<int> zz4List = { 106573 , 106574 , 106575 , 106576 , 106560 , 106562 , 106559 , 106561 } ;
	const std::set<int> ww4List = { 106551 , 106552 } ;

	gStyle->SetOptStat(0) ;

	std::string polText = "" ;
	if ( argc >= 1 )
	{
		if ( argv[1] == std::string("RL") )
			polText = "P(e^{-},e^{+}) = (+0.8 , -0.3)" ;
		else if ( argv[1] == std::string("LR") )
			polText = "P(e^{-},e^{+}) = (-0.8 , +0.3)" ;
	}
	else
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
	constexpr double energy = 250 ;

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

	std::vector<int> processIDVec = {} ;

	for ( auto it = ChannelInfoMap.begin() ; it != ChannelInfoMap.end() ; ++it )
		processIDVec.push_back(it->first) ;

	//	processIDVec = {106485,106552} ;

	std::string dir = argv[2] ;


	std::map<std::string , TH1D*> zMassHistoMap = {} ;
	std::map<std::string , TH1D*> recMassHistoMap = {} ;
	std::map<std::string , TH1D*> cosThetaZHistoMap = {} ;
	std::map<std::string , TH1D*> massVsEnergyHistoMap = {} ;
	std::map<std::string , TH1D*> y23HistoMap = {} ;
	std::map<std::string , TH1D*> y34HistoMap = {} ;

	std::map<std::string , TH1D*> ptHistoMap = {} ;
	std::map<std::string , TH1D*> zPtHistoMap = {} ;
	std::map<std::string , TH1D*> mass2JetHistoMap = {} ;

	std::map<std::string , TH1D*> cosThetaMissHistoMap = {} ;

	std::map<std::string , TH1D*> recMassFinalHistoMap = {} ;


	std::map<std::string , TH1D*> zMassHistoSubMap = {} ;
	std::map<std::string , TH1D*> recMassHistoSubMap = {} ;
	std::map<std::string , TH1D*> mass2JetHistoSubMap = {} ;

	std::map<std::string , TH1D*> y23HistoSubMap = {} ;
	std::map<std::string , TH1D*> y34HistoSubMap = {} ;

	std::array<TH2D* , 2> thrustHistos = {} ;
	{
		TH2D* histoS = new TH2D("histoThrustS" , ";Major Thrust;Minor Thrust" , 200 , 0 , 1 , 200 , 0 , 1) ;
		TH2D* histoB = new TH2D("histoThrustB" , ";Major Thrust;Minor Thrust" , 200 , 0 , 1 , 200 , 0 , 1) ;
		histoS->SetDirectory(0) ;
		histoB->SetDirectory(0) ;
		setStyle(histoS) ;
		setStyle(histoB) ;
		thrustHistos[0] = histoS ;
		thrustHistos[1] = histoB ;
	}
	std::array<TH2D* , 2> thrustHistosInvis = {} ;
	{
		TH2D* histoS = new TH2D("histoThrustInvisS" , ";Major Thrust;Minor Thrust" , 200 , 0 , 1 , 200 , 0 , 1) ;
		TH2D* histoB = new TH2D("histoThrustInvisB" , ";Major Thrust;Minor Thrust" , 200 , 0 , 1 , 200 , 0 , 1) ;
		histoS->SetDirectory(0) ;
		histoB->SetDirectory(0) ;
		setStyle(histoS) ;
		setStyle(histoB) ;
		thrustHistosInvis[0] = histoS ;
		thrustHistosInvis[1] = histoB ;
	}

	std::array<TH2D* , 2> histosZZ4 = {} ;
	{
		TH2D* histoS = new TH2D("histosZZ4S" , ";m_{Z};m_{rest}" , 400 , 0 , 200 , 400 , 0 , 200) ;
		TH2D* histoB = new TH2D("histosZZ4B" , ";m_{Z};m_{rest}" , 400 , 0 , 200 , 400 , 0 , 200) ;
		histoS->SetDirectory(0) ;
		histoB->SetDirectory(0) ;
		setStyle(histoS) ;
		setStyle(histoB) ;
		histosZZ4[0] = histoS ;
		histosZZ4[1] = histoB ;
	}
	std::array<TH2D* , 2> histosWW4 = {} ;
	{
		TH2D* histoS = new TH2D("histosWW4S" , ";m_{W};m_{rest}" , 400 , 0 , 200 , 400 , 0 , 200) ;
		TH2D* histoB = new TH2D("histosWW4B" , ";m_{W};m_{rest}" , 400 , 0 , 200 , 400 , 0 , 200) ;
		histoS->SetDirectory(0) ;
		histoB->SetDirectory(0) ;
		setStyle(histoS) ;
		setStyle(histoB) ;
		histosWW4[0] = histoS ;
		histosWW4[1] = histoB ;
	}

	std::array<TH2D* , 2> histos3Jets = {} ;
	{
		TH2D* histoS = new TH2D("histoRec3JetsS" , ";m_{W};m_{recoil}" , 400 , 0 , 200 , 400 , 0 , 200) ;
		TH2D* histoB = new TH2D("histoRec3JetsB" , ";m_{W};m_{recoil}" , 400 , 0 , 200 , 400 , 0 , 200) ;
		histoS->SetDirectory(0) ;
		histoB->SetDirectory(0) ;
		setStyle(histoS) ;
		setStyle(histoB) ;
		histos3Jets[0] = histoS ;
		histos3Jets[1] = histoB ;
	}

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
		histoZMass->SetLineColor(process.second) ;

		std::stringstream toto2 ; toto2 << process.first << "_recMass" ;
		TH1D* histoRecMass = new TH1D( toto2.str().c_str() , ";m_{recoil} (GeV);#normalized events" , 150 , 0 , 220 ) ;
		histoRecMass->SetDirectory(0) ;
		histoRecMass->SetLineColor(process.second) ;

		std::stringstream toto3 ; toto3 << process.first << "_cosThetaZ" ;
		TH1D* histoCosThetaZ = new TH1D( toto3.str().c_str() , ";cos_{#theta_{Z}};#normalized events" , 100 , 0 , 1 ) ;
		histoCosThetaZ->SetDirectory(0) ;
		histoCosThetaZ->SetLineColor(process.second) ;

		std::stringstream toto4 ; toto4 << process.first << "_MassVsEnergy" ;
		TH1D* histoMassVsEnergy = new TH1D( toto4.str().c_str() , ";m_{qq}/E_{qq};#normalized events" , 100 , 0 , 1 ) ;
		histoMassVsEnergy->SetDirectory(0) ;
		histoMassVsEnergy->SetLineColor(process.second) ;

		std::stringstream toto5 ; toto5 << process.first << "_y23" ;
		TH1D* histoy23 = new TH1D( toto5.str().c_str() , ";#-log10(y_{23});#normalized events" , 100 , 0 , 5 ) ;
		histoy23->SetDirectory(0) ;
		histoy23->SetLineColor(process.second) ;

		std::stringstream toto6 ; toto6 << process.first << "_y34" ;
		TH1D* histoy34 = new TH1D( toto6.str().c_str() , ";#-log10(y_{34});#normalized events" , 100 , 0 , 5 ) ;
		histoy34->SetDirectory(0) ;
		histoy34->SetLineColor(process.second) ;

		std::stringstream totoPt ; totoPt << process.first << "_Pt" ;
		TH1D* histoPt = new TH1D( totoPt.str().c_str() , ";P_{T} (GeV);#normalized events" , 100 , 0 , 150 ) ;
		histoPt->SetDirectory(0) ;
		histoPt->SetLineColor(process.second) ;

		std::stringstream totozPt ; totozPt << process.first << "_zPt" ;
		TH1D* histozPt = new TH1D( totozPt.str().c_str() , ";Z_{P_{T}} (GeV);#normalized events" , 100 , 0 , 150 ) ;
		histozPt->SetDirectory(0) ;
		histozPt->SetLineColor(process.second) ;

		std::stringstream toto2Jm ; toto2Jm << process.first << "_2jm" ;
		TH1D* histoMass2Jet = new TH1D( toto2Jm.str().c_str() , ";m_{2jets} (GeV);#normalized events" , 100 , 0 , 250 ) ;
		histoMass2Jet->SetDirectory(0) ;
		histoMass2Jet->SetLineColor(process.second) ;

		std::stringstream totocm ; totocm << process.first << "_cm" ;
		TH1D* histoCosThetaMiss = new TH1D( totocm.str().c_str() , ";|cos#theta_{miss}|;#normalized events" , 150 , 0 , 1 ) ;
		histoCosThetaMiss->SetDirectory(0) ;
		histoCosThetaMiss->SetLineColor(process.second) ;

		std::stringstream toto7 ; toto7 << process.first << "_recMass" ;
		TH1D* histoRecMassFinal = new TH1D( toto7.str().c_str() , ";m_{recoil} (GeV);n events" , 20 , 110 , 160 ) ;
		histoRecMassFinal->SetDirectory(0) ;

		if ( process.first == std::string("ZH") )
		{
			histoRecMassFinal->SetLineColor(process.second) ;
		}
		else
		{
			histoRecMassFinal->SetFillColor(process.second) ;
			histoRecMassFinal->SetLineColor(kBlack) ;
		}

		setStyle(histoZMass) ;
		setStyle(histoRecMass) ;
		setStyle(histoCosThetaZ) ;
		setStyle(histoMassVsEnergy) ;
		setStyle(histoy23) ;
		setStyle(histoy34) ;
		setStyle(histoPt) ;
		setStyle(histozPt) ;
		setStyle(histoMass2Jet) ;
		setStyle(histoCosThetaMiss) ;
		setStyle(histoRecMassFinal) ;

		zMassHistoMap.insert( {process.first , histoZMass} ) ;
		recMassHistoMap.insert( {process.first , histoRecMass} ) ;
		cosThetaZHistoMap.insert( {process.first , histoCosThetaZ} ) ;
		massVsEnergyHistoMap.insert( {process.first , histoMassVsEnergy} ) ;
		y23HistoMap.insert( {process.first , histoy23} ) ;
		y34HistoMap.insert( {process.first , histoy34} ) ;
		ptHistoMap.insert( {process.first , histoPt} ) ;
		zPtHistoMap.insert( {process.first , histozPt} ) ;
		mass2JetHistoMap.insert( {process.first , histoMass2Jet} ) ;
		cosThetaMissHistoMap.insert( {process.first , histoCosThetaMiss} ) ;
		recMassFinalHistoMap.insert( {process.first , histoRecMassFinal} ) ;
	}


	for ( const auto& process : subProcessMap )
	{
		std::stringstream toto ; toto << process.first << "_zMass" ;
		TH1D* histoZMass = new TH1D( toto.str().c_str() , ";m_{z} (GeV);#normalized events" , 80 , 30 , 150 ) ;
		histoZMass->SetDirectory(0) ;
		histoZMass->SetLineColor(process.second) ;

		std::stringstream toto2 ; toto2 << process.first << "_recMass" ;
		TH1D* histoRecMass = new TH1D( toto2.str().c_str() , ";m_{recoil} (GeV);#normalized events" , 80 , 0 , 220 ) ;
		histoRecMass->SetDirectory(0) ;
		histoRecMass->SetLineColor(process.second) ;

		std::stringstream toto2Jm ; toto2Jm << process.first << "_2jm" ;
		TH1D* histoMass2Jet = new TH1D( toto2Jm.str().c_str() , ";m_{2jets} (GeV);#normalized events" , 80 , 0 , 250 ) ;
		histoMass2Jet->SetDirectory(0) ;
		histoMass2Jet->SetLineColor(process.second) ;

		std::stringstream totoy23 ; totoy23 << process.first << "_y23" ;
		TH1D* histoy23 = new TH1D( totoy23.str().c_str() , ";#-log10(y_{23});#normalized events" , 80 , 0 , 5 ) ;
		histoy23->SetDirectory(0) ;
		histoy23->SetLineColor(process.second) ;

		std::stringstream totoy34 ; totoy34 << process.first << "_y34" ;
		TH1D* histoy34 = new TH1D( totoy34.str().c_str() , ";#-log10(y_{34});#normalized events" , 80 , 0 , 5 ) ;
		histoy34->SetDirectory(0) ;
		histoy34->SetLineColor(process.second) ;

		setStyle(histoZMass) ;
		setStyle(histoRecMass) ;
		setStyle(histoMass2Jet) ;
		setStyle(histoy23) ;
		setStyle(histoy34) ;

		zMassHistoSubMap.insert( {process.first , histoZMass} ) ;
		recMassHistoSubMap.insert( {process.first , histoRecMass} ) ;
		mass2JetHistoSubMap.insert( {process.first , histoMass2Jet} ) ;
		y23HistoSubMap.insert( {process.first , histoy23} ) ;
		y34HistoSubMap.insert( {process.first , histoy34} ) ;
	}

	zMassHistoSubMap.at("ZH")->SetFillColor(kRed-10) ;
	recMassHistoSubMap.at("ZH")->SetFillColor(kRed-10) ;
	mass2JetHistoSubMap.at("ZH")->SetFillColor(kRed-10) ;
	y23HistoSubMap.at("ZH")->SetFillColor(kRed-10) ;
	y34HistoSubMap.at("ZH")->SetFillColor(kRed-10) ;

	zMassHistoSubMap.at("ZH")->SetLineWidth(0) ;
	recMassHistoSubMap.at("ZH")->SetLineWidth(0) ;
	mass2JetHistoSubMap.at("ZH")->SetLineWidth(0) ;
	y23HistoSubMap.at("ZH")->SetLineWidth(0) ;
	y34HistoSubMap.at("ZH")->SetLineWidth(0) ;

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


		eventReader.setTree(tree) ;


		int iEntry = 0 ;
		while( tree->GetEntry( iEntry++ ) )
		{
			Event event = eventReader.getEvent(iEntry-1) ;

			event.processID = processID ;
			eventReader.processID = processID ;
			auto process = Process::getProcess(event) ;


			auto subProcessList = Process::getSubProcess(event) ;

			//
			if ( processID == 106485 || processID == 106486 )
				if ( event.decayID == 13 || event.decayID == 22 || event.decayID == 23 || event.decayID == 25 )
					continue ;

			if ( processID == 108063 || processID == 108064 )
				if ( event.subDecayID == 7 )
					continue ;

			auto cutVal = Cut::finalCut(event) ;
			if ( cutVal < 2 )
				continue ;

			//						if ( Cut::isInvis(event))
			//							continue ;



			double zMass = event.zMass ;
			double recMass = event.recMass ;
			double pt = std::sqrt(event.pMiss[0]*event.pMiss[0] + event.pMiss[1]*event.pMiss[1]) ;
			double zPt = event.zPt ;
			double cosThetaZ = event.cosThetaZ ;
			double massVsE = event.zMass/(event.z1e + event.z2e) ;
			if ( Cut::isInvis(event) )
			{
				zMass = event.zMassVec[2] ;
				recMass = event.recMassVec[2] ;
				zPt = event.zPtVec[2] ;
				cosThetaZ = event.cosThetaZVec[2] ;
				massVsE = event.mass2Jet/event.totalEnergy ;
			}

			for ( const auto& subProcess : subProcessList )
			{
				if ( subProcessMap.count(subProcess) )
				{
					zMassHistoSubMap.at( subProcess )->Fill(zMass , weight) ;
					recMassHistoSubMap.at( subProcess )->Fill(recMass , weight) ;
					mass2JetHistoSubMap.at( subProcess )->Fill(event.mass2Jet , weight) ;
					y23HistoSubMap.at( subProcess )->Fill(event.y23 , weight) ;
					y34HistoSubMap.at( subProcess )->Fill(event.y34 , weight) ;
				}
			}


			if ( signal.count(processID) )
			{
				zMassHistoSubMap.at( "ZH" )->Fill(zMass , weight) ;
				recMassHistoSubMap.at( "ZH" )->Fill(recMass , weight) ;
				mass2JetHistoSubMap.at( "ZH" )->Fill(event.mass2Jet , weight) ;


				y23HistoSubMap.at("ZH")->Fill(event.y23 , weight) ;
				y34HistoSubMap.at("ZH")->Fill(event.y34 , weight) ;


				histosZZ4[0]->Fill(event.zz12mass , event.zz34mass , weight) ;
				histosWW4[0]->Fill(event.ww12mass , event.ww34mass , weight) ;

				histos3Jets[0]->Fill(event.wwMass3 , event.wwRecMass3 , weight) ;

				if ( !Cut::isInvis(event) )
					thrustHistos[0]->Fill(event.majorThrust , event.minorThrust , weight) ;
				else
					thrustHistosInvis[0]->Fill(event.majorThrust , event.minorThrust , weight) ;
			}
			else
			{
				if ( zz4List.count(processID) )
				{
					if ( processID == 106575 || processID == 106576 )
					{
						if ( event.decayID%2 != 0 )
							histosZZ4[1]->Fill(event.zz12mass , event.zz34mass , weight) ;
					}
				}

				if ( ww4List.count(processID) )
					histosWW4[1]->Fill(event.ww12mass , event.ww34mass , weight) ;

				if ( qqlvList.count(processID) )
					histos3Jets[1]->Fill(event.wwMass3 , event.wwRecMass3 , weight) ;

				if ( qqList.count(processID) )
				{
					if ( !Cut::isInvis(event) )
						thrustHistos[1]->Fill(event.majorThrust , event.minorThrust , weight) ;
					else
						thrustHistosInvis[1]->Fill(event.majorThrust , event.minorThrust , weight) ;
				}
			}


			zMassHistoMap.at( process )->Fill(zMass , weight) ;
			recMassHistoMap.at( process )->Fill(recMass , weight) ;

			y23HistoMap.at( process )->Fill(event.y23 , weight) ;
			y34HistoMap.at( process )->Fill(event.y34 , weight) ;

			ptHistoMap.at( process )->Fill(pt , weight) ;
			zPtHistoMap.at( process )->Fill(zPt , weight) ;

			mass2JetHistoMap.at( process )->Fill(event.mass2Jet , weight) ;

			cosThetaMissHistoMap.at( process )->Fill(std::abs(event.cosThetaMiss) , weight) ;

			cosThetaZHistoMap.at( process )->Fill(std::abs(cosThetaZ) , weight) ;


			massVsEnergyHistoMap.at( process )->Fill(massVsE , weight) ;


			if ( cutVal == 9 )
				recMassFinalHistoMap.at( process )->Fill(recMass , weight) ;

		}

		file->Close() ;
	}

	TCanvas* c1 = drawCanvas("c1" , zMassHistoMap , polText , energy , true , {70,110}) ;
	TCanvas* c2 = drawCanvas("c2" , recMassHistoMap , polText , energy , true , {110,160}) ;
	TCanvas* c3 = drawCanvas("c3" , cosThetaZHistoMap , polText , energy , true , {0.9}) ;
	TCanvas* c4 = drawCanvas("c4" , massVsEnergyHistoMap , polText , energy , true , {0.8}) ;
	TCanvas* c5 = drawCanvas("c5" , y23HistoMap , polText , energy) ;
	TCanvas* c6 = drawCanvas("c6" , y34HistoMap , polText , energy) ;
	TCanvas* cPt = drawCanvas("cPt" , ptHistoMap , polText , energy) ;
	TCanvas* czPt = drawCanvas("czPt" , zPtHistoMap , polText , energy , true , {20}) ;
	TCanvas* czM2j = drawCanvas("czM2j" , mass2JetHistoMap , polText , energy) ;
	TCanvas* cCTM = drawCanvas("cCTM" , cosThetaMissHistoMap , polText , energy) ;

	TCanvas* c1s = drawCanvas("c1s" , zMassHistoSubMap , polText , energy , false) ;
	TCanvas* c2s = drawCanvas("c2s" , recMassHistoSubMap , polText , energy , false) ;
	TCanvas* c3s = drawCanvas("c3s" , mass2JetHistoSubMap , polText , energy , false , {110}) ;

	TCanvas* cy23s = drawCanvas("cy23s" , y23HistoSubMap , polText , energy , false ) ;
	TCanvas* cy34s = drawCanvas("cy34s" , y34HistoSubMap , polText , energy , false , {2.22}) ;

	TCanvas* c7 = new TCanvas("c7" , "c7" , 1000 , 1000) ;
	c7->cd() ;
	setStyle(c7) ;

	THStack* hs = new THStack("hs",";m_{recoil} (GeV);nEvents/2.5GeV") ;

	std::vector< std::string > chanVec = {"ZH" , "q#bar{q}" , "q#bar{q}q#bar{q}" , "q#bar{q}ll" , "q#bar{q}l#nu" , "q#bar{q}#nu#nu"} ;
	auto chanVecRev = chanVec ;
	std::reverse(chanVecRev.begin() , chanVecRev.end() ) ;

	for ( const auto& str : chanVecRev )
		hs->Add( recMassFinalHistoMap.at(str) ) ;

	hs->Draw("hist") ;
	setStyle(hs) ;

	c7->Modified() ;

	//legend
	TLegend* leg2 = new TLegend(0.65,0.65,0.88,0.88) ;
	leg2->SetBorderSize(0) ;
	for ( const auto& str : chanVec )
	{
		auto histo = recMassFinalHistoMap.at(str) ;
		auto le2 = leg2->AddEntry(histo , str.c_str() , "f") ;
		le2->SetTextColor( processMap.at(str) ) ;
	}
	leg2->Draw() ;

	addWIP(c7) ;
	addPolText(c7,polText) ;
	addLumiText(c7) ;
	addEnergyText(c7 , 250) ;


	TCanvas* c2DS = drawCanvas("c2DS" , zMassVsRecMassVec.at(0) , polText , energy) ;
	TCanvas* c2DB = drawCanvas("c2DB" , zMassVsRecMassVec.at(1) , polText , energy) ;

	TCanvas* cThrustS = drawCanvas("cThrustS" , thrustHistos.at(0) , polText , energy) ;
	TCanvas* cThrustB = drawCanvas("cThrustB" , thrustHistos.at(1) , polText , energy) ;

	drawSquare(cThrustS , 0 , 0 , 0.40 , 0.10) ;
	drawSquare(cThrustB , 0 , 0 , 0.40 , 0.10) ;

	TCanvas* cThrustInvS = drawCanvas("cThrustInvS" , thrustHistosInvis.at(0) , polText , energy) ;
	TCanvas* cThrustInvB = drawCanvas("cThrustInvB" , thrustHistosInvis.at(1) , polText , energy) ;

	TCanvas* cZZS = drawCanvas("cZZS" , histosZZ4.at(0) , polText , energy) ;
	TCanvas* cZZB = drawCanvas("cZZB" , histosZZ4.at(1) , polText , energy) ;

	drawEllipse(cZZS , 91.2 , 90 , 10) ;
	drawEllipse(cZZB , 91.2 , 90 , 10) ;

	TCanvas* cWWS = drawCanvas("cWWS" , histosWW4.at(0) , polText , energy) ;
	TCanvas* cWWB = drawCanvas("cWWB" , histosWW4.at(1) , polText , energy) ;

	drawEllipse(cWWS , 80.43 , 80 , 10) ;
	drawEllipse(cWWB , 80.43 , 80 , 10) ;

	TCanvas* c3JetsS = drawCanvas("c3JetsS" , histos3Jets.at(0) , polText , energy) ;
	TCanvas* c3JetsB = drawCanvas("c3JetsB" , histos3Jets.at(1) , polText , energy) ;

	drawSquare(c3JetsS , 75 , 60 , 85 , 160) ;
	drawSquare(c3JetsB , 75 , 60 , 85 , 160) ;


	TFile* outputFile = new TFile("test.root" , "RECREATE") ;
	outputFile->cd() ;


	c1->Write("c1") ;
	c2->Write("c2") ;
	c3->Write("c3") ;
	c4->Write("c4") ;
	c5->Write("c5") ;
	c6->Write("c6") ;
	c7->Write("c7") ;
	cPt->Write("cPt") ;
	czPt->Write("czPt") ;
	czM2j->Write("czM2j") ;
	c2DS->Write("c2DS") ;
	c2DB->Write("c2DB") ;
	cCTM->Write("cCTM") ;

	c1s->Write("c1s") ;
	c2s->Write("c2s") ;
	c3s->Write("c3s") ;

	cy23s->Write("cy23s") ;
	cy34s->Write("cy34s") ;

	cThrustS->Write("cThrustS") ;
	cThrustB->Write("cThrustB") ;
	cThrustInvS->Write("cThrustInvS") ;
	cThrustInvB->Write("cThrustInvB") ;

	cZZS->Write("cZZS") ;
	cZZB->Write("cZZB") ;

	cWWS->Write("cWWS") ;
	cWWB->Write("cWWB") ;

	c3JetsS->Write("c3JetsS") ;
	c3JetsB->Write("c3JetsB") ;

	outputFile->Close() ;

	return 0 ;
}
