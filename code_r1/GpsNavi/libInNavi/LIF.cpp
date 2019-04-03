// LIF.cpp: implementation of the CLIF class.
//
//////////////////////////////////////////////////////////////////////

#include "LIF.h"
#include "NuroDefine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////// 

CLIF::CLIF()
{
	m_nMapID      = 0;
	m_nBlockIndex = 0;
	m_nRoadIndex  = 0 ;
}

CLIF::~CLIF()
{

}

nuBOOL CLIF::GetLIFInfo(nuULONG nMapID, nuULONG nBlockIndex, nuULONG nRoadIndex, nuLONG nTurnAngle,NOW_LAME_INFO *pLans, nuTCHAR *sFileName )
{
	if (   nMapID      == m_nMapID
		&& nBlockIndex == m_nBlockIndex
		&& nRoadIndex  == m_nRoadIndex )
	{
		//pLans->bReGetInfo = nuFALSE;
		return nuTRUE;
	}
	else
	{
		nuMemset( pLans, 0, sizeof(NOW_LAME_INFO) );
		pLans->bReGetInfo = nuTRUE;
		m_nMapID          = nMapID;
		m_nBlockIndex     = nBlockIndex;
		m_nRoadIndex      = nRoadIndex;
		nuMemset( m_sFileName, 0, sizeof(m_sFileName) );
		nuTcscpy( m_sFileName,sFileName );
		
	}
   //首先根据nMapID打开相应的文件
   nuFILE *pFile = NULL;
   if ( !OpenFile( (nuUSHORT)nMapID, &pFile))
   {
	   //nuFclose( pFile );
	   return nuFALSE;
   }

   //
   LIFHEADER LIF_Header;
   if ( !GetLIFHeader( pFile, &LIF_Header) )
   {
	   nuFclose( pFile );
	   return nuFALSE;	
   }
   if ( LIF_Header.nBlockCount <= nBlockIndex)
   {
	   nuFclose( pFile );
	   return nuFALSE;
   }
   //
   nuLONG nBlkAddres;
   if ( !GetBlockAddres( pFile, &nBlkAddres, nBlockIndex )  )
   {
	   nuFclose( pFile );
	   return nuFALSE;
   }
   if ( nBlkAddres == -1 )//如果某个Block没资料，值为-1
   {
	   nuFclose( pFile );
	   return nuFALSE;
   }
   //
   LIFBLOCKHEADER BlkHeader;
   if( !GetLIFBLOCKHEADER( pFile, nBlkAddres, &BlkHeader) )
   {
	   nuFclose( pFile );
	   return nuFALSE;
   }
   
   nuLONG nRoadAdd = 0;
   if (  !bFindRoad( BlkHeader,  nRoadIndex, nBlkAddres, &nRoadAdd, pFile ) )
   {
	   nuFclose( pFile );
	   return nuFALSE;
   }
  /* bFindRoad( Blkheader,  nRoadIndex, nBlkAddres, pFile )
   if ( nRoadIndex >= BlkHeader.nTotalRdSegs )
   {
	   nuFclose( pFile );
	   return nuFALSE;
   }
   
   //
   nuLONG nRoadAdd;
   if ( !GetRoadAddres( pFile, &nRoadAdd, nRoadIndex, nBlkAddres ) )
   {
	   nuFclose( pFile );
	   return nuFALSE;
   }
   */
   //
   LIFBLOCKRD_B BlkRD_B;
   if( !GetRD_BInfo( pFile, nRoadAdd, &BlkRD_B ))
   {
	   nuFclose( pFile );
	   return nuFALSE;
   }

   //
    if ( !GetArrowInfo(pFile, &BlkRD_B,nTurnAngle, pLans) )
    {
		nuFclose( pFile );
		return nuFALSE;
    }
	nuFclose( pFile );
   //
   
   return nuTRUE;

}

