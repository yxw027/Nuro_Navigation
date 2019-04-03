
#ifndef COMMONSTRUCT_CONSTRUCTED_BY_DANIEL
#define COMMONSTRUCT_CONSTRUCTED_BY_DANIEL

#include "NuroDefine.h"

#define _MAX_ICONNUM		5
#define _MAX_KMNAME		   50
#define _MAX_ROADNAME	   50
#define _MAX_CITYNAME	   12
#define _MAX_ROADLISTNAME 100
#define _LANEGUIDENUM	   16
#define _MAX_CITYTOWN	   52
#define _MAX_POINAME	   68
#define _MAX_NEXTROADLIST   3
#define _MAX_FULLLAME	  512
#define _MAX_DRAW_POLYGON_COUNT 30
#define _NURO_DEF_MAX_STAR_NUMBER 48

/*typedef  struct tag_NURO_Next_Road_Info
{
		wchar_t	 wsRoadName[_MAX_ROADNAME];
		nuULONG	 RoadState;
		nuULONG	 lIcontype;
		nuULONG	 lDistance;
}NURO_Next_Road_Info;*/

typedef struct tag_NURO_NaviList
{
	nuUSHORT nIconType;
	wchar_t  wsIconNum[_MAX_ICONNUM];
	wchar_t	 wsRoadName[_MAX_ROADLISTNAME];
	nuULONG  lDistance;
}NURO_NaviList;

typedef  struct tag_NURO_Lane_Guide_Info
{
	nuULONG	  lLaneCount;
	nuBOOL	  bLaneEnable[_LANEGUIDENUM];
	nuSHORT   nLaneID[_LANEGUIDENUM];
}NURO_Lane_Guide_Info;

typedef  struct tag_NURO_Total_Navi_Info
{
	nuULONG	  lNextRoadCount;	
	nuUSHORT  nIconType;
	wchar_t	  wsRoadName[_MAX_ROADLISTNAME];
	nuULONG	  lDistance;
	nuULONG	  lDisToTarget;
	nuULONG	  lTimeToTarget;
	nuULONG   lRealCrossdDis;
	nuULONG	  lRealID;
      //wchar_t   wsRealCrossdName[_MAX_ROADLISTNAME];
	nuULONG	  lPassNum;
	nuULONG   RoadListNum;
	nuULONG   RoutingListCount;
	NURO_NaviList RoadList[_MAX_NEXTROADLIST];
	NURO_Lane_Guide_Info LaneGuideData;
}NURO_Total_Navi_Info;

typedef struct tag_NURO_Satellite_DATA
{
	nuSHORT	  nStarID;
	nuSHORT	  nAngle_elevation;
	nuSHORT	  nSignalNoiseRatio;
	nuSHORT   nAngle_position;
}NURO_Satellite_DATA, *PNURO_Satellite_DATA;

typedef struct tag_NURO_GPS_DATA
{
	nuLONG	 nLatitude;
	nuLONG	 nLongitude;
	nuSHORT	 nAngle;
	nuSHORT	 nSpeed;
	nuSHORT	 nYear;
	nuBYTE	 nMonth;
	nuBYTE	 nDay;
	nuBYTE	 nHours;
	nuBYTE	 nMinutes;
	nuBYTE	 nSeconds;
	nuBYTE	 nStarNumUsed;
	nuBYTE	 nStatus;
	nuBYTE	 nActiveStarNum;
	nuLONG	 nAntennaHeight;
	nuULONG  nReciveTimes;
	NURO_Satellite_DATA pStarList[_NURO_DEF_MAX_STAR_NUMBER];
}NURO_GPS_DATA, *PNURO_GPS_DATA;

typedef  struct tag_NURO_DrawMapInfo
{
 	nuBOOL 	 bCCD;	
	nuBOOL   bShowPic;
	nuBYTE   nSpeedLimit;
	nuLONG	 CarOnMapID;
	nuLONG	 CarOnCityID;		
	nuLONG	 CarOnTownID;
	nuLONG	 CenterOnMapID;
	nuLONG	 CenterOnCityID;
	nuLONG	 CenterOnTownID;
      	nuLONG   ptCurrentCarX;
	nuLONG   ptCurrentCarY;
	nuLONG   ptMapCarX;
	nuLONG   ptMapCarY;
	nuLONG	 ptMapCenterX;
	nuLONG	 ptMapCenterY;
	nuLONG   lMapAngle;
	nuLONG   lCarAngle;
	nuULONG	 lCCDSpeed;
	nuULONG  nDirectionAngle;
	nuULONG	 nScaleValue;
	nuTCHAR	 tsMapPath[128];
	nuLONG	 lDate;
	nuLONG   lTime;
	nuLONG   dcourse;
	nuLONG	 lGPSX;
	nuLONG	 lGPSY;
	nuLONG	 lSpeed;
	nuLONG   nGPSState;
	nuLONG	 nRoadDis;
	nuLONG   nRdDx;
	nuLONG   nRdDy;
	wchar_t  wsKiloName[_MAX_KMNAME];
	wchar_t	 wsCityName[_MAX_CITYNAME];
	wchar_t	 wsRoadName[_MAX_ROADNAME];
	wchar_t	 wsCenterPoi[_MAX_POINAME];
}NURO_DrawMapInfo;

typedef  struct tag_NURO_DrawInfo
{
	nuBOOL     		  bNaviState;
	nuBOOL	 		  bRouting;
	nuBYTE			  nMoveState;
	NURO_DrawMapInfo   	  DrawMapInfo;
	NURO_Total_Navi_Info 	  DrawNaviInfo;
}NURO_DrawInfo;
//----------------------------------------------------------------------------------------------------------------------------------//

/*typedef struct tag_StrNaviKingConfig
{
	nuBYTE	nOverSpeed;
	nuBYTE	nReRouteTime;
	nuWORD	nReRouteDis;
	nuWORD	nLowCCDDis;
	nuWORD	nHighCCDDis;
	nuWORD	n3DAngle;	
	nuWORD	nReseve;						
}StrNaviKingConfig;*/
#endif




