// RoadName.cpp: implementation of the CRoadName class.
//
//////////////////////////////////////////////////////////////////////

#include "RoadName.h"
#include "NuroEngineStructs.h"
#include "libDrawMap.h"

#include "PrevRoadNameInfoList.h" // Added by Dengxu @ 2012 02 26
#define TGDXDY										1//4
#define MIN_ROADLEN_FORNAME							10
#define MIN_ROAD_RAWDIS								20

#define MAX_ROAD_ICON_COUNT                         10

#define RDNAME_L_TO_R			0x00
#define RDNAME_LT_TO_RB			0x01
#define RDNAME_LB_TO_RT			0x02
#define RDNAME_T_TO_D			0x10
#define RDNAME_TL_TO_BR			0x11
#define RDNAME_TR_TO_BL			0x12

#define BANNER_HEIGHT						10
#define BANNER_CUT							3
#define BANNER_LT_EX						(1)
#define BANNER_TP_EX						(01)
#define BANNER_RT_EX						(00)
#define BANNER_BT_EX						(00)
#define BANNER_L_SPACE						2
#define BANNER_R_SPACE						3


#define VALUE_0								L'0'
#define VALUE_9								L'9'
#define VALUE_Dot							L';'
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRoadName::CRoadName()
{
	m_nRdNameIndex	= 0;
	m_nRdNameCount	= 0;
	m_pRdNameList	= NULL;
	m_pRoadSetting	= NULL;
	m_nMapType		= 0;
	m_nOldMapAngle  = 0;
//	pAddRoadName	= AddRoadName3D;
//	pDrawRoadName	= DrawRoadName3D;
}

CRoadName::~CRoadName()
{
	Free();
}

nuBOOL CRoadName::Initialize(nuWORD nNameCount, PROADSETTING	pRoadSetting, nuBYTE nMapType /*= ROADNAME_MAP*/)
{
	//CRoadName();
	m_pRdNameList = (LPRDNAMENODE)g_pDMLibMM->MM_GetStaticMemMass(nNameCount*sizeof(RDNAMENODE));
	if( m_pRdNameList == NULL )
	{
		return nuFALSE;
	}
	nuMemset(m_pRdNameList, 0, nNameCount*sizeof(RDNAMENODE));
	m_nRdNameCount = nNameCount;
	m_nRdNameIndex = 0;
	m_pRoadSetting = pRoadSetting;
	m_nMapType	= nMapType;
	nuLONG nTmp = 0;
	if( !nuReadConfigValue("MAPVERSION", &nTmp) )
	{
		nTmp = 0;
	}
	m_nMapDataType = (nuBYTE)nTmp;

	if( !nuReadConfigValue("ADDROADFONTSIZE", &nTmp) )
	{
		nTmp = 0;
	}
	m_nAddRoadFontSize = (nuBYTE)nTmp;
	//
	m_nRdIconIdx = 0;
	m_pRdIconList = new Road_Icon_Node[MAX_ROAD_ICON_COUNT];
	if( NULL == m_pRdIconList )
	{
		return nuFALSE;
	}
	
	m_byPrevScaleIdx = 0;
	m_b3DMode = 0;
	m_nMapDirectMode = MAP_DIRECT_NORTH;
	return nuTRUE;
}

nuVOID CRoadName::Free()
{
	m_nRdNameIndex	= 0;
	m_nRdNameCount	= 0;
	//m_pRdNameList	= NULL;
	if( NULL != m_pRdIconList )
	{
		delete[] m_pRdIconList;
		m_pRdIconList = NULL;
	}
	m_nRdIconIdx = 0;
}

nuBOOL CRoadName::ResetRdNameList()
{
	int count = 0;
	int i = 0;
	nuroPOINT ptTmp;
	nuSHORT nTempAngle = m_nOldMapAngle - g_pDMGdata->carInfo.nMapAngle;
	if (m_byPrevScaleIdx != g_pDMGdata->uiSetData.nScaleIdx || 
		m_b3DMode        != g_pDMGdata->uiSetData.b3DMode || 
		m_nMapDirectMode != g_pDMGdata->uiSetData.nMapDirectMode||
		10 <= NURO_ABS(nTempAngle))
	{
	    m_nRdNameIndex = 0;
	}
	else
	{	
		if (g_pDMGdata->uiSetData.b3DMode)
		{
			NURORECT rtScreen = {0};
			NURORECT rect;
			nuWORD nRdNameCount = m_nRdNameIndex;
			m_nRdNameIndex = 0;
			rtScreen.left	= g_pDMGdata->transfData.nBmpLTx;
			rtScreen.top	= g_pDMGdata->transfData.nBmpLTy;
			rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth ;
			rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight;
			if( g_pDMGdata->uiSetData.b3DMode )
			{
				rtScreen.top += 100;
			}
			for (i = 0; i < nRdNameCount; i++)
			{
				g_pDMLibMT->MT_MapToBmp(m_pRdNameList[i].ptMapStart.x, m_pRdNameList[i].ptMapStart.y, &ptTmp.x, &ptTmp.y);
				g_pDMLibMT->MT_Bmp2Dto3D(ptTmp.x, ptTmp.y, MAP_DEFAULT);
				rect.top    = ptTmp.y - m_pRdNameList[i].nFh - BANNER_HEIGHT;
				rect.bottom	= ptTmp.y;
				rect.left	= ptTmp.x - m_pRdNameList[i].nFw * m_pRdNameList[i].rdNameLen / BANNER_CUT;
				rect.right	= ptTmp.x + m_pRdNameList[i].nFw * m_pRdNameList[i].rdNameLen * (BANNER_CUT - 1) / BANNER_CUT;
				if ( nuPointInRect(&ptTmp, &rtScreen) && !CheckRect(&rect) )
				{	
					m_pRdNameList[i].ptStart = ptTmp;
					m_pRdNameList[i].rect = rect;
					m_pRdNameList[count++] = m_pRdNameList[i];
					m_nRdNameIndex = count;
				}
			}
		}
		else
		{
			for (i = 0; i < m_nRdNameIndex; i++)
			{
				if (nuRectCoverRect(&m_pRdNameList[i].mapRect, &g_pDMGdata->transfData.nuShowMapSize))
				{
					g_PrevRoadName.ConvertRect_MapToBmp(m_pRdNameList[i].rect, m_pRdNameList[i].mapRect);
					g_pDMLibMT->MT_MapToBmp(m_pRdNameList[i].ptMapStart.x, m_pRdNameList[i].ptMapStart.y, &ptTmp.x, &ptTmp.y);
					m_pRdNameList[i].ptStart = ptTmp;
					m_pRdNameList[count++] = m_pRdNameList[i];
				}
			}
			m_nRdNameIndex = count;
		}
	}
	m_byPrevScaleIdx = g_pDMGdata->uiSetData.nScaleIdx;
	m_b3DMode        = g_pDMGdata->uiSetData.b3DMode;
	m_nMapDirectMode = g_pDMGdata->uiSetData.nMapDirectMode;
	m_nOldMapAngle   = g_pDMGdata->carInfo.nMapAngle;
	return nuTRUE;
}

nuBOOL CRoadName::AddRoadName(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, nuBYTE nRoadType, nuroPOINT *pPt,
							nuINT idx1, nuINT idx2)
{
	if( m_nMapDataType == 1 && nNameLen >= 4 )
	{
		nNameLen -= 4;
	}
	if( nDwIdx == -1 || nNameAddr == -1 || nNameLen == 0 || pPt == NULL|| idx2 - idx1 < 1 || m_nRdNameIndex == m_nRdNameCount )
	{
		return nuFALSE;
	}
	for (nuINT i = 0; i < m_nRdNameIndex; i++)
	{
		if (m_pRdNameList[i].rdDwIdx == nDwIdx && 
			m_pRdNameList[i].rdNameAddr == nNameAddr && 
			m_pRdNameList[i].rdNameLen == nNameLen && 
			m_pRdNameList[i].rdType == nRoadType)
		{
			return nuTRUE;
		}
	}
	if( g_pDMGdata->uiSetData.b3DMode )
	{
		return AddRoadName3D(nDwIdx, nNameAddr, nNameLen, nRoadType, pPt, idx1, idx2);
	}
	else
	{
		return AddRoadNameCN(nDwIdx, nNameAddr, nNameLen, nRoadType, pPt, idx1, idx2);
//		return AddRoadNameCH(nDwIdx, nNameAddr, nNameLen, nRoadType, pPt, idx1, idx2);
	}
//	return (this->*pAddRoadName)(nDwIdx, nNameAddr, nNameLen, nRoadType, pPt, idx1, idx2);
}

/* 中文路名显示的构建方法，一个字符的宽度要满足是2个英文字符的宽度。
 * 2D画面的显示，3D不用这个方法。
 */
