
#include "LaneGuide.h"
#include "NuroDefine.h"
#include "NuroNaviConst.h"

CLaneGuide::CLaneGuide()
{
	InitClass();
}

CLaneGuide::~CLaneGuide()
{
	CloseClass();
}

nuBOOL CLaneGuide::InitClass()
{
	LGFile = NULL;
	nuMemset(LaneGuideData.Lg_B_Data.LandGuide, 0, sizeof(nuBYTE) * MAX_LANE_COUNTS);
	Init_LaneGuide_Struct();
	return true;
}

nuVOID CLaneGuide::Init_LaneGuide_Struct()
{
	LaneGuideData.Lg_A_Data = NULL;
}

nuVOID CLaneGuide::CloseClass()
{
	Release_LaneGuide();
}

nuVOID CLaneGuide::Release_LaneGuide()
{
	if(NULL != LaneGuideData.Lg_A_Data)
	{
		delete [] LaneGuideData.Lg_A_Data;
		LaneGuideData.Lg_A_Data = NULL;
	}
	nuMemset(LaneGuideData.Lg_B_Data.LandGuide, 0, sizeof(nuBYTE) * MAX_LANE_COUNTS);
	if(NULL != LGFile)
	{
		nuFclose(LGFile);
		LGFile = NULL;
	}
}

nuBOOL CLaneGuide::Read_LaneGuide(nuTCHAR *sFileName)
{
	LGFile = nuTfopen( sFileName, NURO_FS_RB);
	if( LGFile == NULL )
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "LGFile %s",sFileName);
		return nuFALSE;
	}
	else
	{
		if(1 != nuFread(&LaneGuideData.Lg_Head, sizeof(LG_Head), 1, LGFile))
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "LGFile fail 1");
			return nuFALSE;
		}
		LaneGuideData.Lg_A_Data = new LG_A_Data[LaneGuideData.Lg_Head.TotalNTCnt];
		if(NULL == LaneGuideData.Lg_A_Data)
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "LGFile fail 2");
			return nuFALSE;
		}
		if(NULL != nuFseek(LGFile, sizeof(LG_Head), NURO_SEEK_SET))
		{	
			__android_log_print(ANDROID_LOG_INFO, "navi", "LGFile fail 3");
			return nuFALSE;
		}
		if(LaneGuideData.Lg_Head.TotalNTCnt != nuFread(LaneGuideData.Lg_A_Data, sizeof(LG_A_Data), LaneGuideData.Lg_Head.TotalNTCnt, LGFile))
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "LGFile fail 4");
			return nuFALSE;
		}
	}
}

nuBOOL CLaneGuide::Check_LaneGuideFlag(nuroPOINT FromCoor, nuroPOINT ToCoor, nuLONG *LaneGuideIndex)
{
	nuLONG lMax = LaneGuideData.Lg_Head.TotalNTCnt - 1;
	nuLONG lMin = 0;
	nuLONG lMid = 0;

	while((lMax - lMin) > 1) 
	{
		lMid = (lMax + lMin) / 2;
		if(LaneGuideData.Lg_A_Data[lMid].FromX > FromCoor.x)
		{
			lMax = lMid;
		}
		else if(LaneGuideData.Lg_A_Data[lMid].FromX < FromCoor.x)
		{
			lMin = lMid;
		}
		else
		{
			if(LaneGuideData.Lg_A_Data[lMid].FromY > FromCoor.y)
			{
				lMax = lMid;
			}
			else if(LaneGuideData.Lg_A_Data[lMid].FromY > FromCoor.y)
			{
				lMin = lMid;
			}
			else
			{
				*LaneGuideIndex = DoubleCheckData(lMid, FromCoor, ToCoor);
				return nuTRUE;
			}
		}
		if((lMax - lMin) <= 1)
		{
			break;
		}
	}
	return nuTRUE;
}

