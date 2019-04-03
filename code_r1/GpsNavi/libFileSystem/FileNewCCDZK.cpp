// FileNewCCDZK.cpp: implementation of the CFileNewCCDZK class.
//
//////////////////////////////////////////////////////////////////////

#include "FileNewCCDZK.h"
#include "nuTTSdefine.h"
#include "NuroConstDefined.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _CCD2_TYPE_DISTANCE		        0xff

#define _EEYE_FILE_SPEED_LIMIT			0x00
#define _EEYE_FILE_SPEED_UNIT			0x01
#define _EEYE_FILE_HEAD_COUNT			2

CFileNewCCDZK::CFileNewCCDZK()
{

}

CFileNewCCDZK::~CFileNewCCDZK()
{
    FreeCcd2Data(0);
}

nuBOOL CFileNewCCDZK::Initialize()
{
	nuMemset(m_pCcd2Checking, 0, sizeof(m_pCcd2Checking));
	m_nCheckingCount	 = 0;
	m_pFile				= NULL;
	m_nMapIdx			= -1;

	nuMemset(m_pCcd2Played, 0, sizeof(m_pCcd2Played));
	m_wsSpeedLimit[0] = 0;
	m_nSpeedLmtCount	= (nuBYTE)GetCcd2TypeName(_EEYE_FILE_SPEED_LIMIT, (nuBYTE*)m_wsSpeedLimit, sizeof(m_wsSpeedLimit));
	m_nSpeedUnitCount	= (nuBYTE)GetCcd2TypeName(_EEYE_FILE_SPEED_UNIT, (nuBYTE*)m_wsSpeedUnit, sizeof(m_wsSpeedUnit));
	return nuTRUE;
}

nuVOID CFileNewCCDZK::Free()
{
	
}

