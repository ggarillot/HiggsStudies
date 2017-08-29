#include "Jet.h"

DiJet::DiJet()
	: _jet1( fastjet::PseudoJet(0,0,0,0) ) ,
	  _jet2( fastjet::PseudoJet(0,0,0,0) ) ,
	  _diJet(_jet1 + _jet2)
{
}

DiJet::DiJet(const fastjet::PseudoJet& a , const fastjet::PseudoJet& b)
	: _jet1(a) ,
	  _jet2(b) ,
	  _diJet(a+b)
{

}

DiJet::~DiJet()
{

}

double DiJet::getCosAngleBetweenJets() const
{
	CLHEP::Hep3Vector p1( _jet1.px() , _jet1.py() , _jet1.pz() ) ;
	CLHEP::Hep3Vector p2( _jet2.px() , _jet2.py() , _jet2.pz() ) ;

	return ( p1.dot(p2) )/(p1.mag()*p2.mag()) ;
}
