#ifndef Minimisation_h
#define Minimisation_h

#include <vector>
#include <string>

#include <fastjet/PseudoJet.hh>
#include <CLHEP/Vector/ThreeVector.h>

class Minimisation
{
	public :
		Minimisation(unsigned int nParam_) ;
		virtual ~Minimisation() = default ;

		virtual double functionToMinimize(const double* param) = 0 ;
		double eval(const std::vector<double>& param) ;
		virtual double minimize() ;

		virtual void printParam() const ;

		std::vector<double> getParams() const { return bestParam ; }

		void setParams(std::vector<double> values) ;


	protected :
		unsigned int nParam ;
		std::vector<double> bestParam = {} ;
		std::vector<std::pair<double,double>> limitsParam = {} ;
		std::vector<std::string> nameParam = {} ;
		double step = 0.2 ;

} ;

class ThrustAxisComputer : public Minimisation
{
	public :
		ThrustAxisComputer(const std::vector<fastjet::PseudoJet> particles) ;

		double functionToMinimize(const double* param) ;

	protected :
		std::vector<CLHEP::Hep3Vector> pVec = {} ;


} ;

class MajorThrustAxisComputer : public Minimisation
{
	public :
		MajorThrustAxisComputer(const std::vector<fastjet::PseudoJet> particles , CLHEP::Hep3Vector ref) ;

		double functionToMinimize(const double* param) ;

	protected :
		std::vector<CLHEP::Hep3Vector> pVec = {} ;

} ;

#endif // Minimisation_h