PCCD2_DATA	CFileNewCCDZK::GetCcd2CheckData(nuWORD nMapIdx, const nuroRECT& rtRect, nuINT* pBlkCount)
{
	nuDWORD nXStart = 0, nXEnd = 0, nYStart = 0, nYEnd = 0;
	nXStart	= (rtRect.left + HALF_MAX_LONGITUDE) / _CCD2_BLOCK_SIZE;
	nXEnd	= (rtRect.right + HALF_MAX_LONGITUDE) / _CCD2_BLOCK_SIZE;
	nYStart	= (rtRect.top + HALF_MAX_LATITUDE) / _CCD2_BLOCK_SIZE;
	nYEnd	= (rtRect.bottom + HALF_MAX_LATITUDE) / _CCD2_BLOCK_SIZE;
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, nuTEXT(".ccd"));
	if( !m_pFsApi->FS_FindFileWholePath(nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return NULL;
	}
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( NULL == pFile )
	{
		return NULL;
	}
	CCD2_FILE_HEAD ccd2FileHead = {0};
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&ccd2FileHead, sizeof(CCD2_FILE_HEAD), 1, pFile) != 1 )
	{
		nuFclose(pFile);
		pFile=NULL;
		return NULL;
	}
	//...
	/*
	ccd2FileHead.nXStart	+= 900;
	ccd2FileHead.nXEnd		+= 900;
	ccd2FileHead.nYStart	+= 450;
	ccd2FileHead.nYEnd		+= 450;
	*/
	if( nXStart < ccd2FileHead.nXStart )
	{
		nXStart	= ccd2FileHead.nXStart;
	}
	if( nXEnd > ccd2FileHead.nXEnd )
	{
		nXEnd	= ccd2FileHead.nXEnd;
	}
	if( nYStart < ccd2FileHead.nYStart )
	{
		nYStart	= ccd2FileHead.nYStart;
	}
	if( nYEnd > ccd2FileHead.nYEnd )
	{
		nYEnd	= ccd2FileHead.nYEnd;
	}
	nuDWORD i = 0, j = 0, nBlockID = 0;
	nuDWORD pBlockIDs[_MAX_CCD_BLOCKS_FOR_CHECKING] = {0};
	nuDWORD pBlockAddrs[_MAX_CCD_BLOCKS_FOR_CHECKING] = {0};
	nuUINT nNeedLoaden = 0;
	for(i = nYStart; i <= nYEnd; ++i)
	{
		for(j = nXStart; j <= nXEnd; ++j)
		{
			nBlockID	= _CCD2_BLOCK_X_COUNT * i + j;
			nuBOOL bFind = nuFALSE;
			for(nuBYTE k = 0; k < m_nCheckingCount; ++k)
			{
				if( m_pCcd2Checking[k].nBlockID == nBlockID )
				{
					bFind = nuTRUE;
					m_pCcd2Checking[k].bUsedNow	= 1;
					break;
				}
			}
			if( !bFind && nNeedLoaden < _MAX_CCD_BLOCKS_FOR_CHECKING )
			{
				pBlockIDs[nNeedLoaden] = nBlockID;
				nuLONG nAddIdx = (i - ccd2FileHead.nYStart) * (ccd2FileHead.nXEnd - ccd2FileHead.nXStart + 1) + (j - ccd2FileHead.nXStart);
				if( nuFseek(pFile, sizeof(CCD2_FILE_HEAD) + nAddIdx * 4, NURO_SEEK_SET) != 0 ||
					nuFread(&pBlockAddrs[nNeedLoaden], 4, 1, pFile) != 1 )
				{
					nuFclose(pFile);
					pFile=NULL;
					return NULL;
				}//Read BlockAddress
				++nNeedLoaden;
			}
		}
	}
	//remove blocks unused
	j = -1;
	nuINT nCount = 0;
	for( i = 0; i < m_nCheckingCount; ++i )
	{
		if( m_pCcd2Checking[i].bUsedNow )
		{
			if( -1 != j )
			{
				m_pCcd2Checking[j] = m_pCcd2Checking[i];
			}
			m_pCcd2Checking[i].bUsedNow = 0;
		}
		else
		{
            m_pMmApi->MM_RelDataMemMass( &m_pCcd2Checking[i].nMemIdx );
			if( -1 == j )
			{
				j = i;
			}
			++nCount;
		}
	}
	m_nCheckingCount -= nCount;
	//Read blocks
	for(i = 0; i < nNeedLoaden; ++i)
	{
		if( _MAX_CCD_BLOCKS_FOR_CHECKING == m_nCheckingCount )
		{
			break;
		}
		if( nuFseek(pFile, pBlockAddrs[i], NURO_SEEK_SET) != 0 ||
			nuFread(&nCount, 4, 1, pFile) != 1 )
		{
			break;
		}
		nuLONG nSize = nCount * sizeof(CCD2_BODY_A1) + 4;
		nuBYTE* pData = (nuBYTE*)m_pMmApi->MM_GetDataMemMass(nSize, &m_pCcd2Checking[m_nCheckingCount].nMemIdx);
		if( NULL == pData )
		{
			break;
		}
		nuMemcpy(pData, &nCount, 4);
		pData += 4;
		nSize -= 4;
		if( nuFread(pData, nSize, 1, pFile) != 1 )
		{
			m_pMmApi->MM_RelDataMemMass(&m_pCcd2Checking[m_nCheckingCount].nMemIdx);
			break;
		}
		m_pCcd2Checking[m_nCheckingCount].nBlockID	= pBlockIDs[i];
		m_pCcd2Checking[m_nCheckingCount].nMapIdx	= nMapIdx;
        m_pCcd2Checking[m_nCheckingCount].bUsedNow  = 0;
		++m_nCheckingCount;
	}
	//...
	nuFclose(pFile);
	pFile=NULL;
	if( NULL != pBlkCount )
	{
		*pBlkCount = m_nCheckingCount;
	}
	return m_pCcd2Checking;
}

