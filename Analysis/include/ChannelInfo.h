#ifndef ChannelInfo_h
#define ChannelInfo_h

#include <map>

enum Pol
{
	LR,
	RL
} ;

struct ChannelInfo
{
		int processID ;
		double xSect ;
		Pol pol ;
} ;


static const std::map<int,ChannelInfo> ChannelInfoMap =
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
	{ 106608 , {106608 , 71272.814 , RL} }
} ;

#endif //ChannelInfo_h
