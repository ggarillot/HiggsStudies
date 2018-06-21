#ifndef Process_h
#define Process_h

#include <string>
#include <map>

#include "TColor.h"
#include "Event.h"

enum Polarisation
{
	LR,
	RL,
	LL,
	RR
} ;

struct Process
{
		Process() = default ;
		Process(std::string _name , Color_t _color)
			: name(_name),
			  color(_color)
		{}

		std::string name = "" ;
		Color_t color = kBlack ;

		std::map<Polarisation , unsigned int> nSelected = {} ;
		std::map<Polarisation , unsigned int> nTotal = {} ;

		static std::string getProcess(const Event& event) ;
		static std::string getSubProcess(const Event& event) ;
};

struct ChannelInfo
{
		int processID ;
		double xSect ;
		Polarisation pol ;
} ;

static const std::map<int , ChannelInfo> ChannelInfoMap =
{
	{ 106485 , {106485 , 346.01308 , LR} } ,
	{ 106486 , {106486 , 221.95154 , RL} } ,

	{ 106551 , {106551 , 14874.280 , LR} } ,
	{ 106552 , {106552 , 136.35687 , RL} } ,

	{ 106577 , {106577 , 18780.976 , LR} } ,
	{ 106578 , {106578 , 172.73264 , RL} } ,

	{ 106573 , {106573 , 1402.0553 , LR} } ,
	{ 106574 , {106574 , 604.97119 , RL} } ,

	{ 106575 , {106575 , 1422.1429 , LR} } ,
	{ 106576 , {106576 , 713.52633 , RL} } ,

	{ 106607 , {106607 , 129148.58 , LR} } ,
	{ 106608 , {106608 , 71272.814 , RL} } ,

	{ 106564 , {106564 , 9999.5172 , LR} } ,
	{ 106566 , {106566 , 85.616028 , RL} } ,
	{ 106563 , {106563 , 119.68261 , LL} } ,
	{ 106565 , {106565 , 119.28943 , RR} } ,

	{ 106560 , {106560 , 459.05527 , LR} } ,
	{ 106562 , {106562 , 316.5156  , RL} } ,
	{ 106559 , {106559 , 259.75634 , LL} } ,
	{ 106561 , {106561 , 258.95773 , RR} } ,

	{ 106571 , {106571 , 456.79981 , LR} } ,
	{ 106572 , {106572 , 130.78916 , RL} }
} ;

#endif //Process_h