nuUINT CFileNewCCDZK::FreeCcd2Data(nuINT nMode)
{
	for(nuUINT i = 0; i < m_nCheckingCount; ++i)
	{
		m_pMmApi->MM_RelDataMemMass( &m_pCcd2Checking[i].nMemIdx );
	}
	m_nCheckingCount = 0;
	return 1;
}

nuUINT CFileNewCCDZK::CheckCcd2(PCCDINFO pCcdInfo)
{
	if( NULL == pCcdInfo )
	{
		return 0;
	}
	nuroRECT	rtRange = {0};
	nuroPOINT	point = {0};
	rtRange.left	= pCcdInfo->ptCarMapped.x - _CCD_CHECK_MAX_DIS;
	rtRange.right	= pCcdInfo->ptCarMapped.x + _CCD_CHECK_MAX_DIS;
	rtRange.top		= pCcdInfo->ptCarMapped.y - _CCD_CHECK_MAX_DIS;
	rtRange.bottom	= pCcdInfo->ptCarMapped.y + _CCD_CHECK_MAX_DIS;
	nuroRECT rtSmall ={0};
	#define  _CCD_CHECK_MIN_DIS  50 
	rtSmall.left	= pCcdInfo->ptCarMapped.x - _CCD_CHECK_MIN_DIS;
	rtSmall.right	= pCcdInfo->ptCarMapped.x + _CCD_CHECK_MIN_DIS;
	rtSmall.top		= pCcdInfo->ptCarMapped.y - _CCD_CHECK_MIN_DIS;
	rtSmall.bottom	= pCcdInfo->ptCarMapped.y + _CCD_CHECK_MIN_DIS;

	RemovePlayed(&rtSmall);
	nuWORD nMapIdx = pCcdInfo->nCarMapIdx;
	if( 0 != pCcdInfo->nCcdRdCount )
	{
		nMapIdx = pCcdInfo->pCcdRoad[ pCcdInfo->nCcdRdCount - 1 ].nCcdMapIdx;
	}
	if( GetCcd2CheckData(nMapIdx, rtRange, NULL) == NULL )
	{
		return 0;
	}
	nuDWORD i = 0;
	nuBYTE* pData = NULL;
	PCCD2_BODY_A1 pCcd2A1 = NULL;
	nuDWORD	nCount;
	nuroRECT rtBlock;
	nuBOOL bCCDPointInRect = nuFALSE;
	for(i = 0; i < m_nCheckingCount; ++i)
	{
		pData = (nuBYTE*)m_pMmApi->MM_GetDataMassAddr(m_pCcd2Checking[i].nMemIdx);
		if( NULL == pData )
		{
			continue;
		}
		nuBlockIDtoRectEx(m_pCcd2Checking[i].nBlockID, rtBlock, _CCD2_BLOCK_SIZE);
		nuMemcpy(&nCount, pData, 4);
		pCcd2A1 = (PCCD2_BODY_A1)(pData + 4);
		for(nuDWORD j = 0; j < nCount; ++j)
		{
			nuINT k = -1;
			CCDROAD ccdRoad;
			nuMemset(&ccdRoad, NULL, sizeof(CCDROAD));
			if(!m_pFsApi->pGdata->uiSetData.bNavigation)
			{
				ccdRoad.nCcdBlkIdx	= pCcdInfo->nCarBlkIdx;
				ccdRoad.nCcdMapIdx	= pCcdInfo->nCarMapIdx;
				ccdRoad.nCcdRdIdx	= pCcdInfo->nCarRdIdx;
				ccdRoad.nOneWay		= pCcdInfo->nOneWay;
			}
			else
			{
				k = 0;
			}
			while( k < pCcdInfo->nCcdRdCount )
			{
				if( k >= 0 )
				{
					ccdRoad = pCcdInfo->pCcdRoad[k];
				}
				if( nMapIdx == ccdRoad.nCcdMapIdx	&&
					pCcd2A1[j].nBlkIdx	== ccdRoad.nCcdBlkIdx &&
					pCcd2A1[j].nRdIdx	== ccdRoad.nCcdRdIdx  )
				{
					//angle judge
					point.x = rtBlock.left + pCcd2A1[j].nX;
					point.y = rtBlock.top + pCcd2A1[j].nY;
					
					if( nuPointInRect(&point, &rtRange) )
					{
						bCCDPointInRect = nuTRUE;
						nuINT l = -1;
						nuBOOL bNewCcd = nuTRUE;
						for(nuWORD m = 0; m <_CCD2_MAX_CHECK_COUNT; ++m)
						{
							nuBOOL bPointInArray = nuFALSE;
								if( m_pCcd2Played[m].point.x == point.x &&
									m_pCcd2Played[m].point.y == point.y )
								{
								bPointInArray = nuTRUE;
							}
							if( bPointInArray )
							{
								if( 1 == m_pCcd2Played[m].bUsed || 2 == m_pCcd2Played[m].bUsed )
								{
									bNewCcd = nuFALSE;
									break;
								}
							
							}
							else if( 0 == m_pCcd2Played[m].bUsed )
							{
								if((pCcd2A1[j].nCheckDir != ccdRoad.nOneWay))
								{
									continue;
								}
								l = m;
							}
						}
						if( bNewCcd && l >= 0  && m_pFsApi->pUserCfg->bCCDVoice)
						{
							++m_pFsApi->pGdata->drawInfoMap.nCCDNum;
							m_pCcd2Played[l].bUsed = 1;
							m_pCcd2Played[l].point = point;
							// Modified by Damon For CCD Must popMsg
							//if( 1 == m_pFsApi->pUserCfg->bCCDVoice )
							//{
							#ifdef VALUE_EMGRT
									nuWCHAR wsTTsTxt[256] = {0};
									nuUINT nNum = GetEEyeTypeName(pCcd2A1[j].nType, wsTTsTxt, sizeof(wsTTsTxt));
									if( 0 == nNum)
									{
										nuWCHAR wsConst[25] = {21069 ,26041, 26377, 27979, 36895, 29031, 30456 };
										nuWcscat( wsTTsTxt, wsConst );
										nNum = nuWcslen(wsTTsTxt);
									}
									if( pCcd2A1[j].nSpeed > 0 )
									{
										AddLimitSpeed(pCcd2A1[j].nSpeed*10, pCcd2A1[j].nType,  &wsTTsTxt[nNum], 256 - nNum);
									}
									if( pCcd2A1[j].nSpeed > 0 )
									{
									nuWCHAR wsKM[25] ={20844 ,37324 };
									nuWcscat( wsTTsTxt, wsKM );
									}
									if(pCcd2A1[j].nSpeed > 0 && m_pFsApi->pUserCfg->nPlaySpeedWarning && m_pFsApi->pGdata->carInfo.nCarSpeed > (pCcd2A1[j].nSpeed * 10))
									{
										nuWCHAR wsOverSpeed[25] ={24744,24050,36229,36895};
										nuWcscat( wsTTsTxt,wsOverSpeed);
									}
									m_pFsApi->pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_EEYESFIND, nNum, wsTTsTxt);	
							#else//VALUE_EMGRT		
							  #ifndef NURO_USE_CYBERON_TTS
								nuWCHAR wsName[256] = {0};
//								nuUINT nWsCount = ColCcd2Name(pCcd2A1[j].nType, pCcd2A1[j].nSpeed*10, wsName, 256);
								nuUINT nWsCount = GetCcd2TypeName(pCcd2A1[j].nType, (nuBYTE*)wsName, 256);
					
								// Damon 20110715 先還原 不然不會送CallBack至UI 
								m_pFsApi->pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_CCDFIND, 256, wsName);
							  #else//NURO_USE_CYBERON_TTS
								nuWCHAR wsName[256] = {0}, wsSpeedName[20] = {0}, wsSpeedlimit[3] = {0x9650, 0x901F}, wsKm[3] = {0x516C, 0x91CC};
								//nuWCHAR wsSpeed[5] = {0x4E00,0x767E,0x4E00,0x5341,0};
//								nuUINT nWsCount = ColCcd2Name(pCcd2A1[j].nType, pCcd2A1[j].nSpeed*10, wsName, 256);
								nuUINT nWsCount = GetCcd2TypeName(pCcd2A1[j].nType, (nuBYTE*)wsName, 256);
								if(5 != pCcd2A1[j].nType)
								{
									nuWcscat(wsName, wsSpeedlimit);
									nuItow(pCcd2A1[j].nSpeed*10, wsSpeedName,10);
									nuWcscat(wsName, wsSpeedName);
									nuWcscat(wsName, wsKm);									
								}								
								m_pFsApi->pGdata->NC_SoundPlay(VOICE_CODE_OTHERS_ASY, 0, 0, O_CCDFIND, 256, wsName);
								k = pCcdInfo->nCcdRdCount;
								j = nCount;
							  #endif//NURO_USE_CYBERON_TTS
							#endif//VALUE_EMGRT
							//}
							//else
							//{
								//m_pFsApi->pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_ASY, 3, 0, 0, 0, NULL);
							//}
						}
						break;
					}
				}
				++k;
			}
		}
	}
	if ( nuFALSE == bCCDPointInRect )
	{
		m_pFsApi->pGdata->drawInfoMap.nCCDNum = 0;
	}
	return 1;
}

