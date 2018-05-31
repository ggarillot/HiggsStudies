#ifndef EVENTSHAPE
#define EVENTSHAPE

#include <Eigen/Dense>
#include <fastjet/PseudoJet.hh>
#include <CLHEP/Vector/ThreeVector.h>

#include <random>

class EventShape
{
	public :
		EventShape() = default ;
		~EventShape() = default ;

		void setPartList(const std::vector<fastjet::PseudoJet>& particles ) ;

		void setThMomPower(double tp) ;
		double getThMomPower() ;
		void setFast(int nf) ;
		int getFast() const ;

		CLHEP::Hep3Vector thrustAxis() const ;
		CLHEP::Hep3Vector majorAxis() const ;
		CLHEP::Hep3Vector minorAxis() const ;

		double thrust() const ;
		double majorThrust() const ;
		double minorThrust() const ;
		// thrust :: Corresponding thrust, major, and minor value.

		double oblateness() const ;

	private:
		static int nInt(double x) ;

		double ulAngle(double x, double y) ;
		double sign(double a, double b) ;
		void ludbrb(Eigen::MatrixXd& mom , double theta , double phi , double bx , double by , double bz) ;

		int iPow(int man, int exp) ;

		double m_dSphMomPower = 2 ;
		// PARU(41): Power of momentum dependence in sphericity finder.

		double m_dDeltaThPower = 0 ;
		// PARU(42): Power of momentum dependence in thrust finder.

		int m_iFast = 4 ;
		// MSTU(44): # of initial fastest particles choosen to start search.

		double m_dConv = 0.0001 ;
		// PARU(48): Convergence criteria for axis maximization.

		int m_iGood = 2 ;
		// MSTU(45): # different starting configurations that must
		// converge before axis is accepted as correct.

		Eigen::Matrix4d m_dAxes = {} ;
		// m_dAxes[1] is the Thrust axis.
		// m_dAxes[2] is the Major axis.
		// m_dAxes[3] is the Minor axis.


		std::mt19937_64 generator = std::mt19937_64() ;
		std::uniform_real_distribution<double> distribution = std::uniform_real_distribution<double>(0.0,1.0) ;

		std::array<double,4> m_dThrust = {} ;
		double m_dOblateness = 0 ;

		static unsigned int m_maxpart ;

} ;

#endif