nuLONG CLaneGuide::DoubleCheckData(nuLONG AdataIndex, nuroPOINT FromCoor, nuroPOINT ToCoor)
{
	nuLONG Index = AdataIndex;
	while(Index > 0)
	{//往前找
		if(LaneGuideData.Lg_A_Data[Index].FromX == FromCoor.x && LaneGuideData.Lg_A_Data[Index].FromY == FromCoor.y)
		{
			if(LaneGuideData.Lg_A_Data[Index].ToX == ToCoor.x && LaneGuideData.Lg_A_Data[Index].ToY == ToCoor.y)
			{
				return Index;
			}
			Index --;
		}
		else
		{
			break;
		}
	}
	while(AdataIndex < LaneGuideData.Lg_Head.TotalNTCnt)
	{
		if(LaneGuideData.Lg_A_Data[AdataIndex].FromX == FromCoor.x && LaneGuideData.Lg_A_Data[AdataIndex].FromY == FromCoor.y)
		{
			if(LaneGuideData.Lg_A_Data[AdataIndex].ToX == ToCoor.x && LaneGuideData.Lg_A_Data[AdataIndex].ToY == ToCoor.y)
			{
				return AdataIndex;
			}
			AdataIndex ++;
		}
		else
		{
			break;
		}
	}
	return ERRORDATA;
}

nuBOOL CLaneGuide::Get_LaneGuideInfo(nuTCHAR *sFileName, nuroPOINT FromCoor, nuroPOINT ToCoor, nuLONG TurnFlag, NOW_LAME_GUIDE *pLightArray)
{
	nuLONG LaneGuideIndex = ERRORDATA;
	if(NULL == LGFile)
	{
		if(!Read_LaneGuide(sFileName))
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "Read_LaneGuide");
			return nuFALSE;
		}
	}
	if(LaneGuideData.Lg_A_Data == NULL || LaneGuideData.Lg_Head.TotalNTCnt == 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "LaneGuideData.Lg_A_Data == NULL || LaneGuideData.Lg_Head.TotalNTCnt == 0");
		return nuFALSE;
	}
	if(Check_LaneGuideFlag(FromCoor, ToCoor, &LaneGuideIndex) && LaneGuideIndex != ERRORDATA)
	{
		long LaneGuideAddr  = LaneGuideData.Lg_A_Data[LaneGuideIndex].SignPoint;
		long LaneGuideCount = LaneGuideData.Lg_A_Data[LaneGuideIndex].signCount;
		if(LaneGuideAddr == ERRORDATA || LaneGuideCount == ERRORDATA)
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "if(LaneGuideAddr == ERRORDATA || LaneGuideCount == ERRORDATA)");
			return nuFALSE;
		}
		nuMemset(LaneGuideData.Lg_B_Data.LandGuide, 0, sizeof(nuBYTE) * MAX_LANE_COUNTS);
		if(NULL != nuFseek(LGFile, LaneGuideData.Lg_A_Data[LaneGuideIndex].SignPoint, NURO_SEEK_SET))
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "NULL != nuFseek(LGFile, LaneGuideData.Lg_A_Data[LaneGuideIndex].SignPoint, NURO_SEEK_SET)");
			return nuFALSE;
		}
		if(LaneGuideCount != nuFread(LaneGuideData.Lg_B_Data.LandGuide, sizeof(nuBYTE), LaneGuideCount,LGFile))
		{
			__android_log_print(ANDROID_LOG_INFO, "navi", "LaneGuideCount != nuFread(LaneGuideData.Lg_B_Data.LandGuide, sizeof(nuBYTE), LaneGuideCount,LGFile)");
			return nuFALSE;
		}
		nuMemset(pLightArray->bLight, false, sizeof(nuBOOL) * MAX_LANE_COUNTS);
		pLightArray->LaneCount = LaneGuideCount;
		nuMemcpy(pLightArray->LightNum , LaneGuideData.Lg_B_Data.LandGuide, sizeof(nuBYTE) * MAX_LANE_COUNTS);
		if(pLightArray->LaneCount > 1)
		{
			Judge_LG_Light(TurnFlag, pLightArray);	
		}	
		else
		{
			pLightArray->LaneCount = 0;
		}	
	}	
	return nuTRUE;
}