nuVOID CFileNewCCDZK::RemovePlayed(nuroRECT* pRect)
{
	nuroRECT TmpRt = {0};
	nuroPOINT	point = {0};
	point.x = ( pRect->left + pRect->right ) / 2;
	point.y = ( pRect->top + pRect->bottom ) /2;
	TmpRt.left   = point.x - 500;
	TmpRt.top    = point.y  - 500;
	TmpRt.right  = point.x + 500;
	TmpRt.bottom = point.y + 500;
	
	for(nuINT i = 0; i < _CCD2_MAX_CHECK_COUNT; ++i)
	{
		if( m_pCcd2Played[i].bUsed == 1)
		{
			if( nuPointInRect(&m_pCcd2Played[i].point, pRect) )
			{
				if(m_pFsApi->pGdata->drawInfoMap.nCCDNum > 0)
				{
					--m_pFsApi->pGdata->drawInfoMap.nCCDNum;
				}
				m_pCcd2Played[i].bUsed = 2;
				//m_pFsApi->pGdata->NC_SoundPlay(VOICE_CODE_DEFAULT_ASY, 6, 0, 0, 0, NULL);
				if( NULL != m_pFsApi->pGdata->pfCallBack )
				{
					CALL_BACK_PARAM cbParam;
					cbParam.nCallType = CALLBACK_TYPE_CCDEND;
					m_pFsApi->pGdata->pfCallBack(&cbParam);
				}
			}
			else if( !nuPointInRect(&m_pCcd2Played[i].point, &TmpRt) )
			{
				nuMemset( &m_pCcd2Played[i], 0, sizeof(m_pCcd2Played[i]) );
			}
		}
		if( 2 == m_pCcd2Played[i].bUsed )
		{
			 if( !nuPointInRect(&m_pCcd2Played[i].point, &TmpRt) )
			{
				nuMemset( &m_pCcd2Played[i], 0, sizeof(m_pCcd2Played[i]) );
			}
		}
		
	
	}
}

