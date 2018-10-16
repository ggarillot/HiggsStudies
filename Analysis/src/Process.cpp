#include "Process.h"


Process::Process(std::string _name , Color_t _color)
	: name(_name),
	  color(_color)
{
	for ( const auto& pol : {LL,LR,RL,RR} )
	{
		nSelected.insert( { pol , 0 } ) ;
		nTotal.insert( { pol , 0 } ) ;
	}
}

std::string Process::getProcess(const Event& event)
{
	const std::set<int> signal = { 106485 , 106486 , 108015 , 108016 , 108079 , 108080 , 108031 , 108032 , 108047 , 108048 , 108063 , 108064 , 106525 , 106526 } ;
	const std::set<int> qqqq = { 106551 , 106552 , 106573 , 106574 } ;
	const std::set<int> qqlv = { 106577 , 106578 , 106563 , 106564 , 106565 , 106566 , 250018 , 250020 } ;
	const std::set<int> qq = { 106607 , 106608 , 250114 , 250116 } ;

	if ( signal.count(event.processID) )
		return std::string("ZH") ;
	else if ( qqqq.count(event.processID) )
		return std::string("q#bar{q}q#bar{q}") ;
	else if ( qqlv.count(event.processID) )
		return std::string("q#bar{q}l#nu") ;
	else if ( qq.count(event.processID) )
		return std::string("q#bar{q}") ;
	else if ( event.processID == 106575 || event.processID == 106576 )
	{
		if ( event.decayID%2 == 0 )
			return std::string("q#bar{q}#nu#nu") ;
		else
			return std::string("q#bar{q}ll") ;
	}
	else if ( event.processID == 106560 || event.processID == 106562 || event.processID == 106559 || event.processID == 106561 )
		return std::string("q#bar{q}ll") ;
	else if ( event.processID == 106571 || event.processID == 106572 )
		return std::string("q#bar{q}#nu#nu") ;
	else
	{
		std::cout << "Process::Error in getProcess : processID = " << event.processID << std::endl ;
		throw ;
	}
}

std::vector<std::string> Process::getSubProcess(const Event& event)
{
	const std::set<int> signal = { 106485 , 106486 , 108015 , 108016 , 108079 , 108080 , 108031 , 108032 , 108047 , 108048 , 108063 , 108064 , 106525 , 106526 } ;

	if ( !signal.count(event.processID) )
		return {} ;
//		throw ;

	if ( event.decayID == 1 )
		return { std::string("H->q#bar{q}/gg") , std::string("H->d#bar{d}") } ;
	else if ( event.decayID == 3 )
		return { std::string("H->q#bar{q}/gg") , std::string("H->s#bar{s}") } ;
	else if ( event.decayID == 4 )
		return { std::string("H->q#bar{q}/gg") , std::string("H->c#bar{c}") } ;
	else if ( event.decayID == 5 )
		return { std::string("H->q#bar{q}/gg") , std::string("H->b#bar{b}") } ;
	else if ( event.decayID == 11 )
		return { std::string("H->ee") } ;
	else if ( event.decayID == 13 )
		return { std::string("H->#mu#mu") } ;
	else if ( event.decayID == 15 )
		return { std::string("H->#tau#tau") } ;
	else if ( event.decayID == 21 )
		return { std::string("H->q#bar{q}/gg") , std::string("H->gg") } ;
	else if ( event.decayID == 22 )
		return { std::string("H->#gamma#gamma") } ;
	else if ( event.decayID == 23 )
	{
		if ( event.subDecayID == 1 )
			return { std::string("H->ZZ") , std::string("H->ZZ->qqqq") } ;
		else if ( event.subDecayID > 20 && event.subDecayID < 24 )
			return { std::string("H->ZZ") , std::string("H->ZZ->qqll") } ;
		else if ( event.subDecayID == 4 )
			return { std::string("H->ZZ") , std::string("H->ZZ->qqvv") } ;
		else if ( event.subDecayID > 500 && event.subDecayID < 540 )
			return { std::string("H->ZZ") , std::string("H->ZZ->llll") } ;
		else if ( event.subDecayID > 600 )
			return { std::string("H->ZZ") , std::string("H->ZZ->llvv") } ;
		else if ( event.subDecayID == 7 )
			return { std::string("H->ZZ") , std::string("H->ZZ->#nu#nu#nu#nu") } ;
		else
			throw ;
	}
	else if ( event.decayID == 24 )
	{
		if ( event.subDecayID == 1 )
			return { std::string("H->WW") , std::string("H->WW->qqqq") } ;
		else if ( event.subDecayID > 30 && event.subDecayID < 34 )
			return { std::string("H->WW") , std::string("H->WW->qqlv") } ;
		else if ( event.subDecayID > 600 )
			return { std::string("H->WW") , std::string("H->WW->lvlv") } ;
		else
			throw ;
	}
	else if ( event.decayID == 25 )
		return { std::string("H->Z#gamma") } ;
	else
	{
		std::cout << "getSubProcess error : " << event.processID << " " << event.decayID << " " << event.subDecayID << std::endl ;
		throw ;
	}
}