nuBOOL CRoadName::AddRoadNameCH(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, nuBYTE nRoadType,
							  nuroPOINT *pPt, nuINT idx1, nuINT idx2)
{
	nuroPOINT pt1={0};
	nuroPOINT pt2={0};
	nuroPOINT ptTmp={0};
	nuroPOINT tmpPt={0};
	nuroPOINT ptStep[2]={0};

	pt1 = pPt[idx1];
	pt2 = pPt[idx1+1];
	nuLONG Dx, Dy, Dis, aDx, aDy, tmpLen, tmpV;
	Dx= Dy= Dis= aDx= aDy= tmpLen= tmpV=0;
	nuBYTE tmpAlign=0;
	nuINT lDis, index,lStep,lStepCount;
	lDis= index=lStep=lStepCount=0;
	nuBOOL bIncrease = nuFALSE;
	nuBYTE nFH, nFW;
	nFH= nFW=0;
	NURORECT rtScreen={0};
	NURORECT tmpRect={0};
	nFH = m_pRoadSetting[nRoadType].nFontHeight/2;
	nFW = m_pRoadSetting[nRoadType].nFontWidth*3/4;
	if( nFW == 0 )
	{
		nFW = nFH;
	}
	Dx = pPt[idx1].x - pPt[idx2].x;
	Dy = pPt[idx1].y - pPt[idx2].y;
	Dis = (nuLONG)nuSqrt(Dx*Dx + Dy*Dy);
	if( Dis < nFW*nNameLen && !g_pDMGdata->uiSetData.nBsdDrawMap)
	{
		return nuFALSE;
	}
	index = CheckName(nDwIdx, nNameAddr, &Dis);
	if( index < 0 || index == m_nRdNameCount )
	{
		return nuFALSE;
	}
	lDis = Dis;
	if( m_nMapType == MAP_DEFAULT )
	{
/*
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW*nNameLen/2;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFW*nNameLen/2;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - nFW*nNameLen/2;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - nFW*nNameLen/2;
*/
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW*3;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFW*8;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - nFW*3;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - nFW*5;
	}
	else//Zoom map
	{
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW*nNameLen/2;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFW*nNameLen/2;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - nFW*nNameLen/2;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - nFW*nNameLen/2;
	}
	for(nuINT i = idx1; i < idx2; i++)
	{
/*
		ptTmp.x = (pPt[i].x + pPt[i+1].x)/2;
		ptTmp.y = (pPt[i].y + pPt[i+1].y)/2;
		if( (pPt[i].y > rtScreen.bottom && pPt[i+1].y > rtScreen.bottom) ||
			(pPt[i].y < rtScreen.top	&& pPt[i+1].y < rtScreen.top   ) ||
			(pPt[i].x < rtScreen.left   && pPt[i+1].x < rtScreen.left  ) ||
			(pPt[i].x > rtScreen.right  && pPt[i+1].x > rtScreen.right ) ||
			!nuPointInRect(&ptTmp, &rtScreen) )
		{
			continue;
		}

		Dx = pPt[i+1].x - pPt[i].x;
		Dy = pPt[i+1].y - pPt[i].y;
		Dis = (nuLONG)nuSqrt(Dx*Dx + Dy*Dy);
		if( Dis > lDis )
		{
			bIncrease = nuTRUE;
			pt1	= pPt[i];
			pt2	= pPt[i+1];
			lDis = Dis;
		}
*/
		if( (pPt[i].y > rtScreen.bottom && pPt[i+1].y > rtScreen.bottom) ||
			(pPt[i].y < rtScreen.top	&& pPt[i+1].y < rtScreen.top   ) ||
			(pPt[i].x < rtScreen.left   && pPt[i+1].x < rtScreen.left  ) ||
			(pPt[i].x > rtScreen.right  && pPt[i+1].x > rtScreen.right ) )
		{
			continue;
		}
		Dx = pPt[i+1].x - pPt[i].x;
		Dy = pPt[i+1].y - pPt[i].y;
		Dis = (nuLONG)nuSqrt(Dx*Dx + Dy*Dy);

//		if(Dis>=nFW*nNameLen)
		{
//			lStepCount=((Dis*4)/(nFW*nNameLen))+1;
			lStepCount=(Dis/((g_pDMGdata->transfData.nBmpHeight/10)+1))+1;
			lStep=0;
			for(nuINT j=0;j<=lStepCount;j++)
			{
				ptTmp.x = pPt[i].x + Dx*j/lStepCount;
				ptTmp.y = pPt[i].y + Dy*j/lStepCount;
				if( !nuPointInRect(&ptTmp, &rtScreen) )
				{
					if(lStep>0) break;
					continue;
				}
				else
				{
					if(lStep==0)
					{
						ptStep[0]=ptTmp;
					}
					else
					{
						ptStep[1]=ptTmp;
					}
					lStep++;
				}
			}
			if( lStep>=2 && Dis > lDis)
			{
				bIncrease = nuTRUE;
				pt1	= ptStep[0];
				pt2	= ptStep[1];
				lDis = Dis;
			}
		}
//		else
		{
/*
			ptTmp.x = (pPt[i].x + pPt[i+1].x)/2;
			ptTmp.y = (pPt[i].y + pPt[i+1].y)/2;
			if( !nuPointInRect(&ptTmp, &rtScreen) )
			{
				continue;
			}
			if( Dis > lDis )
			{
				bIncrease = nuTRUE;
				pt1	= pPt[i];
				pt2	= pPt[i+1];
				lDis = Dis;
			}
*/
		}
	}
	if( !bIncrease )
	{
		return nuFALSE;
	}
	Dx = pt2.x - pt1.x;
	Dy = pt2.y - pt1.y;
	ptTmp.x = (pt1.x + pt2.x) / 2;
	ptTmp.y = (pt1.y + pt2.y) / 2;
	aDx = NURO_ABS(Dx);
	aDy = NURO_ABS(Dy);
	if( aDx >= aDy )//Draw name from left to right
	{
		if( Dx < 0 )
		{
			tmpPt = pt1;
			pt1 = pt2;
			pt2 = tmpPt;
			Dx = pt2.x - pt1.x;
			Dy = pt2.y - pt1.y;
		}
        if(Dx==0)
        {
            Dx = 1;
        }
		tmpLen = nFW * Dy / Dx;
		/*
		if( Dx < nFW * nNameLen )
		{
			tmpRect.left = pt1.x;
			tmpRect.top	 = pt1.y;
		}
		else
		*/
		{
			tmpRect.left = ptTmp.x - nFW * (nNameLen/2);
			tmpRect.top	 = ptTmp.y - tmpLen * (nNameLen/2);
			pt1.x = tmpRect.left;
			pt1.y = tmpRect.top;
		}
		tmpRect.right	= tmpRect.left + nFW * nNameLen;
		tmpRect.bottom	= tmpRect.top + tmpLen * nNameLen;

		if( NURO_ABS(Dy*10/Dx) < TGDXDY )//Draw from left to right
		{
			tmpRect.top		= ptTmp.y - nFH;
			tmpRect.bottom  = ptTmp.y + nFH;
			tmpAlign		= RDNAME_L_TO_R;
		}
		else if( Dy > 0 )//Draw from left top to right bottom
		{
			tmpRect.bottom += (nFH - tmpLen);
			tmpAlign		= RDNAME_LT_TO_RB;
		}
		else//Draw from left bottom to right top
		{	
			tmpV			= tmpRect.bottom;
			tmpRect.bottom	= tmpRect.top;
			tmpRect.top		= tmpV;
			//
			tmpRect.top		-= tmpLen;
			tmpRect.bottom	+= nFH;
			tmpAlign		= RDNAME_LB_TO_RT;
		}
	}
	else//Draw from up to down
	{
		if( Dy < 0 )
		{
			tmpPt = pt1;
			pt1 = pt2;
			pt2 = tmpPt;
			Dx = pt2.x - pt1.x;
			Dy = pt2.y - pt1.y;
		}
		tmpLen = nFH * Dx / Dy;
		if( NURO_ABS(tmpLen) > nFW )
		{
			tmpLen = tmpLen * nFW / NURO_ABS(tmpLen);
		}
		/*
		if( Dy < nFW * nNameLen )
		{
			tmpRect.left = pt1.x;
			tmpRect.top	 = pt1.y;
		}
		else
		*/
		{
			tmpRect.left = ptTmp.x - tmpLen * (nNameLen/2);
			tmpRect.top	 = ptTmp.y - nFH * (nNameLen/2);
			pt1.x = tmpRect.left;
			pt1.y = tmpRect.top;
		}
		tmpRect.right	= tmpRect.left + tmpLen * nNameLen;
		tmpRect.bottom	= tmpRect.top + nFH * nNameLen;
		if( NURO_ABS(Dx*10/Dy) < TGDXDY )//Draw from top to bottom
		{
			tmpRect.left	= ptTmp.x - nFW;
			tmpRect.right	= ptTmp.x + nFW/2;
			tmpAlign		= RDNAME_T_TO_D;
		}
		else if( Dx > 0 )//Draw from top left to bottom right
		{
			tmpRect.right	+= (nFW - tmpLen);
			tmpAlign		= RDNAME_TL_TO_BR;
		}
		else//Draw from top right to bottom left
		{
			tmpV			= tmpRect.left;
			tmpRect.left	= tmpRect.right;
			tmpRect.right	= tmpV;

			tmpRect.left	-= tmpLen;
			tmpRect.right	+= nFW;
			tmpAlign		= RDNAME_TR_TO_BL;
		}
	}
	if( CheckRect(&tmpRect) )
	{
		return nuFALSE;
	}
	m_pRdNameList[index].rdAlign	= tmpAlign;
	m_pRdNameList[index].ptStart	= pt1;
	m_pRdNameList[index].ptEnd		= pt2;
	m_pRdNameList[index].rdDwIdx	= nDwIdx;
	m_pRdNameList[index].rdNameAddr	= nNameAddr;
	m_pRdNameList[index].rdNameLen	= nNameLen;
	m_pRdNameList[index].rdType		= nRoadType;
	m_pRdNameList[index].rect		= tmpRect;
	m_pRdNameList[index].rdDis		= lDis;
	if( index == m_nRdNameIndex )
	{
		m_nRdNameIndex++;
	}
	return nuTRUE;
}


