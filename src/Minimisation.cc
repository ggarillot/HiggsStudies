#include "Minimisation.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <limits>

#include <Minuit2/Minuit2Minimizer.h>
#include <Math/GSLMinimizer.h>
#include <Math/GSLSimAnMinimizer.h>
#include <Math/Functor.h>

using namespace std ;

Minimisation::Minimisation(unsigned int nParam_)
	: nParam(nParam_)
{
	bestParam = std::vector<double>(nParam , 0) ;
	limitsParam = std::vector<std::pair<double,double>>(nParam , {std::numeric_limits<double>::min() , std::numeric_limits<double>::max()}) ;
	nameParam = std::vector<std::string>(nParam , "") ;

	for ( unsigned int i = 0 ; i < nParam ; ++i )
	{
		stringstream toto ; toto << "param" << i ;
		nameParam[i] = toto.str() ;
	}
}

double Minimisation::eval(const std::vector<double>& param)
{
	return functionToMinimize( &param[0] ) ;
}


double Minimisation::minimize()
{
	assert( bestParam.size() == limitsParam.size() ) ;
	assert( bestParam.size() == nParam ) ;

//	cout << "Launching minimizing" << endl ;
	double prevMin = eval(bestParam) ;
//	cout << "prevMin : " << prevMin << endl ;

	ROOT::Minuit2::Minuit2Minimizer min ;
	min.SetMaxFunctionCalls(400000) ;
	min.SetMaxIterations(2) ;
    min.SetTolerance(1e-4) ;
    min.SetPrintLevel(0) ;

	ROOT::Math::Functor f(this , &Minimisation::functionToMinimize , nParam) ;

	min.SetFunction(f) ;

	for( unsigned int i = 0 ; i < nParam ; i++ )
		min.SetLimitedVariable(i , nameParam[i].c_str() , bestParam[i] , step , limitsParam[i].first , limitsParam[i].second ) ;


	min.Minimize() ;

	const double* xs = min.X() ;
	double newMin = functionToMinimize(xs) ;

	if (newMin < prevMin)
	{
		for(unsigned int i = 0 ; i < nParam ; i++)
			bestParam[i] = xs[i] ;
	}

    std::cout << min.Status() << std::endl ;
	return eval(bestParam) ;
}

void Minimisation::printParam() const
{
	std::cout << "\"parameters\": [" ;
	for (unsigned int i = 0 ; i < nParam ; i++ )
	{
		std::cout << bestParam[i] ;
		if ( i < nParam - 1 )
			std::cout << "," ;
	}
	std::cout << "]" << std::endl ;
}

void Minimisation::setParams(std::vector<double> values)
{
	assert( values.size() == nParam ) ;
	bestParam = values ;
}

ThrustAxisComputer::ThrustAxisComputer(const std::vector<fastjet::PseudoJet> particles)
	: Minimisation(2)
{
	for ( const auto& particle : particles )
		pVec.push_back( CLHEP::Hep3Vector( particle.px() , particle.py() , particle.pz() ) ) ;

    limitsParam[0] = {-1 , 1} ;
    limitsParam[1] = {-1 , 1} ;
}

double ThrustAxisComputer::functionToMinimize(const double* param)
{
//	double cp = std::cos(param[0]) ;
//	double ct = std::cos(param[1]) ;

//	double sp = std::sqrt(1-cp*cp) ;
//	double st = std::sqrt(1-ct*ct) ;

    double cp = param[0] ;
    double ct = param[1] ;

    double sp = std::sqrt(1-cp*cp) ;
    double st = std::sqrt(1-ct*ct) ;

    CLHEP::Hep3Vector n(cp*st , sp*st , ct) ;

	double sum = 0 ;

	for ( const auto& p : pVec )
		sum += p.dot(n) ;

	return -sum ;
}

MajorThrustAxisComputer::MajorThrustAxisComputer(const std::vector<fastjet::PseudoJet> particles , CLHEP::Hep3Vector ref)
	: Minimisation(2)
{
	for ( const auto& particle : particles )
		pVec.push_back( -( CLHEP::Hep3Vector( particle.px() , particle.py() , particle.pz() ).cross(ref) ).cross(ref) ) ;

	limitsParam[0] = {0 , 2*3.1415926536} ;
	limitsParam[1] = {0 , 3.1415926536} ;
}

double MajorThrustAxisComputer::functionToMinimize(const double* param)
{
	double cp = std::cos(param[0]) ;
	double ct = std::cos(param[1]) ;

	double sp = std::sqrt(1-cp*cp) ;
	double st = std::sqrt(1-ct*ct) ;

	CLHEP::Hep3Vector n(cp*st , sp*st , ct) ;

	double sum = 0 ;

	for ( const auto& p : pVec )
		sum += p.dot(n) ;

	return -sum ;
}
