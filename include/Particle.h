#ifndef Particle_h
#define Particle_h

#include <IMPL/ReconstructedParticleImpl.h>
#include <IMPL/MCParticleImpl.h>
#include <IMPL/LCRelationImpl.h>
#include <UTIL/LCRelationNavigator.h>

#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

#include <vector>


class ParticleInfo : public fastjet::PseudoJet::UserInfoBase
{
	public :
		ParticleInfo() ;
		~ParticleInfo() ;


		void setRecoParticle(IMPL::ReconstructedParticleImpl* toto) { _recoParticle = toto ; }
		void setOrigin(int toto) { _origin = toto ; }
		void setId(int toto) { _id = toto ; }



		inline IMPL::ReconstructedParticleImpl* recoParticle() const { return _recoParticle ; }
		inline int origin() const { return _origin ; }
		inline int id() const { return _id ; }

		ParticleInfo(const ParticleInfo &toCopy) = delete ;
		void operator=(const ParticleInfo &toCopy) = delete ;

	protected :

		IMPL::ReconstructedParticleImpl* _recoParticle = nullptr ;

		int _origin = 0 ;
		int _id = 0 ;

} ;


//typedef std::vector<Particle*> ParticleVec ;

#endif //Particle_h