nuBOOL CRoadName::AddRoadName3D(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, nuBYTE nRoadType,
							    nuroPOINT *pPt, nuINT idx1, nuINT idx2)
{
	nuBYTE nFH=0, nFW=0;
	NURORECT rtScreen = {0}, tmpRect = {0}, rtUse = {0}, tmp2DRect = {0};
	NUROPOINT ptMid = {0}, ptL = {0};
	NUROPOINT ptMid2D = {0};
	nuBOOL bIncrease = nuFALSE;
	nuLONG Dis = 0, lDis = 0, Dx = 0 , Dy = 0;
	Dx = pPt[idx1].x - pPt[idx2].x;
	Dy = pPt[idx1].y - pPt[idx2].y;
	Dis = (nuLONG)nuSqrt(Dx*Dx + Dy*Dy);
	nuINT index = CheckName(nDwIdx, nNameAddr, &Dis);
	lDis = Dis;
	if( index < 0 || index == m_nRdNameCount )
	{
		return nuFALSE;
	}
	nFH = m_pRoadSetting[nRoadType].nFontHeight;
	nFW = m_pRoadSetting[nRoadType].nFontWidth;
	if( nFW == 0 )
	{
		nFW = nFH/2;
	}
	if( m_nMapType == MAP_DEFAULT )
	{
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW * nNameLen / BANNER_CUT;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFH + BANNER_HEIGHT;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - 
						  nFW * nNameLen * (BANNER_CUT - 1) / BANNER_CUT;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - 10;
		if( g_pDMGdata->uiSetData.b3DMode )
		{
			//rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
			rtScreen.top += 100;
		}
	}
	else//Zoom map
	{
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW * nNameLen / BANNER_CUT;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFH + BANNER_HEIGHT;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - 
						  nFW * nNameLen * (BANNER_CUT - 1) / BANNER_CUT;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - 10;
		if( g_pDMGdata->uiSetData.b3DMode )
		{
			//rtScreen.top += g_pDMGdata->uiSetData.nSkyHeight;
			rtScreen.top += 100;
		}
	}
	for(nuINT i = idx1; i < idx2; i++)
	{
		ptMid.x = (pPt[i].x + pPt[i+1].x)/2;
		ptMid.y = (pPt[i].y + pPt[i+1].y)/2;
		if( ptMid.y < rtScreen.bottom )
		{
			if( g_pDMGdata->uiSetData.b3DMode )
			{
				ptMid2D = ptMid;
				g_pDMLibMT->MT_Bmp2Dto3D( ptMid.x, ptMid.y, m_nMapType);
			}
			if( !nuPointInRect(&ptMid, &rtScreen) )
			{
				continue;
			}
			tmpRect.top		= ptMid.y - nFH - BANNER_HEIGHT;
			tmpRect.bottom	= ptMid.y;
			tmpRect.left	= ptMid.x - nFW * nNameLen / BANNER_CUT;
			tmpRect.right	= ptMid.x + nFW * nNameLen * (BANNER_CUT - 1) / BANNER_CUT;
			if( CheckRect(&tmpRect) )
			{
				continue;
			}
			Dx = pPt[i].x - pPt[i+1].x;
			Dy = pPt[i].y - pPt[i+1].y;
			Dis = (nuLONG)nuSqrt(Dx*Dx + Dy*Dy);
			if( Dis <= lDis )
			{
				continue;
			}
			lDis = Dis;
			ptL	 = ptMid;
			rtUse = tmpRect;
			bIncrease = nuTRUE;
		}
	}
	if( bIncrease )
	{
		m_pRdNameList[index].ptStart	= ptL;
		m_pRdNameList[index].rect		= rtUse;
		m_pRdNameList[index].rdDis		= (nuINT)lDis;
		m_pRdNameList[index].rdDwIdx	= nDwIdx;
		m_pRdNameList[index].rdNameAddr	= nNameAddr;
		m_pRdNameList[index].rdNameLen	= nNameLen;
		m_pRdNameList[index].rdType		= nRoadType;
		m_pRdNameList[index].nFh        = nFH;
		m_pRdNameList[index].nFw        = nFW;
		g_pDMLibMT->MT_BmpToMap(ptMid2D.x, ptMid2D.y, &m_pRdNameList[index].ptMapStart.x, &m_pRdNameList[index].ptMapStart.y);
		if( index == m_nRdNameIndex )
		{
			m_nRdNameIndex++;
		}
		tmp2DRect.top		= ptMid2D.y - nFH - BANNER_HEIGHT;
		tmp2DRect.bottom	= ptMid2D.y;
		tmp2DRect.left	    = ptMid2D.x - nFW * nNameLen / BANNER_CUT;
		tmp2DRect.right	    = ptMid2D.x + nFW * nNameLen * (BANNER_CUT - 1) / BANNER_CUT;
		g_pDMLibMT->MT_BmpToMap( tmp2DRect.left, tmp2DRect.top, 
			                     &m_pRdNameList[index].mapRect.left, &m_pRdNameList[index].mapRect.top );
		g_pDMLibMT->MT_BmpToMap( tmp2DRect.right, tmp2DRect.bottom, 
			                     &m_pRdNameList[index].mapRect.right, &m_pRdNameList[index].mapRect.bottom );
		g_pDMLibMT->MT_BmpToMap(ptMid2D.x, ptMid2D.y, &m_pRdNameList[index].ptMapStart.x, &m_pRdNameList[index].ptMapStart.y);
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}

nuBOOL CRoadName::AddRoadNameCN(nuWORD nDwIdx, nuDWORD nNameAddr, nuBYTE nNameLen, 
							  nuBYTE nRoadType, nuroPOINT *pPt, nuINT idx1, nuINT idx2)
{
	nuroPOINT pt1, pt2 = {0}, ptTmp = {0}, tmpPt = {0};
	pt1 = pPt[idx1];
	pt2 = pPt[idx1+1];
	nuLONG Dx = 0 , Dy = 0 , Dis = 0 , aDx = 0 , aDy = 0 , tmpLen = 0 , tmpV = 0 ;
	nuBYTE tmpAlign = 0 ;
	nuINT lDis = 0 , index = 0 ;
	nuBOOL bIncrease = nuFALSE;
	nuBYTE nFH = 0 , nFW = 0 ;
	NURORECT rtScreen = {0}, tmpRect = {0};
	RDNAMENODE rdNameNode = {0};
	nFH = m_pRoadSetting[nRoadType].nFontHeight*3/8;
	nFW = m_pRoadSetting[nRoadType].nFontWidth*3/4;
	nFH = nFH*7/5;//Louis Add 2012/02/20
	nFW = nFW*7/5;

	if( nFW == 0 )
	{
		nFW = nFH;
	}
	Dx = pPt[idx1].x - pPt[idx2].x;
	Dy = pPt[idx1].y - pPt[idx2].y;
	Dis = (nuLONG)nuSqrt(Dx*Dx + Dy*Dy);
	if( Dis < nFW*nNameLen && !g_pDMGdata->uiSetData.nBsdDrawMap)
	{
		return nuFALSE;
	}
	index = CheckName(nDwIdx, nNameAddr, &Dis);
	if( index < 0 || index == m_nRdNameCount )
	{
		return nuFALSE;
	}
	lDis = Dis;
	if( m_nMapType == MAP_DEFAULT )
	{
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW*3;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFW*8;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - nFW*3;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - nFW*5;
	}
	else//Zoom map
	{
		rtScreen.left	= g_pDMGdata->transfData.nBmpLTx + nFW*nNameLen/2;
		rtScreen.top	= g_pDMGdata->transfData.nBmpLTy + nFW*nNameLen/2;
		rtScreen.right	= g_pDMGdata->transfData.nBmpLTx + g_pDMGdata->transfData.nBmpWidth - nFW*nNameLen/2;
		rtScreen.bottom	= g_pDMGdata->transfData.nBmpLTy + g_pDMGdata->transfData.nBmpHeight - nFW*nNameLen/2;
	}
	for(nuINT i = idx1; i < idx2; i++)
	{
		ptTmp.x = (pPt[i].x + pPt[i+1].x)/2;
		ptTmp.y = (pPt[i].y + pPt[i+1].y)/2;
		if( (pPt[i].y > rtScreen.bottom && pPt[i+1].y > rtScreen.bottom) ||
			(pPt[i].y < rtScreen.top	&& pPt[i+1].y < rtScreen.top   ) ||
			(pPt[i].x < rtScreen.left   && pPt[i+1].x < rtScreen.left  ) ||
			(pPt[i].x > rtScreen.right  && pPt[i+1].x > rtScreen.right ) ||
			!nuPointInRect(&ptTmp, &rtScreen) )
		{
			continue;
		}

		Dx = pPt[i+1].x - pPt[i].x;
		Dy = pPt[i+1].y - pPt[i].y;
		Dis = (nuLONG)nuSqrt(Dx*Dx + Dy*Dy);
		if( Dis <= lDis )
		{
			continue;
		}
		pt1	= pPt[i];
		pt2	= pPt[i+1];
		lDis = Dis;
		Dx = pt2.x - pt1.x;
		Dy = pt2.y - pt1.y;
		ptTmp.x = (pt1.x + pt2.x) / 2;
		ptTmp.y = (pt1.y + pt2.y) / 2;
		aDx = NURO_ABS(Dx);
		aDy = NURO_ABS(Dy);
		if( aDx >= aDy )//Draw name from left to right
		{
			if( 0 == aDx )
			{
				continue;
			}
			if( Dx < 0 )
			{
				tmpPt = pt1;
				pt1 = pt2;
				pt2 = tmpPt;
				Dx = pt2.x - pt1.x;
				Dy = pt2.y - pt1.y;
			}//exchange pt1 and pt2 to make sure of "pt1.x <= pt2.x"

			tmpLen = nFW * Dy / Dx;

			tmpRect.left = ptTmp.x - nFW * (nNameLen/2);
			tmpRect.top	 = ptTmp.y - tmpLen * (nNameLen/2);
			pt1.x = tmpRect.left;
			pt1.y = tmpRect.top;

			tmpRect.right	= tmpRect.left + nFW * nNameLen;
			tmpRect.bottom	= tmpRect.top + tmpLen * nNameLen;
			
			if( NURO_ABS(Dy*10/Dx) < TGDXDY )//Draw from left to right
			{
				tmpRect.top		= ptTmp.y - nFH;
				tmpRect.bottom  = ptTmp.y + nFH;
				tmpAlign		= RDNAME_L_TO_R;
			}
			else if( Dy > 0 )//Draw from left top to right bottom
			{
				tmpRect.bottom += (nFH - tmpLen);
				tmpAlign		= RDNAME_LT_TO_RB;
			}
			else//Draw from left bottom to right top
			{	
				tmpV			= tmpRect.bottom;
				tmpRect.bottom	= tmpRect.top;
				tmpRect.top		= tmpV;
				//
				tmpRect.top		-= tmpLen;
				tmpRect.bottom	+= nFH;
				tmpAlign		= RDNAME_LB_TO_RT;
			}
		}
		else//Draw from up to down
		{
			if( Dy < 0 )
			{
				tmpPt = pt1;
				pt1 = pt2;
				pt2 = tmpPt;
				Dx = pt2.x - pt1.x;
				Dy = pt2.y - pt1.y;
			}
			tmpLen = nFH * Dx / Dy;
			if( NURO_ABS(tmpLen) > nFW )
			{
				tmpLen = tmpLen * nFW / NURO_ABS(tmpLen);
			}
			tmpRect.left = ptTmp.x - tmpLen * (nNameLen/2);
			tmpRect.top	 = ptTmp.y - nFH * (nNameLen/2);
			pt1.x = tmpRect.left;
			pt1.y = tmpRect.top;
			tmpRect.right	= tmpRect.left + tmpLen * nNameLen;
			tmpRect.bottom	= tmpRect.top + nFH * nNameLen;
			if( NURO_ABS(Dx*10/Dy) < TGDXDY )//Draw from top to bottom
			{
				tmpRect.left	= ptTmp.x - nFW;
				tmpRect.right	= ptTmp.x + nFW/2;
				tmpAlign		= RDNAME_T_TO_D;
			}
			else if( Dx > 0 )//Draw from top left to bottom right
			{
				tmpRect.right	+= (nFW - tmpLen);
				tmpAlign		= RDNAME_TL_TO_BR;
			}
			else//Draw from top right to bottom left
			{
				tmpV			= tmpRect.left;
				tmpRect.left	= tmpRect.right;
				tmpRect.right	= tmpV;
				
				tmpRect.left	-= tmpLen;
				tmpRect.right	+= nFW;
				tmpAlign		= RDNAME_TR_TO_BL;
			}
		}
		if( CheckRectCN(&tmpRect, tmpAlign, nNameLen, nFW) )
		{
			bIncrease = nuTRUE;
			rdNameNode.rdAlign		= tmpAlign;
			rdNameNode.ptStart		= pt1;
			rdNameNode.ptEnd		= pt2;
			rdNameNode.rdDwIdx		= nDwIdx;
			rdNameNode.rdNameAddr	= nNameAddr;
			rdNameNode.rdNameLen	= nNameLen;
			rdNameNode.rdType		= nRoadType;
			rdNameNode.rect			= tmpRect;
			rdNameNode.rdDis		= lDis;
			rdNameNode.nFw			= nFW;
			rdNameNode.nFh			= nFH;
		}
	}
	if( !bIncrease )
	{
		return nuFALSE;
	}
	m_pRdNameList[index] = rdNameNode;
	g_pDMLibMT->MT_BmpToMap( m_pRdNameList[index].rect.left, m_pRdNameList[index].rect.top, 
		                     &m_pRdNameList[index].mapRect.left, &m_pRdNameList[index].mapRect.top );
	g_pDMLibMT->MT_BmpToMap( m_pRdNameList[index].rect.right, m_pRdNameList[index].rect.bottom, 
		                     &m_pRdNameList[index].mapRect.right, &m_pRdNameList[index].mapRect.bottom );
	g_pDMLibMT->MT_BmpToMap( m_pRdNameList[index].ptStart.x, m_pRdNameList[index].ptStart.y,
	                     &m_pRdNameList[index].ptMapStart.x, &m_pRdNameList[index].ptMapStart.y );
	if( index == m_nRdNameIndex )
	{
		m_nRdNameIndex++;
	}
	return nuTRUE;
}

nuINT CRoadName::CheckName(nuWORD nDwIdx, nuDWORD nNameAddr, nuLONG* pDis)
{
	for( nuWORD i = 0; i < m_nRdNameIndex; i++ )
	{
		if( m_pRdNameList[i].rdDwIdx	== nDwIdx &&
			m_pRdNameList[i].rdNameAddr	== nNameAddr )
		{
			if( m_pRdNameList[i].rdDis >= *pDis )
			{
				return -1;
			}
			else
			{
				*pDis = m_pRdNameList[i].rdDis;
				return i;
			}
		}
	}
    if( g_pDMGdata->uiSetData.nBsdDrawMap )
    {
        *pDis = 5;
	}
    else
    {
        *pDis = MIN_ROADLEN_FORNAME;
    }	
	return m_nRdNameIndex;
}

nuBOOL CRoadName::CheckRect(nuroRECT *lpRect)
{
	for(nuWORD i = 0; i < m_nRdNameIndex; i++ )
	{
		if( nuRectCoverRect(lpRect, &m_pRdNameList[i].rect) )
		{
			return nuTRUE;
		}
	}
	return nuFALSE;
}

nuBOOL CRoadName::DrawRoadName()
{
	if( g_pDMGdata->uiSetData.b3DMode )
	{
		return DrawRoadName3D();
	}
	else
	{
		return DrawRoadNameCN();
	}
//	return (this->*pDrawRoadName)();
}


nuBOOL CRoadName::DrawRoadNameCH()
{
	RNEXTEND rnExtend={0};
	nuWCHAR rnName[RN_MAX_LEN]={0};
	nuBYTE nNowRdType = -1;
	nuLONG pInOut = 0;
	nuINT h, w, sx, sy, dx, dy, nRnNum, x, y,NumberShift;
	h= w= sx= sy= dx= dy= nRnNum= x= y=NumberShift=0;
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	nuCOLORREF col = {0};
	nuCOLORREF rimCol = {0};
	nuBYTE n3Dmode = 0;
	if( m_nMapType == MAP_DEFAULT )
	{
		n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	}
	else//Zoom Screen
	{
		n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	}
	for( nuINT i = m_nRdNameIndex - 1; i >= 0; i-- )
	{
		nuMemset(rnName, 0, sizeof(rnName));
		if( !g_pDMLibFS->FS_GetRoadName(m_pRdNameList[i].rdDwIdx, m_pRdNameList[i].rdNameAddr, m_pRdNameList[i].rdType, rnName, RN_MAX_LEN, &rnExtend) )
		{
			continue;
		}
	
		if( nNowRdType != m_pRdNameList[i].rdType )
		{
			if( g_pDMLibMT->MT_SelectObject(DRAW_TYPE_RNFONT, n3Dmode, &m_pRoadSetting[m_pRdNameList[i].rdType], &pInOut) != DRAW_OBJ_ROADNAME )
			{
				continue;
			}
			nNowRdType = m_pRdNameList[i].rdType;
			h = m_pRoadSetting[nNowRdType].nFontHeight * 3 / 4;
			w = m_pRoadSetting[nNowRdType].nFontWidth * 3 / 4;
			if( w <= 0 )
			{
				w = h;
			}
			else
			{
				w *= 2;
			}
			col = nuRGB( m_pRoadSetting[nNowRdType].nFontColorR,
						 m_pRoadSetting[nNowRdType].nFontColorG,
						 m_pRoadSetting[nNowRdType].nFontColorB );
			rimCol	= nuRGB( m_pRoadSetting[nNowRdType].nFontRimColorR,
							 m_pRoadSetting[nNowRdType].nFontRimColorG, 
							 m_pRoadSetting[nNowRdType].nFontRimColorB );
		}
		if( rnExtend.nIconType != 0 && rnExtend.nIconNum != 0 )
		{
			sx = (m_pRdNameList[i].rect.left + m_pRdNameList[i].rect.right)/2;
			sy = (m_pRdNameList[i].rect.top + m_pRdNameList[i].rect.bottom)/2;
			g_pDMLibGDI->GdiDrawIcon(sx - 12, sy - 10, rnExtend.nIconType);
			nuWCHAR wsRd[10] ={0};
			nuItow(rnExtend.nIconNum, wsRd, 10);
			nuroRECT rect_Tmp ={0};
			rect_Tmp.left	=	sx -12;
			rect_Tmp.top	=	sy -12;
			rect_Tmp.right	=	sx +12;
			rect_Tmp.bottom	=	sy +12;
//			g_pDMLibGDI->GdiExtTextOutW(sx -12, sy -12, wsRd, nuWcslen(wsRd));
			
			g_pDMLibGDI->GdiSetTextColor( nuRGB(255, 255, 255)*(rnExtend.nIconType - 1) );
			g_pDMLibGDI->GdiDrawTextW( wsRd, 
				nuWcslen(wsRd),
				&rect_Tmp, 
				NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER );
//			g_libMT.Drawicon(hDC, sx-12, sy-10, IconType);
			continue;
		}
		//
		nRnNum = rnExtend.nRNLen/2;
		NumberShift=0;
		/*for test
		nuroRECT* pRt = ColNameRectCN( &m_pRdNameList[i].rect, 
									   m_pRdNameList[i].rdAlign, 
									   m_pRdNameList[i].rdNameLen/2, 
									   h);
		if( NULL != pRt )
		{
			for(nuBYTE nn = 0; nn < m_pRdNameList[i].rdNameLen/2; ++nn)
			{
				nuroPOINT pppt[5];
				pppt[0].x = pppt[1].x = pRt[nn].left;
				pppt[1].y = pppt[2].y = pRt[nn].top;
				pppt[2].x = pppt[3].x = pRt[nn].right;
				pppt[3].y = pppt[0].y = pRt[nn].bottom;
				pppt[4]   = pppt[0];
				g_pDMLibGDI->GdiPolyline(pppt, 5);
			}
			delete[] pRt;
			pRt=NULL;
		}
		*/
		////////////
		if( m_pRdNameList[i].rdAlign == RDNAME_L_TO_R )
		{
			sx = m_pRdNameList[i].rect.left;
			sy = m_pRdNameList[i].rect.top;
			/*
			g_pDMLibGDI->GdiSetTextColor(rimCol);
			for(nuINT k = -1; k <= 1; k++)
			{
				for(nuINT l = -1; l <= 1; l++)
				{
					if( l == 0 && k == 0 )
					{
						continue;
					}
					g_pDMLibGDI->GdiExtTextOutW(sx+k, sy+l, rnName, nRnNum);
				}
			}
			*/
			g_pDMLibGDI->GdiSetTextColor(col);
//			g_pDMLibGDI->GdiExtTextOutW(sx, sy, rnName, rnExtend.nRNLen/2);
			g_pDMLibGDI->GdiExtTextOutWEx(sx, sy, rnName, rnExtend.nRNLen/2, nuTRUE, rimCol);
			continue;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_T_TO_D )
		{
			dx = 0;
			dy = h;
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
		}
		else if(m_pRdNameList[i].rdAlign == RDNAME_LT_TO_RB )
		{
			dx = w;
			if( nRnNum <= 1 )
			{
				dy = h;
			}
			else
			{
				dy = (m_pRdNameList[i].rect.bottom - m_pRdNameList[i].rect.top - h)/(nRnNum - 1);
			}
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_LB_TO_RT )
		{
			dx = w;
			if( nRnNum <= 1 )
			{
				dy = -h;
			}
			else
			{
				dy = -(m_pRdNameList[i].rect.bottom - m_pRdNameList[i].rect.top - h)/(nRnNum - 1);
			}
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.bottom - h;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_TL_TO_BR )
		{
			if( nRnNum <= 1 )
			{
				dx = w;
			}
			else
			{
				dx = (m_pRdNameList[i].rect.right - m_pRdNameList[i].rect.left - w)/(nRnNum - 1);
			}
			dy = h;
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_TR_TO_BL )
		{
			if( nRnNum <= 1 )
			{
				dx = -w;
			}
			else
			{
				dx = -(m_pRdNameList[i].rect.right - m_pRdNameList[i].rect.left - w)/(nRnNum - 1);
			}
			dy = h;
			x = m_pRdNameList[i].rect.right - h;
			y = m_pRdNameList[i].rect.top;
		}
		else
		{
			continue;
		}
		nuINT nLen = 1;
		nuINT nRnIdx = 0;
		NumberShift=dy*3/16;
		for(nuINT j = 0; (j < nRnNum) && (nRnIdx < nRnNum); j++)
		{
			// *3/4琌琵俱ゅぃ穦び糴肞
			sx = x + j*dx;//*3/4;
			sy = y + j*dy;//*3/4;
			nLen = 1;
			if( rnName[nRnIdx] >= VALUE_0 && rnName[nRnIdx] <= VALUE_9 )
			{
				sx = sx+NumberShift;
			}
			if( rnName[nRnIdx] == VALUE_Dot )
			{
				sx = sx+NumberShift;
			}
/*
			for(nuINT m = nRnIdx; m < nRnNum; m++ )
			{
				if( rnName[m] >= VALUE_0 && rnName[m] <= VALUE_9 )
				{
					nLen = 1 + m - nRnIdx;
				}
				else
				{
					break;
				}
			}
*/
			g_pDMLibGDI->GdiSetTextColor(col);
//			g_pDMLibGDI->GdiExtTextOutW(sx, sy, &rnName[nRnIdx], nLen);
			g_pDMLibGDI->GdiExtTextOutWEx(sx, sy, &rnName[nRnIdx], nLen, nuTRUE, rimCol);
			nRnIdx += nLen;
		}
	}
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	return nuTRUE;
}


nuBOOL CRoadName::DrawRoadNameCN_DX()
{
	m_nRdIconIdx = 0;
	nuBOOL bRdIconRepeat = nuFALSE;
	//
	RNEXTEND rnExtend = {0};
	nuWCHAR rnName[RN_MAX_LEN] = {0};
	nuBYTE nNowRdType = -1;
	nuLONG pInOut = 0;
	nuINT h, w, sx, sy, dx, dy, nRnNum, x, y,NumberShift;
	h= w= sx= sy= dx= dy= nRnNum= x= y=NumberShift=0;
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	nuCOLORREF col = {0};
	nuCOLORREF rimCol = {0};
	nuroFONT nuLogFont = {0};
	nuBYTE n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	for( nuINT i = m_nRdNameIndex - 1; i >= 0; i-- )
	{
		nuMemset(rnName, 0, sizeof(rnName));
		if( !g_pDMLibFS->FS_GetRoadName(m_pRdNameList[i].rdDwIdx, 
			m_pRdNameList[i].rdNameAddr, m_pRdNameList[i].rdType, rnName, RN_MAX_LEN, &rnExtend) )
		{
			continue;
		}
	
		if( nNowRdType != m_pRdNameList[i].rdType )
		{
			if( g_pDMLibMT->MT_SelectObject(DRAW_TYPE_RNFONT, n3Dmode, &m_pRoadSetting[m_pRdNameList[i].rdType], &pInOut) != DRAW_OBJ_ROADNAME )
			{
				continue;
			}
			nNowRdType = m_pRdNameList[i].rdType;
			h = m_pRoadSetting[nNowRdType].nFontHeight * 3 / 4;
			w = m_pRoadSetting[nNowRdType].nFontWidth * 3 / 4;
			if( w <= 0 )
			{
				w = h;
			}
			else
			{
				w *= 2;
			}
			col = nuRGB( m_pRoadSetting[nNowRdType].nFontColorR,
						 m_pRoadSetting[nNowRdType].nFontColorG,
						 m_pRoadSetting[nNowRdType].nFontColorB );
			rimCol	= nuRGB( m_pRoadSetting[nNowRdType].nFontRimColorR,
							 m_pRoadSetting[nNowRdType].nFontRimColorG, 
							 m_pRoadSetting[nNowRdType].nFontRimColorB );
		}
		// --- added @ 2012.11.26 ---
		bRdIconRepeat = nuFALSE;
		for( nuWORD nIdx = 0; nIdx < m_nRdIconIdx; nIdx++ )
		{
			if( m_pRdIconList[nIdx].nIconIndex == rnExtend.nIconNum && 
				m_pRdIconList[nIdx].byIconType == rnExtend.nIconType )
			{
				bRdIconRepeat = nuTRUE;
				break;
			}
		}
		if( bRdIconRepeat )
		{
			bRdIconRepeat = nuFALSE;
			continue;
		}
		// --------------------------
#ifdef VALUE_EMGRT
		if( rnExtend.nIconType != 0 )
#else
		if( 1 == rnExtend.nIconType || 2 == rnExtend.nIconType )
#endif
		{
			sx = m_pRdNameList[i].ptStart.x;
			sy = m_pRdNameList[i].ptStart.y;
			nuWCHAR wsRd[256] = {0};
			g_pDMLibFS->FS_GetRoadIconInfo( m_pRdNameList[i].rdDwIdx,
				g_pDMLibFS->pGdata->uiSetData.nScaleValue, 
				g_pDMLibFS->pGdata->uiSetData.nBglStartScaleValue, 
				&rnExtend, 
				wsRd );
			if( wsRd[0] && g_pDMLibGDI->GdiDrawIcon(sx - 24, sy - 24, rnExtend.nIconType) )
			{
#ifdef VALUE_EMGRT
				DrawRoadIcon(sx - 24, sy - 24, rnExtend.nIconType, wsRd);
#else
				DrawRoadIconTW(sx - 24, sy - 24, rnExtend.nIconType, wsRd);
#endif
				m_pRdIconList[m_nRdIconIdx].byIconType = rnExtend.nIconType;
				m_pRdIconList[m_nRdIconIdx].nIconIndex = rnExtend.nIconNum;
				m_nRdIconIdx++;
			}
			continue;
		}
		//
		nRnNum = rnExtend.nRNLen/2;
		NumberShift=0;
		/*for test
		nuroRECT* pRt = ColNameRectCN( &m_pRdNameList[i].rect, 
									   m_pRdNameList[i].rdAlign, 
									   m_pRdNameList[i].rdNameLen/2, 
									   h);
		if( NULL != pRt )
		{
			for(nuBYTE nn = 0; nn < m_pRdNameList[i].rdNameLen/2; ++nn)
			{
				nuroPOINT pppt[5];
				pppt[0].x = pppt[1].x = pRt[nn].left;
				pppt[1].y = pppt[2].y = pRt[nn].top;
				pppt[2].x = pppt[3].x = pRt[nn].right;
				pppt[3].y = pppt[0].y = pRt[nn].bottom;
				pppt[4]   = pppt[0];
				g_pDMLibGDI->GdiPolyline(pppt, 5);
			}
			delete[] pRt;
			pRt=NULL;
		}
		*/
		////////////
		if( m_pRdNameList[i].rdAlign == RDNAME_L_TO_R )
		{
			sx = m_pRdNameList[i].rect.left;
			sy = m_pRdNameList[i].rect.top;
			/*
			g_pDMLibGDI->GdiSetTextColor(rimCol);
			for(nuINT k = -1; k <= 1; k++)
			{
				for(nuINT l = -1; l <= 1; l++)
				{
					if( l == 0 && k == 0 )
					{
						continue;
					}
					g_pDMLibGDI->GdiExtTextOutW(sx+k, sy+l, rnName, nRnNum);
				}
			}
			*/
			g_pDMLibGDI->GdiSetTextColor(col);
//			g_pDMLibGDI->GdiExtTextOutW(sx, sy, rnName, rnExtend.nRNLen/2);
			g_pDMLibGDI->GdiExtTextOutWEx(sx, sy, rnName, rnExtend.nRNLen/2, nuTRUE, rimCol);
			continue;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_T_TO_D )
		{
			dx = 0;
			dy = h*7/5;//Louis 2012/02/20 dy = h;
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
			
			nuINT nLen = 1;
			nuINT nRnIdx = 0;
			NumberShift=dy*3/16;
			for(nuINT j = 0; (j < nRnNum) && (nRnIdx < nRnNum); j++)
			{
				sx = x + j*dx;
				sy = y + j*dy;
				nLen = 1;
				if( rnName[nRnIdx] >= VALUE_0 && rnName[nRnIdx] <= VALUE_9 )
				{
					sx = sx+NumberShift;
				}
				if( rnName[nRnIdx] == VALUE_Dot )
				{
					sx = sx+NumberShift;
				}
				g_pDMLibGDI->GdiSetTextColor(col);
				g_pDMLibGDI->GdiExtTextOutWEx(sx, sy, &rnName[nRnIdx], nLen, nuTRUE, rimCol);
			nuroRECT* pRt = ColNameRectCN(
							&m_pRdNameList[i].rect, 
							m_pRdNameList[i].rdAlign, 
							m_pRdNameList[i].rdNameLen/2, 
							h);
			if( NULL != pRt )
			{	
				for (nuINT j = 0, nRnIdx = 0; j < nRnNum; j++, nRnIdx++)
				{
					g_pDMLibGDI->GdiExtTextOutWEx(pRt[j].left, pRt[j].top, 
						&rnName[nRnIdx], 1, nuTRUE, rimCol);
				}
				delete[] pRt;
				pRt = NULL;
			}
		}
	} // for loop
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	return nuTRUE;
}

}
nuBOOL CRoadName::DrawRoadNameCN()
{
	m_nRdIconIdx = 0;
	nuBOOL bRdIconRepeat = nuFALSE;
	RNEXTEND rnExtend = {0};
	nuWCHAR rnName[RN_MAX_LEN] = {0};
	nuBYTE nNowRdType = -1;
	nuLONG pInOut = 0;
	nuINT h, w, sx, sy, dx, dy, nRnNum, x, y,NumberShift;
	h= w= sx= sy= dx= dy= nRnNum= x= y=NumberShift=0;
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	nuCOLORREF col = {0};
	nuCOLORREF rimCol = {0};
	nuroFONT nuLogFont = {0};
	nuBYTE n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	ROADSETTING RoadSetting = {0};
	for( nuINT i = m_nRdNameIndex - 1; i >= 0; i-- )
	{
		nuMemset(rnName, 0, sizeof(rnName));
		if( !g_pDMLibFS->FS_GetRoadName(m_pRdNameList[i].rdDwIdx, m_pRdNameList[i].rdNameAddr, m_pRdNameList[i].rdType, rnName, RN_MAX_LEN, &rnExtend) )
		{
			continue;
		}
		if( nNowRdType != m_pRdNameList[i].rdType )
		{
			if(g_pDMLibFS->pUserCfg->bBigMapRoadFont)
			{
				RoadSetting.nFontHeight = m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight + m_nAddRoadFontSize;
			}
			else
			{
				RoadSetting.nFontHeight = m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight;
			}
			if( g_pDMLibMT->MT_SelectObject(DRAW_TYPE_RNFONT, n3Dmode, &RoadSetting, &pInOut) != DRAW_OBJ_ROADNAME )
			{
				continue;
			}
			nNowRdType = m_pRdNameList[i].rdType;
			h = m_pRoadSetting[nNowRdType].nFontHeight * 3 / 4;
			w = m_pRoadSetting[nNowRdType].nFontWidth * 3 / 4;
			if( w <= 0 )
			{
				w = h;
			}
			else
			{
				w *= 2;
			}
			col = nuRGB( m_pRoadSetting[nNowRdType].nFontColorR,
				m_pRoadSetting[nNowRdType].nFontColorG,
				m_pRoadSetting[nNowRdType].nFontColorB );
			rimCol	= nuRGB( m_pRoadSetting[nNowRdType].nFontRimColorR,
				m_pRoadSetting[nNowRdType].nFontRimColorG, 
				m_pRoadSetting[nNowRdType].nFontRimColorB );
		}
		bRdIconRepeat = nuFALSE;
		for( nuWORD nIdx = 0; nIdx < m_nRdIconIdx; nIdx++ )
		{
			if( m_pRdIconList[nIdx].nIconIndex == rnExtend.nIconNum && 
				m_pRdIconList[nIdx].byIconType == rnExtend.nIconType )
			{
				bRdIconRepeat = nuTRUE;
				break;
			}
		}
		if( bRdIconRepeat )
		{
			bRdIconRepeat = nuFALSE;
			continue;
		}
#ifdef VALUE_EMGRT
		if( rnExtend.nIconType != 0 )
#else
		if( 1 == rnExtend.nIconType || 2 == rnExtend.nIconType )
#endif
		{
			sx = m_pRdNameList[i].ptStart.x;
			sy = m_pRdNameList[i].ptStart.y;
			nuWCHAR wsRd[256] = {0};
			g_pDMLibFS->FS_GetRoadIconInfo( m_pRdNameList[i].rdDwIdx,
				g_pDMLibFS->pGdata->uiSetData.nScaleValue, 
				g_pDMLibFS->pGdata->uiSetData.nBglStartScaleValue, 
				&rnExtend, 
				wsRd );
			if( wsRd[0] && g_pDMLibGDI->GdiDrawIcon(sx - 24, sy - 24, rnExtend.nIconType) )
			{
#ifdef VALUE_EMGRT
				DrawRoadIcon(sx - 24, sy - 24, rnExtend.nIconType, wsRd);
#else
				DrawRoadIconTW(sx - 24, sy - 24, rnExtend.nIconType, wsRd);
#endif
				m_pRdIconList[m_nRdIconIdx].byIconType = rnExtend.nIconType;
				m_pRdIconList[m_nRdIconIdx].nIconIndex = rnExtend.nIconNum;
				m_nRdIconIdx++;
			}
			continue;
		}
		nRnNum = rnExtend.nRNLen/2;
		NumberShift=0;
		if( m_pRdNameList[i].rdAlign == RDNAME_L_TO_R )
		{
			sx = m_pRdNameList[i].rect.left;
			sy = m_pRdNameList[i].rect.top;
			g_pDMLibGDI->GdiSetTextColor(col);
			g_pDMLibGDI->GdiExtTextOutWEx(sx, sy, rnName, rnExtend.nRNLen/2, nuTRUE, rimCol);
			continue;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_T_TO_D )
		{
			dx = 0;
			dy = h*7/5;//Louis 2012/02/20 dy = h;
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
			nuINT nLen = 1;
			nuINT nRnIdx = 0;
			NumberShift=dy*3/16;
			for(nuINT j = 0; (j < nRnNum) && (nRnIdx < nRnNum); j++)
			{
				sx = x + j*dx;
				sy = y + j*dy;
				nLen = 1;
				if( rnName[nRnIdx] >= VALUE_0 && rnName[nRnIdx] <= VALUE_9 )
				{
					sx = sx + NumberShift;
				}
				if( rnName[nRnIdx] == VALUE_Dot )
				{
					sx = sx + NumberShift;
				}
				g_pDMLibGDI->GdiSetTextColor(col);
				g_pDMLibGDI->GdiExtTextOutWEx(sx, sy, &rnName[nRnIdx], nLen, nuTRUE, rimCol);
				nRnIdx += nLen;
			}
		}
		else
		{
			nuroRECT* pRt = ColNameRectCN( &m_pRdNameList[i].rect, 
				m_pRdNameList[i].rdAlign, 
				m_pRdNameList[i].rdNameLen/2, 
				h);
			if( NULL != pRt )
			{
				nuINT nLen = 1;
				nuINT nRnIdx = 0;
				for(nuINT j = 0; (j < nRnNum) && (nRnIdx < nRnNum); j++)
				{
					nLen = 1;
					if( rnName[nRnIdx] >= VALUE_0 && rnName[nRnIdx] <= VALUE_9 )
					{
						sx = sx+NumberShift;
					}
					if( rnName[nRnIdx] == VALUE_Dot )
					{
						sx = sx+NumberShift;
					}
					g_pDMLibGDI->GdiSetTextColor(col);
					g_pDMLibGDI->GdiExtTextOutWEx(pRt[j].left, pRt[j].top, &rnName[nRnIdx], nLen, nuTRUE, rimCol);
					nRnIdx += nLen;
				}
				delete[] pRt;
				pRt=NULL;
			}
		}
	}
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	return nuTRUE;
}


nuBOOL CRoadName::DrawRoadNameCHEx()
{
	RNEXTEND rnExtend = {0};
	nuWCHAR rnName[RN_MAX_LEN] = {0};
	nuBYTE nNowRdType = -1;
	nuLONG pInOut = 0;
	nuINT h, w, sx, sy, dx, dy, nRnNum, x, y,NumberShift;
	h = w = sx= sy= dx= dy= nRnNum= x= y=NumberShift=0;
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	nuCOLORREF col = {0};
	nuCOLORREF rimCol = {0};
	nuBYTE n3Dmode=0;
	if( m_nMapType == MAP_DEFAULT )
	{
		n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	}
	else//Zoom Screen
	{
		n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	}

	for( nuINT i = m_nRdNameIndex - 1; i >= 0; i-- )
	{
		nuMemset(rnName, 0, sizeof(rnName));
		if( !g_pDMLibFS->FS_GetRoadName(m_pRdNameList[i].rdDwIdx, m_pRdNameList[i].rdNameAddr, m_pRdNameList[i].rdType, rnName, RN_MAX_LEN, &rnExtend) )
		{
			continue;
		}
		
		if( nNowRdType != m_pRdNameList[i].rdType )
		{
			if( g_pDMLibMT->MT_SelectObject(DRAW_TYPE_RNFONT, n3Dmode, &m_pRoadSetting[m_pRdNameList[i].rdType], &pInOut) != DRAW_OBJ_ROADNAME )
			{
				continue;
			}
			nNowRdType = m_pRdNameList[i].rdType;
			h = m_pRoadSetting[nNowRdType].nFontHeight;
			w = m_pRoadSetting[nNowRdType].nFontWidth;
			if( w <= 0 )
			{
				w = h;
			}
			else
			{
				w *= 2;
			}
			col = nuRGB( m_pRoadSetting[nNowRdType].nFontColorR,
						 m_pRoadSetting[nNowRdType].nFontColorG,
						 m_pRoadSetting[nNowRdType].nFontColorB );
			rimCol	= nuRGB( m_pRoadSetting[nNowRdType].nFontRimColorR,
							 m_pRoadSetting[nNowRdType].nFontRimColorG, 
							 m_pRoadSetting[nNowRdType].nFontRimColorB );
		}
		if( rnExtend.nIconType != 0 && rnExtend.nIconNum != 0 )
		{
			sx = (m_pRdNameList[i].rect.left + m_pRdNameList[i].rect.right)/2;
			sy = (m_pRdNameList[i].rect.top + m_pRdNameList[i].rect.bottom)/2;
			g_pDMLibGDI->GdiDrawIcon(sx - 12, sy - 10, rnExtend.nIconType);
			nuWCHAR wsRd[256] = {0};
			nuItow(rnExtend.nIconNum, wsRd, 10);
			nuroRECT rect_Tmp = {0};
			rect_Tmp.left	=	sx -12;
			rect_Tmp.top	=	sy -12;
			rect_Tmp.right	=	sx +12;
			rect_Tmp.bottom	=	sy +12;
			
			g_pDMLibGDI->GdiSetTextColor( nuRGB(255, 255, 255)*(rnExtend.nIconType - 1) );
			g_pDMLibGDI->GdiDrawTextW( wsRd, 
				nuWcslen(wsRd),
				&rect_Tmp, 
				NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER );
			continue;
		}

		nRnNum = rnExtend.nRNLen/2;
		NumberShift=0;

		if( m_pRdNameList[i].rdAlign == RDNAME_L_TO_R )
		{
			dx = w;
			dy = 0;
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_T_TO_D )
		{
			dx = 0;
			dy = h;
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
		}
		else if(m_pRdNameList[i].rdAlign == RDNAME_LT_TO_RB )
		{
			dx = w;
			if( nRnNum <= 1 )
			{
				dy = h;
			}
			else
			{
				dy = (m_pRdNameList[i].rect.bottom - m_pRdNameList[i].rect.top - h)/(nRnNum - 1);
			}
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_LB_TO_RT )
		{
			dx = w;
			if( nRnNum <= 1 )
			{
				dy = -h;
			}
			else
			{
				dy = -(m_pRdNameList[i].rect.bottom - m_pRdNameList[i].rect.top - h)/(nRnNum - 1);
			}
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.bottom - h;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_TL_TO_BR )
		{
			if( nRnNum <= 1 )
			{
				dx = w;
			}
			else
			{
				dx = (m_pRdNameList[i].rect.right - m_pRdNameList[i].rect.left - w)/(nRnNum - 1);
			}
			dy = h;
			x = m_pRdNameList[i].rect.left;
			y = m_pRdNameList[i].rect.top;
		}
		else if( m_pRdNameList[i].rdAlign == RDNAME_TR_TO_BL )
		{
			if( nRnNum <= 1 )
			{
				dx = -w;
			}
			else
			{
				dx = -(m_pRdNameList[i].rect.right - m_pRdNameList[i].rect.left - w)/(nRnNum - 1);
			}
			dy = h;
			x = m_pRdNameList[i].rect.right - h;
			y = m_pRdNameList[i].rect.top;
		}
		else
		{
			continue;
		}
		nuINT nLen = 1;
		nuINT nRnIdx = 0;
		NumberShift=dy*3/16;
		dx = dx*4/5;
		dy = dy*4/5;
		for(nuINT j = 0; (j < nRnNum) && (nRnIdx < nRnNum); j++)
		{
			sx = x + j*dx;
			sy = y + j*dy;
			nLen = 1;
			if( rnName[nRnIdx] >= VALUE_0 && rnName[nRnIdx] <= VALUE_9 )
			{
				sx = sx+NumberShift;
			}
			if( rnName[nRnIdx] == VALUE_Dot )
			{
				sx = sx+NumberShift;
			}
			g_pDMLibGDI->GdiSetTextColor(col);
			g_pDMLibGDI->GdiExtTextOutWEx(sx, sy, &rnName[nRnIdx], nLen, nuTRUE, rimCol);
			nRnIdx += nLen;
		}
	}
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	return nuTRUE;
}

#define GON_POINTS_NUM						7
nuBOOL CRoadName::DrawRoadName3D()//Draw in the screen from top to bottom
{
	m_nRdIconIdx = 0;
	nuBOOL bRdIconRepeat = nuFALSE;
	//
	if( m_nRdNameIndex == 0 )
	{
		return nuFALSE;
	}
	nuBOOL *pbDraw = new nuBOOL[m_nRdNameIndex];
	if( pbDraw == NULL )
	{
		return nuFALSE;
	}
	nuINT i;
	for(i = 0; i < m_nRdNameIndex; i++)
	{
		pbDraw[i]	= nuFALSE;
	}
	nuINT sx=0;
	nuINT sy=0;
	NURORECT rtText = {0};
	NUROPOINT ptGon[GON_POINTS_NUM+1] = {0};
	nuLONG yValue=0;
	nuLONG pInOut = 0;
	nuWCHAR rnName[RN_MAX_LEN] = {0};
	RNEXTEND rnExtend = {0};
	nuINT nRnNum = 0;
	nuINT nOldMode = g_pDMLibGDI->GdiSetBkMode(NURO_TRANSPARENT);
	nuBYTE n3Dmode = 0;
	nuINT TmpRop2 = 0;
	nuCOLORREF col = {0};
	nuCOLORREF rimCol = {0};
	if( m_nMapType == MAP_DEFAULT )
	{
		n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	}
	else//Zoom Screen
	{
		n3Dmode = g_pDMGdata->uiSetData.b3DMode;
	}
	for( nuINT j = 0; j < m_nRdNameIndex; j++ )
	{
		yValue = 10000;
		i = j;
		for( nuINT k = 0; k < m_nRdNameIndex; k++)
		{
			if( !pbDraw[k] && m_pRdNameList[k].ptStart.y < yValue )
			{
				i = k;
				yValue = m_pRdNameList[k].ptStart.y;
			}
		}
		if( !pbDraw[i] )//Need to draw
		{
			pbDraw[i] = nuTRUE;
			nuMemset(rnName, 0, sizeof(rnName));
			if (!g_pDMLibFS->FS_GetRoadName(m_pRdNameList[i].rdDwIdx, m_pRdNameList[i].rdNameAddr, 
											m_pRdNameList[i].rdType, rnName, RN_MAX_LEN, &rnExtend))
			{
				continue;
			}
			// ----- Added by XiaoQin @ 2012.10.29 -----
			ROADSETTING RoadSetting = {0};
			RoadSetting = m_pRoadSetting[m_pRdNameList[i].rdType];
			if(g_pDMLibFS->pUserCfg->bBigMapRoadFont)
			{
				if( m_pRdNameList[i].ptStart.y < g_pDMGdata->transfData.nBmpHeight/3 )
				{
					RoadSetting.nFontHeight = m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight - 6 + m_nAddRoadFontSize;
				}
				else if( m_pRdNameList[i].ptStart.y >= g_pDMGdata->transfData.nBmpHeight/3 &&
					     m_pRdNameList[i].ptStart.y < g_pDMGdata->transfData.nBmpHeight*2/3 )
				{
					RoadSetting.nFontHeight = m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight - 2 + m_nAddRoadFontSize;
				}
				else
				{
					RoadSetting.nFontHeight = m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight + 2 + m_nAddRoadFontSize;
				}
			}
			else
			{
				if( m_pRdNameList[i].ptStart.y < g_pDMGdata->transfData.nBmpHeight/3 )
				{
					RoadSetting.nFontHeight = m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight - 6;
				}
				else if( m_pRdNameList[i].ptStart.y >= g_pDMGdata->transfData.nBmpHeight/3 &&
						 m_pRdNameList[i].ptStart.y < g_pDMGdata->transfData.nBmpHeight*2/3 )
				{
					RoadSetting.nFontHeight = m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight - 2;
				}
				else
				{
					RoadSetting.nFontHeight = m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight + 2;
				}
			}
			// -----------------------------------------
			if( g_pDMLibMT->MT_SelectObject(DRAW_TYPE_RNFONTANDBANNER, n3Dmode, &RoadSetting, &pInOut) != DRAW_OBJ_ROADNAME )
			{
				continue;
			}
			col = nuRGB( m_pRoadSetting[m_pRdNameList[i].rdType].nFontColorR,
				         m_pRoadSetting[m_pRdNameList[i].rdType].nFontColorG,
				         m_pRoadSetting[m_pRdNameList[i].rdType].nFontColorB );
			rimCol	= nuRGB( m_pRoadSetting[m_pRdNameList[i].rdType].nFontRimColorR,
				             m_pRoadSetting[m_pRdNameList[i].rdType].nFontRimColorG, 
				             m_pRoadSetting[m_pRdNameList[i].rdType].nFontRimColorB );
			// --- added @ 2012.11.26 ---
			bRdIconRepeat = nuFALSE;
			for( nuWORD nIdx = 0; nIdx < m_nRdIconIdx; nIdx++ )
			{
				if( m_pRdIconList[nIdx].nIconIndex == rnExtend.nIconNum && 
					m_pRdIconList[nIdx].byIconType == rnExtend.nIconType )
				{
					bRdIconRepeat = nuTRUE;
					break;
				}
			}
			if( bRdIconRepeat )
			{
				bRdIconRepeat = nuFALSE;
				continue;
			}
			// --------------------------
#ifdef VALUE_EMGRT
			if( rnExtend.nIconType != 0 )
#else
			if( 1 == rnExtend.nIconType || 2 == rnExtend.nIconType )
#endif
			{
				sx = m_pRdNameList[i].ptStart.x;
				sy = m_pRdNameList[i].ptStart.y;
				nuWCHAR wsRd[256] = {0};
				g_pDMLibFS->FS_GetRoadIconInfo( m_pRdNameList[i].rdDwIdx, 
					g_pDMLibFS->pGdata->uiSetData.nScaleValue,
					g_pDMLibFS->pGdata->uiSetData.nBglStartScaleValue, 
					&rnExtend, 
					wsRd );
				if( wsRd[0] && g_pDMLibGDI->GdiDrawIcon(sx - 24, sy - 24, rnExtend.nIconType) )
				{
#ifdef VALUE_EMGRT
					DrawRoadIcon(sx - 24, sy - 24, rnExtend.nIconType, wsRd);
#else
					DrawRoadIconTW(sx - 24, sy - 24, rnExtend.nIconType, wsRd);
#endif
					m_pRdIconList[m_nRdIconIdx].byIconType = rnExtend.nIconType;
					m_pRdIconList[m_nRdIconIdx].nIconIndex = rnExtend.nIconNum;
					m_nRdIconIdx++;
				}				
				continue;
			}
			nRnNum = rnExtend.nRNLen/2;
			rtText = m_pRdNameList[i].rect;
			rtText.bottom -= BANNER_HEIGHT;
			g_pDMLibGDI->GdiDrawTextW(rnName, nRnNum, &rtText, NURO_DT_CENTER | NURO_DT_CALCRECT | NURO_DT_SINGLELINE);
			rtText.left -= BANNER_LT_EX;
			rtText.top -= BANNER_TP_EX;
			rtText.right += BANNER_RT_EX;
			rtText.bottom += BANNER_BT_EX;
			ptGon[0] = m_pRdNameList[i].ptStart;
			ptGon[1].x = ptGon[0].x - BANNER_L_SPACE;
			ptGon[1].y = ptGon[0].y - BANNER_HEIGHT;
			ptGon[2].x = ptGon[0].x - (rtText.right - rtText.left)/BANNER_CUT;
			ptGon[2].y = ptGon[1].y;
			if( ptGon[2].x > ptGon[1].x )
			{
				ptGon[2].x = ptGon[1].x;
			}
			ptGon[3].x = ptGon[2].x;
			ptGon[3].y = ptGon[2].y - (rtText.bottom - rtText.top);
			ptGon[4].x = ptGon[3].x + (rtText.right - rtText.left);
			ptGon[4].y = ptGon[3].y;
			ptGon[5].x = ptGon[4].x;
			ptGon[5].y = ptGon[2].y;
			ptGon[6].x = ptGon[0].x + BANNER_R_SPACE;
			ptGon[6].y = ptGon[5].y;
			if( ptGon[6].x > ptGon[5].x )
			{
				ptGon[6].x = ptGon[5].x;
			}
			rtText.left		= ptGon[2].x;
			rtText.right	= ptGon[4].x;
			rtText.top		= ptGon[3].y;
			rtText.bottom	= ptGon[2].y;
			ptGon[7]		= ptGon[0];

			if( 1 )
			{
				NUROPOINT ptGonEx[4] = {0};
				ptGonEx[0] = m_pRdNameList[i].ptStart;
				ptGonEx[1].x = ptGonEx[0].x - 5;
				ptGonEx[1].y = ptGonEx[0].y - BANNER_HEIGHT - 1;
				ptGonEx[2].x = ptGonEx[0].x + 5;
				ptGonEx[2].y = ptGonEx[1].y;
				ptGonEx[3] = ptGonEx[0];
				TmpRop2 = m_pRoadSetting[m_pRdNameList[i].rdType].nFontBgTransparent % 17;
				g_pDMLibGDI->GdiPolygonEx(ptGonEx, 4, TmpRop2);
			}
			else
			{
				/*
				if( m_pRoadSetting->nFontBgTransparent )
				{
				TmpRop2 = NURO_BMP_TYPE_MIXA;//g_libGDI.GdiSetROP2(NURO_TRANSPARENT);
				}
				else
				{
				TmpRop2 = NURO_BMP_TYPE_MIXX;
				}
				*/
				TmpRop2 = m_pRoadSetting[m_pRdNameList[i].rdType].nFontBgTransparent % 17;
				g_pDMLibGDI->GdiPolygonEx(ptGon, GON_POINTS_NUM, TmpRop2);
			}
			rtText.left += BANNER_LT_EX;
			rtText.top += BANNER_TP_EX;
			rtText.right -= BANNER_RT_EX;
			rtText.bottom -= BANNER_BT_EX;
			//g_pDMLibGDI->GdiDrawTextW(rnName, nRnNum, &rtText, NURO_DT_CENTER | NURO_DT_SINGLELINE | NURO_DT_VCENTER);
			g_pDMLibGDI->GdiSetTextColor(col);
			g_pDMLibGDI->GdiExtTextOutWEx(rtText.left, rtText.top-2, &rnName[0], nRnNum, nuTRUE, rimCol);
		}
	}
	g_pDMLibGDI->GdiSetBkMode(nOldMode);
	delete []pbDraw;
	pbDraw=NULL;
	return nuTRUE;
}

nuBOOL CRoadName::DrawRoadNameEN()
{
	return nuTRUE;
}

nuBOOL CRoadName::CheckRectCN(nuroRECT *lpRect, nuBYTE nAlign, nuBYTE nNameLen, nuBYTE nFtSize)
{
	if( 0 == m_nRdNameIndex )
	{
		return nuTRUE;
	}
	nuBOOL bRes = nuTRUE;
	nuBYTE nNameNum1 = nNameLen / 2;
	nuroRECT* pRect = ColNameRectCN(lpRect, nAlign, nNameNum1, nFtSize);
	for(nuWORD i = 0; i < m_nRdNameIndex; i++ )
	{
		nuBYTE nNameNum2 = m_pRdNameList[i].rdNameLen/2;
		nuroRECT* pRtName = ColNameRectCN( &m_pRdNameList[i].rect, 
											m_pRdNameList[i].rdAlign, 
											nNameNum2, 
											m_pRoadSetting[m_pRdNameList[i].rdType].nFontHeight );
		if( NULL == pRtName )
		{
			if( NULL == pRect )
			{
				if( nuRectCoverRect(lpRect, &m_pRdNameList[i].rect) )
				{
					bRes = nuFALSE;
				}
			}
			else
			{
				for(nuBYTE j = 0; j < nNameNum1; ++j)
				{
					if( nuRectCoverRect(&pRect[j], &m_pRdNameList[i].rect) )
					{
						bRes = nuFALSE;
						break;
					}
				}
			}
		}
		else
		{
			if( NULL == pRect )
			{
				for(nuBYTE j = 0; j < nNameNum2; ++j)
				{
					if( nuRectCoverRect(lpRect, &pRtName[j]) )
					{
						bRes = nuFALSE;
						break;
					}
				}
			}
			else
			{
				for(nuBYTE j = 0; j < nNameNum1; ++j)
				{
					for(nuBYTE k = 0; k < nNameNum2; ++k)
					{
						if( nuRectCoverRect(&pRect[j], &pRtName[k]) )
						{
							bRes = nuFALSE;
							break;
						}
					}
					if( !bRes )
					{
						break;
					}
				}
			}
			delete[] pRtName;
			pRtName=NULL;
		}
		if( !bRes )
		{
			break;
		}
	}
	if( NULL != pRect )
	{
		delete[] pRect;
		pRect=NULL;
	}
	return bRes;
}

nuroRECT* CRoadName::ColNameRectCN(nuroRECT *lpRect, nuBYTE nAlign, nuBYTE nNameNum, nuBYTE nFtSize)
{
	if( RDNAME_L_TO_R == nAlign ||
		RDNAME_T_TO_D == nAlign ||
		1 >= nNameNum )
	{
		return NULL;
	}
	nuroRECT* pRtName = new nuroRECT[nNameNum];
	if( NULL == pRtName )
	{
		return NULL;
	}

	nFtSize = nFtSize*7/5;//Louis 2012/02/20  +4;
	
	for( nuBYTE i = 0; i < nNameNum; ++i )
	{
		nuLONG dx = (lpRect->right - lpRect->left) / nNameNum;
		nuLONG dy = (lpRect->bottom - lpRect->top) / nNameNum;
		if( RDNAME_LT_TO_RB == nAlign )
		{
			if( 0 == i )
			{
				pRtName[0].left		= lpRect->left;
				pRtName[0].top		= lpRect->top;
				pRtName[0].right	= lpRect->left + NURO_MAX(dx, nFtSize);
				pRtName[0].bottom	= lpRect->top + NURO_MAX(dy, nFtSize);
			}
			else
			{
				pRtName[i].left		= pRtName[i-1].left + NURO_MAX(dx, nFtSize);//dx;
				pRtName[i].right	= pRtName[i-1].right + NURO_MAX(dx, nFtSize);//dx;
				pRtName[i].top		= pRtName[i-1].top + dy;
				pRtName[i].bottom	= pRtName[i-1].bottom + dy;
			}
		}
		else if( RDNAME_LB_TO_RT == nAlign )
		{
			if( 0 == i )
			{
				pRtName[0].left		= lpRect->left;
				pRtName[0].top		= lpRect->bottom - NURO_MAX(dy, nFtSize);
				pRtName[0].right	= lpRect->left + NURO_MAX(dx, nFtSize);
				pRtName[0].bottom	= lpRect->bottom;
			}
			else
			{
				pRtName[i].left		= pRtName[i-1].left + NURO_MAX(dx, nFtSize);//dx;
				pRtName[i].right	= pRtName[i-1].right + NURO_MAX(dx, nFtSize);//dx;
				pRtName[i].top		= pRtName[i-1].top - dy;
				pRtName[i].bottom	= pRtName[i-1].bottom - dy;
			}
		}
		else if( RDNAME_TL_TO_BR == nAlign )
		{
			if( 0 == i )
			{
				pRtName[0].left		= lpRect->left;
				pRtName[0].top		= lpRect->top;
				pRtName[0].right	= lpRect->left + NURO_MAX(dx, nFtSize);
				pRtName[0].bottom	= lpRect->top + NURO_MAX(dy, nFtSize);
			}
			else
			{
				pRtName[i].left		= pRtName[i-1].left + dx;
				pRtName[i].right	= pRtName[i-1].right + dx;
				pRtName[i].top		= pRtName[i-1].top + NURO_MAX(dy, nFtSize);//dy;
				pRtName[i].bottom	= pRtName[i-1].bottom + NURO_MAX(dy, nFtSize);//dy;
			}
		}
		else if( RDNAME_TR_TO_BL == nAlign )
		{
			if( 0 == i )
			{
				pRtName[0].left		= lpRect->right - NURO_MAX(dx, nFtSize);
				pRtName[0].top		= lpRect->top;
				pRtName[0].right	= lpRect->right;
				pRtName[0].bottom	= lpRect->top + NURO_MAX(dy, nFtSize);
			}
			else
			{
				pRtName[i].left		= pRtName[i-1].left - dx;
				pRtName[i].right	= pRtName[i-1].right - dx;
				pRtName[i].top		= pRtName[i-1].top + NURO_MAX(dy, nFtSize);//dy;
				pRtName[i].bottom	= pRtName[i-1].bottom + NURO_MAX(dy, nFtSize);//dy;
			}
		}
		else
		{
			delete[] pRtName;
			pRtName=NULL;
			return NULL;
		}
	}
	return pRtName;
}

nuBOOL CRoadName::DrawRoadIcon(nuLONG sx, nuLONG sy, nuBYTE byIconType, nuWCHAR* wsRdNum)
{
	//Set Font Color
	if( 15 == byIconType || 14 == byIconType )
	{
		g_pDMLibGDI->GdiSetTextColor(nuRGB(0, 0, 0)); //Black
	}
	else
	{
		g_pDMLibGDI->GdiSetTextColor(nuRGB(255, 255, 255)); //White
	}
	//
	nuroRECT rect_Tmp = {0};
	NUROFONT nuFont = {0};
	rect_Tmp.left =	sx;
	rect_Tmp.top  = sy;
	if( 11 == byIconType || 13 == byIconType )
	{
		if( g_pDMLibFS->pGdata->transfData.nBmpWidth == 800 ||
			g_pDMLibFS->pGdata->transfData.nBmpHeight == 480 )
		{
			rect_Tmp.right	=	rect_Tmp.left + 61;
			rect_Tmp.bottom	=	rect_Tmp.top + 40;
		}
		else
		{
			rect_Tmp.right	=	rect_Tmp.left + 36;
			rect_Tmp.bottom	=	rect_Tmp.top + 22;
		}
	}
	else
	{
		if( g_pDMLibFS->pGdata->transfData.nBmpWidth == 800  ||
			g_pDMLibFS->pGdata->transfData.nBmpHeight == 480 )
		{
			rect_Tmp.right	=	rect_Tmp.left + 61;
			rect_Tmp.bottom	=	rect_Tmp.top + 35;
		}
		else
		{
			rect_Tmp.right	=	rect_Tmp.left + 36;
			rect_Tmp.bottom	=	rect_Tmp.top + 18;
		}
	}	
	//Set Font
	if( g_pDMLibFS->pGdata->transfData.nBmpWidth == 800 ||
		g_pDMLibFS->pGdata->transfData.nBmpHeight == 480 )
	{
		nuFont.lfHeight = 24;
	}
	else
	{
		nuFont.lfHeight = 14;
	}
	g_pDMLibGDI->GdiSetFont(&nuFont);

	g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);

	return nuTRUE;
}

