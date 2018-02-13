#ifndef Jet_h
#define Jet_h

#include <IMPL/ReconstructedParticleImpl.h>
#include <IMPL/MCParticleImpl.h>
#include <IMPL/LCRelationImpl.h>
#include <UTIL/LCRelationNavigator.h>

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>
#include <fastjet/ClusterSequence.hh>


class JetInfo : public fastjet::PseudoJet::UserInfoBase
{
	public :
		JetInfo() = default ;
		~JetInfo() = default ;

		void setMcOrigin(const std::map<int,float> _origin) { _mcOrigin = _origin ; }

		std::map<int,float> mcOrigin() const { return _mcOrigin ; }


		JetInfo(const JetInfo &toCopy) = delete ;
		void operator=(const JetInfo &toCopy) = delete ;

	protected :

		std::map<int,float> _mcOrigin = {} ;
} ;


class DiJet
{
	public :
		DiJet() ;
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
