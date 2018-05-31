#ifndef Minimisation_h
#define Minimisation_h

#include <vector>
#include <string>

#include <fastjet/PseudoJet.hh>

class Minimisation
{
	public :
		Minimisation(unsigned int nParam_) ;
		virtual ~Minimisation() = default ;

		virtual double functionToMinimize(const double* param) = 0 ;
		double eval(const std::vector<double>& param) ;
		virtual double minimize() ;

		virtual void printParam() const ;

		enum Limits { kNoLimit , kNegative , kPositive } ;

		std::vector<double> getParams() const { return bestParam ; }

		void setParams(std::vector<double> values) ;


	protected :
		unsigned int nParam ;
		std::vector<double> bestParam = {} ;
		std::vector<Limits> limitsParam = {} ;
		std::vector<std::string> nameParam = {} ;
		double step = 0.2 ;

} ;

class ThrustAxisComputer : public Minimisation
{
	public :
		ThrustAxisComputer(const std::vector<fastjet::PseudoJet> _particles) ;

		double functionToMinimize(const double* param) ;

	protected :
		const std::vector<fastjet::PseudoJet>* particles = nullptr ;


} ;

#endif // Minimisation_h
