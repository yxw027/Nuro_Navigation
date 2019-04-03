// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:52:00 $
// $Revision: 1.2 $
#ifndef MATHTOOL_STRUCTURE
	#define MATHTOOL_STRUCTURE
	#include "NuroDefine.h"

	typedef struct tagUSERCFG
	{
		nuUCHAR    Voice; 
		nuBOOL	   DayNightMode;
		nuUCHAR    CrossWayAmpView;
		nuUCHAR    LenUnit;
		nuUSHORT   ClosePOIDis_C;
		nuUSHORT   ClosePOIDis_E;
		nuUCHAR    OverSpeedSet_C;
		nuUCHAR    OverSpeedSet_E;
		nuUCHAR    RuteingMode;
		nuBOOL	   BirdViewMode;//same to Run3D;
		nuUCHAR    FreewayShowMode;
		nuUCHAR    Language;
		nuUCHAR    Luminosity;
		nuUCHAR    Contradistinction;
		nuBOOL     IfFixOnRoad;
		nuBOOL     Pastport;
		nuBOOL     Money;
		nuBOOL     Carpoor;
		nuWORD     Screen_L;
		nuWORD     Screen_T;
		nuWORD     Screen_R;
		nuWORD     Screen_B;
		nuBOOL     b_EuroChar;
		nuBYTE     cAutoZoom; 
		nuBOOL     bCCDVoice;
		nuBOOL     bShowWaitTime;
		nuWORD     wTotalWaitTime;
		nuUCHAR    Reserve[94];
	}USERCFG, *PUSERCFG;

#endif