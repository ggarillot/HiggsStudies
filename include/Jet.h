#ifndef Jet_h
#define Jet_h

#include <IMPL/ReconstructedParticleImpl.h>
#include <IMPL/MCParticleImpl.h>
#include <IMPL/LCRelationImpl.h>
#include <UTIL/LCRelationNavigator.h>

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>
#include <fastjet/ClusterSequence.hh>


class Jet
{
	public :

	protected :

} ;


class DiJet
{
	public :
		DiJet(const fastjet::PseudoJet& a , const fastjet::PseudoJet& b) ;
		~DiJet() ;

		double getCosAngleBetweenJets() const ;

		fastjet::PseudoJet jet1() const { return _jet1 ; }
		fastjet::PseudoJet jet2() const { return _jet2 ; }
		fastjet::PseudoJet diJet() const { return _diJet ; }

	protected :
		fastjet::PseudoJet _jet1 ;
		fastjet::PseudoJet _jet2 ;

		fastjet::PseudoJet _diJet ;

} ;


#endif //Jet_h
