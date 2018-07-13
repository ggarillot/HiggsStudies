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
	//ZH
	{ 106485 , {106485 , 346.01308 , LR} } ,
	{ 106486 , {106486 , 221.95154 , RL} } ,

	//WW->qqqq
	{ 106551 , {106551 , 14874.280 , LR} } ,
	{ 106552 , {106552 , 136.35687 , RL} } ,

	//WW->qqlv
	{ 106577 , {106577 , 18780.976 , LR} } ,
	{ 106578 , {106578 , 172.73264 , RL} } ,

	//ZZ->qqqq
	{ 106573 , {106573 , 1402.0553 , LR} } ,
	{ 106574 , {106574 , 604.97119 , RL} } ,

	//ZZ->qqlv
	{ 106575 , {106575 , 1422.1429 , LR} } ,
	{ 106576 , {106576 , 713.52633 , RL} } ,

	//qq
	{ 106607 , {106607 , 129148.58 , LR} } ,
	{ 106608 , {106608 , 71272.814 , RL} } ,

	//Wev->qqev
	{ 106564 , {106564 , 9999.5172 , LR} } ,
	{ 106566 , {106566 , 85.616028 , RL} } ,
	{ 106563 , {106563 , 119.68261 , LL} } ,
	{ 106565 , {106565 , 119.28943 , RR} } ,

	//Zee->qqee
	{ 106560 , {106560 , 459.05527 , LR} } ,
	{ 106562 , {106562 , 316.5156  , RL} } ,
	{ 106559 , {106559 , 259.75634 , LL} } ,
	{ 106561 , {106561 , 258.95773 , RR} } ,

	//Zvv->qqvv
	{ 106571 , {106571 , 456.79981 , LR} } ,
	{ 106572 , {106572 , 130.78916 , RL} }

	//	h->mu mu
	//	{ 108015 , {108015 , 0.07646889068 , LR} } ,
	//	{ 108016 , {108016 , 0.04905129034 , RL} }

	//	h->inv
	//	{ 108079 , {108079 , 0.36919595636 , LR} } ,
	//	{ 108080 , {108080 , 0.23682229318 , RL} }

	//	h->gam gam
	//	{ 108031 , {108031 , 0.795830084 , LR} } ,
	//	{ 108032 , {108032 , 0.510488542 , RL} }

	//	h->Z gam
	//	{ 108047 , {108047 , 0.536320274 , LR} } ,
	//	{ 108048 , {108048 , 0.344024887 , RL} }

	//	h->ZZ
	//	{ 108063 , {108063 , 9.238549236 , LR} } ,
	//	{ 108064 , {108064 , 5.926106118 , RL} }

} ;

struct HiggsDecayMode
{
		std::string name ;
		double xSect ;
		int decayID ;
		int subDecayID ;
};

static const std::map<std::string , HiggsDecayMode> higgsDecayMap =
{
	{ "H->bb"      , { "H->bb"      , 0.578  , 5  , 0} } ,
	{ "H->cc"      , { "H->cc"      , 0.027  , 4  , 0} } ,
	{ "H->gg"      , { "H->gg"      , 0.086  , 21 , 0} } ,
	{ "H->tau tau" , { "H->tau tau" , 0.064  , 15 , 0} } ,
	{ "H->WW"      , { "H->WW"      , 0.216  , 24 , 0} } ,
	{ "H->ZZ"      , { "H->ZZ"      , 0.027  , 23 , 0} } ,
	{ "H->gam gam" , { "H->gam gam" , 0.0023 , 22 , 0} } ,
	{ "H->Z gam"   , { "H->Z gam"   , 0.0016 , 25 , 0} } ,
	{ "H->inv"     , { "H->inv"     , 0.001  , 23 , 7} }
} ;

#endif //Process_h