nuBOOL CLIF::OpenFile(nuUSHORT nMapID,nuFILE **ppFile)
{
	/*TCHAR filename[260] = {0};
	nuGetModuleFileName(NULL,filename,260);
	for(int i =nuTcslen(filename) - 1; i >= 0; i--)
	{
		if( filename[i] == nuTEXT('\\') )
		{
			filename[i+1] = nuTEXT('\0');
			break;
		}
	}
	nuTcscpy(filename, nuTEXT(".LIF"));
	if( !LibFS_FindFileWholePath(nMapID, filename, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	//nuTcscat( filename, nuTEXT("MAP\\BJ\\BJ.LIF") );*/
	*ppFile = nuTfopen( m_sFileName, NURO_FS_RB);
	if( *ppFile == NULL )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CLIF::GetLIFHeader(nuFILE *pFile, LIFHEADER *pLIFHeader)
{
	if ( pFile == NULL || pLIFHeader == NULL )
	{
		return nuFALSE;
	}
	if( 0 != nuFseek( pFile, 0, NURO_SEEK_SET ))
	{
		return nuFALSE;
	}
	if ( 1 != nuFread( pLIFHeader, sizeof(LIFHEADER), 1, pFile))
	{
		return nuFALSE;
	}
    return nuTRUE;
}

nuBOOL CLIF::GetBlockAddres(nuFILE *pFile, nuLONG *pBlkAddres, nuLONG nBlkIndex)
{
	if ( pFile == NULL || pBlkAddres == NULL )
	{
		return  nuFALSE;
	}
	if ( 0 != nuFseek( pFile, sizeof(LIFHEADER) + sizeof(nuLONG) * nBlkIndex, NURO_SEEK_SET ) )
	{
		return nuFALSE;
	}
	if ( nuFread( pBlkAddres, sizeof(nuDWORD), 1, pFile ) != 1 )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CLIF::GetLIFBLOCKHEADER( nuFILE *pFile, nuLONG nStartAdd, LIFBLOCKHEADER *pBlkHeader)
{
	if ( pFile == NULL || pBlkHeader == NULL )
	{
		return nuFALSE;
	}
	if(0 != nuFseek( pFile, nStartAdd, NURO_SEEK_SET ))
	{
		return nuFALSE;
	}
	if ( 1 != nuFread( pBlkHeader, sizeof(LIFBLOCKHEADER), 1, pFile))
	{
		return nuFALSE;
	}
    return nuTRUE;
}

nuBOOL CLIF::GetRoadAddres(nuFILE *pFile, nuLONG *pRoadAddres, nuLONG nRoadIndex, nuLONG nBlockBaseAdd )
{
	if ( pFile == NULL || pRoadAddres == NULL )
	{
		return  nuFALSE;
	}
	if (0 != nuFseek( pFile, nBlockBaseAdd + sizeof(LIFBLOCKHEADER) + sizeof(nuLONG) * nRoadIndex, NURO_SEEK_SET ) )
	{
		return nuFALSE;
	}
	if ( nuFread( pRoadAddres, sizeof(nuDWORD), 1, pFile ) != 1 )
	{
		return nuFALSE;
	}
	return nuTRUE;	
}

nuBOOL CLIF::GetRD_BInfo(nuFILE *pFile, nuLONG nAddB, LIFBLOCKRD_B *pBkRD_B)
{
	if ( pFile == NULL || pBkRD_B == NULL )
	{
		return nuFALSE;
	}
	if(0 != nuFseek( pFile, nAddB, NURO_SEEK_SET ) )
	{
		return nuFALSE;
	}
	if (  nuFread( pBkRD_B, sizeof(LIFBLOCKRD_B), 1, pFile ) != 1  )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CLIF::GetArrowInfo(nuFILE *pFile, LIFBLOCKRD_B *pBlockRD_b,nuLONG nTurnAngle, NOW_LAME_INFO *pLans)
{
	if ( pFile == NULL || pBlockRD_b == NULL || pLans == NULL )
	{
		return nuFALSE;
	}

	LIFBLOCKC1 BlkC1;
	if(0 != nuFseek( pFile, pBlockRD_b->B_address, NURO_SEEK_SET ))
	{
		return nuFALSE;
	}
	nuBYTE nArrowIndex = 0;

	for ( nArrowIndex = 0; nArrowIndex < pBlockRD_b->nARROWS ; nArrowIndex++ )
	{
		if ( pBlockRD_b->nCTYPE == 1 )
		{
			if (0 !=  nuFseek( pFile, sizeof( nuDWORD), NURO_SEEK_CUR ) )
			{
				return nuFALSE;
			}
		}
		if ( nuFread( &BlkC1, sizeof(LIFBLOCKC1), 1, pFile ) != 1)
		{
			return nuFALSE;
		}
        if ( !AddArrowInfo( &BlkC1, nTurnAngle,pLans) )
        {
			return nuFALSE;
        }					
	}
	return nuTRUE;
	
}

nuBOOL CLIF::AddArrowInfo( LIFBLOCKC1 *pBlkC1,nuLONG nTurnAngle, NOW_LAME_INFO *pLans)
{
	if ( pBlkC1 == NULL || pLans == NULL )
	{
		return nuFALSE;
	}
	if ( pLans->nNowCount == 0 )
	{
		pLans->nNowCount = 1;
		pLans->pLaneList[0].nArrowCount = 1;
		pLans->pLaneList[0].nLaneCar    = pBlkC1->nCT;
		pLans->pLaneList[0].nLaneType   = pBlkC1->nLANE_TYPE;
		pLans->pLaneList[0].pASList[0]  = pBlkC1->nAS;
		pLans->pLaneList[0].nLanHighLight = bJudgeHighLight( pBlkC1->nAS, nTurnAngle );
		pLans->pLaneList[0].nLaneNo     = pBlkC1->nLANE_NO;
	}
	else
	{
		//首先判断车道编号是否重复，重复则直接继续添加到此车道内，
		//否则判断车道顺序，然后插入某个合适的位置
		nuBYTE nLansNum = 0;
		nuBOOL bHaveRecord = nuFALSE;
		int nLANE_NO_Index = 0;
		//车道号按从小到大的顺序排列
		nuBYTE nLow = 0,nHigh = pLans->nNowCount - 1;
		nuBYTE nMid = ( nLow + nHigh ) / 2;
		if ( pBlkC1->nLANE_NO < pLans->pLaneList[nLow].nLaneNo )
		{
			nLANE_NO_Index = -1;
		}
		else if ( pBlkC1->nLANE_NO > pLans->pLaneList[nHigh].nLaneNo )
		{
			nLANE_NO_Index = nHigh;
		}
		else 
		{
			for ( nLansNum = 0; nLansNum < pLans->nNowCount; nLansNum++ )
			{	
				if ( pLans->pLaneList[nMid].nLaneNo == pBlkC1->nLANE_NO )
				{
					nLANE_NO_Index = nMid;
					bHaveRecord    = nuTRUE;
					break;
				}
				else if ( pLans->pLaneList[nMid].nLaneNo > pBlkC1->nLANE_NO )
				{
					nHigh = nMid - 1;
					if ( nLow >= nHigh  )
					{
						nLANE_NO_Index = nLow;
						break;
					}
					else
					{
						nMid  = ( nLow + nHigh ) /2 ;
					}
					
				}
				else if ( pLans->pLaneList[nMid].nLaneNo < pBlkC1->nLANE_NO )
				{
					nLow = nMid + 1;
					if ( nLow >= nHigh )
					{
						nLANE_NO_Index = nLow - 1;
					}
					else
					{
						nMid = ( nLow + nHigh ) / 2;
					}
					
				}
			}

		}
		
		//车道号已经存在
		if ( bHaveRecord )
		{
			//防止数组溢出
			if ( pLans->pLaneList[nLANE_NO_Index].nArrowCount < MAX_ARROWS_OF_LANE )
			{
				pLans->pLaneList[nLANE_NO_Index].pASList[ pLans->pLaneList[nLANE_NO_Index].nArrowCount] = pBlkC1->nAS; 
				pLans->pLaneList[nLANE_NO_Index].nLanHighLight = bJudgeHighLight( pBlkC1->nAS, nTurnAngle );
				pLans->pLaneList[nLANE_NO_Index].nArrowCount++;
			}	
		}
		else
		{
			//防止数组溢出
			if ( pLans->nNowCount < MAX_LANE_COUNTS )
			{ 
				//先进行车道的资料插入
				int nNumMove = pLans->nNowCount - 1 - nLANE_NO_Index;
				int nStartMove = nLANE_NO_Index + 1;
				int nIndex = 0;
				for ( nIndex = 0; nIndex < nNumMove; nIndex++ )
				{
					nuMemcpy( &(pLans->pLaneList[nStartMove + nNumMove - nIndex]), &(pLans->pLaneList[nStartMove + nNumMove - nIndex - 1]), sizeof(SINGLE_LANE_INFO) );
				}
				pLans->pLaneList[nStartMove].nLaneNo      = pBlkC1->nLANE_NO;
				pLans->pLaneList[nStartMove].nLaneCar     = pBlkC1->nCT;
				pLans->pLaneList[nStartMove].nLaneType    = pBlkC1->nLANE_TYPE;
				pLans->pLaneList[nStartMove].pASList[0]   = pBlkC1->nAS;
				pLans->pLaneList[nStartMove].nLanHighLight = bJudgeHighLight( pBlkC1->nAS, nTurnAngle );
				pLans->pLaneList[nStartMove].nArrowCount  = 1;
				pLans->nNowCount++;
			}
			
		}

	}
	return nuTRUE;
}

nuBOOL CLIF::bFindRoad( LIFBLOCKHEADER Blkheader, nuULONG nRoadIndex,nuLONG  nBlkAddres, nuLONG *pRoadAddr, nuFILE *pFile )
{
	if (   pFile == NULL 
		|| nBlkAddres <= 0 
		|| nRoadIndex <= 0 )
	{
		return nuFALSE;
	}
	nuLONG nIndex = 0;
	nuBOOL bFind = nuFALSE;
	nuLONG nIndexSeek = 0;
	for ( nIndex = 0; nIndex < Blkheader.nTotalRdSegs; nIndex++ )
	{
		nuLONG nTmpIndex;
		if( 0 != nuFseek( pFile,nBlkAddres + sizeof(Blkheader) + nIndex * sizeof(nuLONG), NURO_SEEK_SET ) )
		{
			break;
		}
		if ( 1 == nuFread( &nTmpIndex, sizeof(nuLONG), 1, pFile ) )
		{
			if ( nTmpIndex == nRoadIndex )
			{
				nIndexSeek = nIndex;
				bFind = nuTRUE;
				break;
			}
		}
		else
		{
			break;
		}
	}

	//Test
	/*
	nIndexSeek = Blkheader.nTotalRdSegs - 1;
	bFind = nuTRUE;*/

	if ( bFind )
	{
		*pRoadAddr = nBlkAddres + sizeof(Blkheader) + sizeof(nuLONG) * Blkheader.nTotalRdSegs + nIndexSeek * sizeof(LIFBLOCKRD_B);
	}
	return bFind;
}

//这里传进来的角度为逆时针方向的角度
nuBOOL CLIF::bJudgeHighLight(nuBYTE nArrow, nuLONG nAngle)
{
	//首先转化为顺时针
	nAngle = 360 - ( 360 + nAngle ) % 360;
	nuLONG nIndex = (nuLONG (( nAngle + 22.5 ) / 45) ) % 8;
	if ( nIndex == nArrow )
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
}