nuUINT CFileNewCCDZK::GetCcd2TypeName(nuBYTE nType, nuBYTE* pBuff, nuWORD nBuffLen)
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	if( !m_pFsApi->FS_GetFilePath(FILE_NAME_EEYETYPE_TEXT, tsFile, sizeof(tsFile)) )
	{
		return 0;
	}
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( !pFile )
	{
		return 0;
	}
	nuUINT nNum = 0;
	if( nuFseek(pFile, 0, NURO_SEEK_SET) == 0 )
	{
		nNum = nuFreadLineIndexW(nType, (nuWCHAR*)pBuff, nBuffLen/sizeof(nuWCHAR), pFile);
	}
	nuFclose(pFile);
	pFile=NULL;
	return nNum;
}

nuUINT CFileNewCCDZK::ColCcd2Name(nuBYTE nType, nuBYTE nSpeed, nuWCHAR* pwsName, nuWORD nCount)
{
	if( NULL == pwsName )
	{
		return 0;
	}
	nuUINT nRes = GetCcd2TypeName(nType + _EEYE_FILE_HEAD_COUNT, (nuBYTE*)pwsName, nCount*sizeof(nuWCHAR));
	if( nRes && nSpeed && _CCD2_TYPE_DISTANCE != nType )
	{
		nuWcsncat(pwsName, m_wsSpeedLimit, nCount - nRes);
		nRes += m_nSpeedLmtCount;
		nuWCHAR wsNum[10] = {0};
		nuItow(nSpeed, wsNum, 10);
		nuWcsncat(pwsName, wsNum, nCount - nRes);
		nRes += nuWcslen(wsNum);
		nuWcsncat(pwsName, m_wsSpeedUnit, nCount - nRes);
		nRes += m_nSpeedUnitCount;
	}
	return nRes;
}

