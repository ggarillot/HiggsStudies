#include <iostream>
#include <string>

#include "json.hpp"

#include "Event.h"

#include "TFile.h"
#include "TTree.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TH1.h"
#include "THStack.h"


std::vector<Event> getEventsFromFile(std::string fileName)
{
	std::vector<Event> eventList = {} ;

	TFile* file = new TFile(fileName.c_str() , "READ") ;
	TTree* tree = dynamic_cast<TTree*>( file->Get("tree") ) ;

	EventReader eventReader ;
	eventReader.setTree(tree) ;

	eventList.reserve( static_cast<unsigned int>( tree->GetEntries() ) ) ;
	for ( unsigned int iEntry = 0 ; iEntry < tree->GetEntries() ; ++iEntry )
	{
		Event event = eventReader.getEvent(iEntry) ;
		eventList.push_back(event) ;
	}

	return eventList ;
}

enum SignalOrBg { kBackground , kSignal } ;

struct Channel
{
		Channel(std::string _name , SignalOrBg _status , Color_t _color)
		{
			name = _name ; status = _status , color = _color ;
		}

		std::string name = "" ;
		SignalOrBg status = kBackground ;
		Color_t color = kBlack ;
} ;

bool cut(const Event& event)
{
	bool wwCut = (event.ww12mass - 80.34)*(event.ww12mass - 80.34) + (event.ww34mass - 80.34)*(event.ww34mass - 80.34) < 120 ;
	bool zzCut = (event.zz12mass - 91.2)*(event.zz12mass - 91.2) + (event.zz34mass - 91.2)*(event.zz34mass - 91.2) < 120 ;

//	return wwCut && zzCut ;
	return wwCut ;
}

int main(int argc , char** argv)
{
	std::string dir = "/home/garillot/Code/HiggsStudies" ;

	std::vector< std::pair<Channel , std::vector<Event>>> channelMap = {} ;

	auto ZH = getEventsFromFile(dir + std::string("/ZH.root") ) ;
	auto WW = getEventsFromFile(dir + std::string("/WW.root") ) ;
	auto ZZ = getEventsFromFile(dir + std::string("/ZZ.root") ) ;

	Channel cZH = { std::string("ZH") , kSignal , kBlack } ;
	Channel cWW = { std::string("WW") , kBackground , kRed } ;
	Channel cZZ = { std::string("ZZ") , kBackground , kBlue } ;

	channelMap.push_back( {cZH , ZH} ) ;
	channelMap.push_back( {cWW , WW} ) ;
	channelMap.push_back( {cZZ , ZZ} ) ;


	TCanvas* c1 = new TCanvas("c1" , "c1" , 800 , 800) ;

	THStack* stack = new THStack("stack" , "") ;
	for ( const auto& channel : channelMap )
	{
		TH1* histo = new TH1D(channel.first.name.c_str() , channel.first.name.c_str() , 125 , 0 , 250) ;
		for (const auto& event : channel.second)
		{
			if ( !cut(event) )
				continue ;

			histo->Fill(event.recMass) ;
		}

		histo->Scale( 1.0/histo->Integral() ) ;
		histo->SetLineColor(channel.first.color) ;

		stack->Add(histo , "HIST") ;
	}

	stack->Draw("nostack") ;

	TFile* file = new TFile("test.root" , "RECREATE") ;
	file->cd() ;
	c1->Write("c1") ;
	file->Close() ;

	return 0 ;
}
