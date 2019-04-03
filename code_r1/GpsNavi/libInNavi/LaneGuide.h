#if !defined(AFX_LANEGUIDE_H__9950F05D_D3BE_4E93_8E3D_90AA9F2926D3__INCLUDED_)
#define AFX_LANEGUIDE_H__9950F05D_D3BE_4E93_8E3D_90AA9F2926D3__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"
#include "NuroDefine.h"
#include "NuroConstDefined.h"

#define ERRORDATA    -1

typedef struct tag_strLG_Head 
{
	nuLONG  TotalNTCnt;
	nuLONG  Bpoint;
	nuLONG  Reserve;
}LG_Head;

typedef struct tag_strLG_A_Data
{
	nuLONG  FromX;
	nuLONG  FromY;
	nuLONG  ToX;
	nuLONG  ToY;
	nuLONG  signCount : 8;
	nuLONG  SignPoint : 24;
}LG_A_Data;

typedef struct tag_strLG_B_Data
{
	nuBYTE LandGuide[MAX_LANE_COUNTS];
}LG_B_Data;

typedef struct tag_strLG
{
	LG_Head   Lg_Head;
	LG_A_Data *Lg_A_Data;
	LG_B_Data Lg_B_Data;
}LG;

class CLaneGuide
{
	public:
		CLaneGuide();
		virtual ~CLaneGuide();

		nuBOOL  InitClass();
		nuVOID  CloseClass();
		nuVOID  Release_LaneGuide();
		nuVOID  Init_LaneGuide_Struct();
		nuBOOL  Get_LaneGuideInfo( nuTCHAR *sFileName, nuroPOINT FromCoor, nuroPOINT ToCoor, nuLONG TurnAngle, 
			                       NOW_LAME_GUIDE *LightArray );
	private:
		nuBOOL  Read_LaneGuide(nuTCHAR *sFileName);
		nuBOOL  Check_LaneGuideFlag(nuroPOINT FromCoor, nuroPOINT ToCoor, nuLONG *LaneGuideIndex);
		nuLONG  DoubleCheckData(nuLONG AdataIndex, nuroPOINT FromCoor, nuroPOINT ToCoor);
		nuVOID  Judge_LG_Light(nuLONG TurnAngle, NOW_LAME_GUIDE *pLightArray);

	public:
		LG LaneGuideData;
		nuFILE *LGFile;
};

#endif