nuUINT CFileNewCCDZK::GetEEyeTypeName(nuBYTE nType, nuPVOID pBuff, nuUINT nBuffLen)
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	if( !m_pFsApi->FS_GetFilePath(FILE_NAME_EEYETYPE_TEXT, tsFile, sizeof(tsFile)) )
	{
		return 0;
	}
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( !pFile )
	{
		return 0;
	}
	nuUINT nNum = 0;
	if( nuFseek(pFile, 0, NURO_SEEK_SET) == 0 )
	{
		nNum = nuFreadLineIndexW(nType, (nuWCHAR*)pBuff, nBuffLen/sizeof(nuWCHAR), pFile);
	}
	if( 0 == nNum )
	{
		nNum = nuFreadLineIndexW(0, (nuWCHAR*)pBuff, nBuffLen/sizeof(nuWCHAR), pFile);
	}
	nuFclose(pFile);
	return nNum;
}

nuVOID CFileNewCCDZK::AddLimitSpeed(nuINT nSpeed, nuWORD nType, nuWCHAR *wsBuff, nuINT nWsNum)
{
	nuINT nWsLimitNum = 0;
	nuWCHAR wsLimit[25] = {0};
	if( nWsLimitNum == 0 )
	{
		nWsLimitNum = (nuBYTE)CFileSystemBaseZ::GetConstStr(CONST_STR_LIMIT, wsLimit, 5);
	}
	/* @Zhikun 2009.08.25
	if( 0 == m_nWsDisLimitNum )
	{
		m_nWsDisLimitNum = (nuBYTE)GetConstStr(CONST_STR_DIS_LIMIT, m_wsDisLimit, 6);
	}
	*/
	nuWCHAR wsNum[10] = {0};
	if( EMG_ET_TYPE_CM != nType )
	{	
		nuWcsncat(wsBuff, wsLimit, nuWcslen(wsLimit));
		if( nSpeed == 110 || nSpeed == 120 )
		{
			wsNum[0] = 19968 ;
			wsNum[1] = 30334;
			if ( nSpeed == 110 )
			{
				wsNum[2] = 19968;
			}
			else if( nSpeed == 120 )
			{
				wsNum[2] = 20108;
			}
			wsNum[3] = 21313;
		}
		else
		{
			nuItow(nSpeed, wsNum, 10);
		}
		nuWcsncat(wsBuff, wsNum, nWsNum);
	}
}