nuVOID CLaneGuide::Judge_LG_Light(nuLONG TurnFlag, NOW_LAME_GUIDE *pLightArray)
{
	nuINT nDirect = 0;
	nuINT Enable = 0;
	/*if(TurnFlag == T_No || TurnFlag == T_Afore)
	{//直行
		nDirect = 3;
	}
	if(TurnFlag == T_Left || TurnFlag == T_LeftSide || TurnFlag == T_LUturn)
	{//左轉或靠左
		nDirect = 12;
	}
	if(TurnFlag == T_Right || TurnFlag == T_RightSide || TurnFlag == T_RUturn)
	{//右轉或靠右
		nDirect = 45;
	}*/
     	__android_log_print(ANDROID_LOG_INFO, "LaneGuide", "TurnFlag %d, LaneCount %d", TurnFlag, pLightArray->LaneCount);
	nuLONG i = 0;
	for(; i < pLightArray->LaneCount; i++)
	{
		switch(TurnFlag)
		{
		case T_No:
			if(pLightArray->LightNum[i] == '2' ||
			   pLightArray->LightNum[i] == '3'||
			   pLightArray->LightNum[i] == '4'||
			   pLightArray->LightNum[i] == '7'||
			   pLightArray->LightNum[i] == 'C'||
			   pLightArray->LightNum[i] == 'F'||
			   pLightArray->LightNum[i] == 'H'||
			   pLightArray->LightNum[i] == 'I'||
			   pLightArray->LightNum[i] == 'M'||
			   pLightArray->LightNum[i] == 'Z')
			{
				pLightArray->bLight[i] = nuTRUE;
				Enable++;
			}
			break;
		case T_Afore:
			if( i+1 < pLightArray->LaneCount && pLightArray->LightNum[i] != 3 && pLightArray->LightNum[i+1] != 3 && pLightArray->LaneCount == 2)
			{//單純針對直行，多車道只亮直行燈 雙車道非直行不顯示 2012.03.30 By shyanx
				pLightArray->LaneCount = 0;
			}
			if(pLightArray->LightNum[i] == '2'||
			   pLightArray->LightNum[i] == '3'||
			   pLightArray->LightNum[i] == '4'||
			   pLightArray->LightNum[i] == '7'||
			   pLightArray->LightNum[i] == 'C'||
			   pLightArray->LightNum[i] == 'F'||
			   pLightArray->LightNum[i] == 'H'||
			   pLightArray->LightNum[i] == 'I'||
			   pLightArray->LightNum[i] == 'M'||
			   pLightArray->LightNum[i] == 'Z')
			{
				pLightArray->bLight[i] = nuTRUE;
				Enable++;
			}
			
			break;
		case T_Left:
			if(pLightArray->LightNum[i] == '1'||
			   pLightArray->LightNum[i] == '2'||
			   pLightArray->LightNum[i] == '6'||
			   pLightArray->LightNum[i] == '7'||
			   pLightArray->LightNum[i] == 'E'||
			   pLightArray->LightNum[i] == 'J'||
			   pLightArray->LightNum[i] == 'M'||
			   pLightArray->LightNum[i] == 'N'||
			   pLightArray->LightNum[i] == 'Z')
			{
				pLightArray->bLight[i] = nuTRUE;
				Enable++;
			}
			break;
		case T_LeftSide:
			if(pLightArray->LightNum[i] == '1'||
			   pLightArray->LightNum[i] == '2'||
			   pLightArray->LightNum[i] == '6'||
			   pLightArray->LightNum[i] == '7'||
			   pLightArray->LightNum[i] == 'E'||
			   pLightArray->LightNum[i] == 'J'||
			   pLightArray->LightNum[i] == 'M'||
			   pLightArray->LightNum[i] == 'N'||
			   pLightArray->LightNum[i] == 'Z')
			{
				pLightArray->bLight[i] = nuTRUE;
				Enable++;
			}
			break;
		case T_LUturn:
			if(	pLightArray->LightNum[i] == '1'||
				pLightArray->LightNum[i] == '2'||
				pLightArray->LightNum[i] == '6'||
				pLightArray->LightNum[i] == '7'||
				pLightArray->LightNum[i] == '9'||
			    pLightArray->LightNum[i] == 'B'||
			    pLightArray->LightNum[i] == 'E'||
			    pLightArray->LightNum[i] == 'H'||
			    pLightArray->LightNum[i] == 'J'||
			    pLightArray->LightNum[i] == 'L'||
			    pLightArray->LightNum[i] == 'M'||
				pLightArray->LightNum[i] == 'Z')
			{//左轉或靠左
				pLightArray->bLight[i] = nuTRUE;
				Enable++;
			}
			break;
		case T_Right:	
			if(pLightArray->LightNum[i] == '4'||
			   pLightArray->LightNum[i] == '5'||
			   pLightArray->LightNum[i] == '6'||
			   pLightArray->LightNum[i] == '7'||
			   pLightArray->LightNum[i] == '8'||
			   pLightArray->LightNum[i] == 'D'||
			   pLightArray->LightNum[i] == 'K'||
			   pLightArray->LightNum[i] == 'L'||
			   pLightArray->LightNum[i] == 'Z')
			{
				pLightArray->bLight[i] = nuTRUE;
				Enable++;
			}
			break;
		case T_RightSide:
			if(pLightArray->LightNum[i] == '4'||
			   pLightArray->LightNum[i] == '5'||
			   pLightArray->LightNum[i] == '6'||
			   pLightArray->LightNum[i] == '7'||
			   pLightArray->LightNum[i] == '8'||
			   pLightArray->LightNum[i] == 'D'||
			   pLightArray->LightNum[i] == 'K'||
			   pLightArray->LightNum[i] == 'L'||
			   pLightArray->LightNum[i] == 'Z')
			{
				pLightArray->bLight[i] = nuTRUE;
				Enable++;
			}
			break;
		case T_RUturn:
			if(pLightArray->LightNum[i] == '4'||
			   pLightArray->LightNum[i] == '5'||
			   pLightArray->LightNum[i] == '6'||
			   pLightArray->LightNum[i] == '7'||
			   pLightArray->LightNum[i] == '8'||
			   pLightArray->LightNum[i] == 'A'||
			   pLightArray->LightNum[i] == 'D'||
			   pLightArray->LightNum[i] == 'G'||
			   pLightArray->LightNum[i] == 'I'||
			   pLightArray->LightNum[i] == 'K'||
			   pLightArray->LightNum[i] == 'L'||
			   pLightArray->LightNum[i] == 'N'||
			   pLightArray->LightNum[i] == 'Z')
			{//右轉或靠右
				pLightArray->bLight[i] = nuTRUE;
				Enable++;
			}
			break;
		default:
			break;
		}
		/*if((pLightArray->LightNum[i] == '6' || pLightArray->LightNum[i] == '7'))
		{
			pLightArray->bLight[i] = true;
			Enable++;
			continue;
		}*/
__android_log_print(ANDROID_LOG_INFO, "LaneGuide", "Get Enable %d, LightID %d", pLightArray->bLight[i], pLightArray->LightNum[i]);
	}
	if(0 == Enable )
	{
		for(i = 0; i < pLightArray->LaneCount; i++)
		{
			if(pLightArray->LightNum[i] == '3' || pLightArray->LightNum[i] == 'Z')
			{
				pLightArray->bLight[i] = nuTRUE;
			}
		}
	}
}