nuBOOL CRoadName::DrawRoadIconTW(nuLONG sx, nuLONG sy, nuBYTE byIconType, nuWCHAR* wsRdNum)
{
	//Set Font Color
	g_pDMLibGDI->GdiSetTextColor(nuRGB(255, 255, 255)*(byIconType - 1));
	//
	NUROFONT nuFont = {0};
	nuroRECT rect_Tmp = {0};
	rect_Tmp.left = sx;
	rect_Tmp.top = sy;
	rect_Tmp.right = rect_Tmp.left + 48;
	rect_Tmp.bottom = rect_Tmp.top + 48;
	if( nuWcslen(wsRdNum) == 1 )
	{
		nuFont.lfHeight	= 36;
		if( byIconType > 0 )
		{
			nuFont.lfHeight	-= 8;
		}
		g_pDMLibGDI->GdiSetFont(&nuFont);							
		g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
	}
	else if( nuWcslen(wsRdNum) == 2 )
	{
		if( wsRdNum[1] < 58 ) //57对应9
		{
			nuFont.lfHeight	= 26;
			if( byIconType > 0 )
			{
				rect_Tmp.top -= 3;
			}
			g_pDMLibGDI->GdiSetFont(&nuFont);							
			g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
		}
		else
		{
			nuWCHAR wsRdNum2[10] = {0};
			wsRdNum2[0] = wsRdNum[1];
			wsRdNum[1] = 0;
			NUROFONT nuFont2	= {0};			
			nuFont2.lfHeight	= 15;
			g_pDMLibGDI->GdiSetFont(&nuFont2);
			g_pDMLibGDI->GdiExtTextOutWEx( rect_Tmp.left + 18, 
				rect_Tmp.top + 25, 
				wsRdNum2, nuWcslen(wsRdNum2), false, 0 );
			nuFont.lfHeight	= 25;
			g_pDMLibGDI->GdiSetFont(&nuFont);
			rect_Tmp.left   = rect_Tmp.left;
			rect_Tmp.right  = rect_Tmp.right;
			rect_Tmp.top    = rect_Tmp.top - 9;
			rect_Tmp.bottom = rect_Tmp.bottom - 7;
			g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
		}	
	}
	else if( nuWcslen(wsRdNum) == 3 )
	{
		if( wsRdNum[2] < 58 ) //57对应9
		{
			nuFont.lfHeight	= 20;
			g_pDMLibGDI->GdiSetFont(&nuFont);							
			g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
		}
		else
		{
			nuWCHAR wsRdNum2[10] = {0};
			wsRdNum2[0] = wsRdNum[2];
			wsRdNum[2] = 0;
			NUROFONT nuFont2    = {0};
			nuFont2.lfHeight	= 15;
			g_pDMLibGDI->GdiSetFont(&nuFont2);
			g_pDMLibGDI->GdiExtTextOutWEx( rect_Tmp.left + 18, 
				rect_Tmp.top + 25, 
				wsRdNum2, nuWcslen(wsRdNum2), nuFALSE, 0 );
			nuFont.lfHeight	= 22;
			g_pDMLibGDI->GdiSetFont(&nuFont);
			rect_Tmp.left   = rect_Tmp.left;
			rect_Tmp.right  = rect_Tmp.right;
			rect_Tmp.top    = rect_Tmp.top - 10;
			rect_Tmp.bottom = rect_Tmp.bottom - 10;
			g_pDMLibGDI->GdiDrawTextW(wsRdNum, nuWcslen(wsRdNum), &rect_Tmp, NURO_DT_SINGLELINE|NURO_DT_CENTER|NURO_DT_VCENTER);
		}
	}
	return nuTRUE;
}
nuBYTE CRoadName::DrawRoadNamefilter(nuWCHAR *wsRoadName, nuINT nWordCount)
{
	nuBYTE i = 1;
	if(wsRoadName[0] == 0x0028)
	{
		return nWordCount * 2;
	}
	while(i < nWordCount)
	{
		if((wsRoadName[i] >= 0x0021 && wsRoadName[i] < 0x0030) ||
			(wsRoadName[i] > 0x0039 && wsRoadName[i] < 0x00C0))
		{
			wsRoadName[i] = 0;
			break;
		}
		i++;
	}
	return i*2;
}