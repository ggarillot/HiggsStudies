#ifndef ISR_h
#define ISR_h

#include <CLHEP/Vector/LorentzVector.h>

#include <vector>

struct ISR
{
		std::vector< CLHEP::HepLorentzVector > mcFourVec = {} ;
		std::vector< CLHEP::HepLorentzVector > fourVec = {} ;
} ;

#endif //ISR_h
