// BlockDw.cpp: implementation of the CBlockDw class.
//
//////////////////////////////////////////////////////////////////////

#include "BlockDw.h"
#include "NuroModuleApiStruct.h"

//Library
#include "NuroCommonStructs.h"
#include "NuroClasses.h"
#include "libFileSystem.h"
#include "MapFileFhZ.h"

#ifdef NURO_USE_LOGFILE
#include "LogFileCtrlZ.h"
#endif

extern PMEMMGRAPI	g_pFsMmApi;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockDw::CBlockDw()
{
	m_nDwCount		= 0;
	m_pDwDataList	= NULL;
	m_screenMap.pDwList	= NULL;
	m_screenMap.nDwCount= 0;
	nuMemset(&m_blockIDArrayOld, 0, sizeof(BLOCKIDARRAY));
	/*test
		m_nRdBlocks = 0;
		m_nPdBlocks = 0;
		m_nBlBlocks = 0;
		m_nBaBlocks = 0;
	test*/
}

CBlockDw::~CBlockDw()
{
	Free();
}

nuBOOL CBlockDw::Initialize(class CMapFileFhZ* pFileFh)
{
	/* 2009.06.16
	if( !m_fileFB.Initialize() )
	{
		return nuFALSE;
	}
	*/
	m_nDwCount = LibFS_GetMapCount();
	if( m_nDwCount == 0 )
	{
		return nuFALSE;
	}
	m_pDwDataList = (PDWDATA)g_pFsMmApi->MM_GetStaticMemMass(m_nDwCount*sizeof(DWDATA));
	if( m_pDwDataList == NULL )
	{
		return nuFALSE;
	}
	for(nuWORD i = 0; i < m_nDwCount; i++)
	{
		m_pDwDataList[i].bCover	= 0;
		m_pDwDataList[i].nNumOfBlocksCovered	= 0;
		m_pDwDataList[i].pBlockList	= NULL;
	}
	m_pFileFh = pFileFh;
	if(!nuReadConfigValue("USETMC",&m_lUseTMC))
	{
		m_lUseTMC = 0;
	}
	if(m_lUseTMC)
	{
		LibFS_Initial_TMC_Data(m_nDwCount);
	}
	return nuTRUE;
}

nuVOID CBlockDw::Free()
{
	//Release and delete Blocks
	RelScreenMap();
	for(nuWORD i = 0; i < m_nDwCount; i++)
	{
		ReleaseAllBlocks(i);
	}
	m_nDwCount		= 0;
	m_pDwDataList	= NULL;
//2009.06.16	m_fileFB.Free();
	if(m_lUseTMC)
	{
		LibFS_Release_TMC_Data();
	}	
}

nuBOOL CBlockDw::LoadBlocks(const BLOCKIDARRAY& nBIDLMap, const BLOCKIDARRAY& nBIDLZoom)
{
	nuDWORD blockID = 0;
	nuBYTE nZoomBlockExtend = 0;
	nuBYTE nBlockExtend		= 0;
	
	if( m_blockIDArrayOld.nXend		== nBIDLMap.nXend	&&
		m_blockIDArrayOld.nXstart	== nBIDLMap.nXstart	&&
		m_blockIDArrayOld.nYend		== nBIDLMap.nYend	&&
		m_blockIDArrayOld.nYstart	== nBIDLMap.nYstart )
	{
		return nuTRUE;
	}
	m_blockIDArrayOld = nBIDLMap;
#ifdef _USE_LOG_FILE_FILESYSTEM
	/*
	CLogFileCtrlZ logFile;
	logFile.OpenLogFile(nuTEXT("FileSys.log"));
	logFile.OutString(nuTEXT("FB judge:"));
	logFile.SaveTicks();
	*/
#endif
	for(nuLONG i = nBIDLMap.nYstart; i <= nBIDLMap.nYend; i++ )
	{
		for(nuLONG j = nBIDLMap.nXstart; j <= nBIDLMap.nXend; j++ )
		{
			/* 2009.06.12
			blockID = i*XBLOCKNUM + j;
			for ( nuINT k = 0; k < m_nDwCount; k++)
			{
				if( m_fileFB.BlockIDInMap(blockID, k) )
				{
					BLK_OUT_DATA blkOut;
					m_pFileFh->BlockIDToMap(nuLOWORD(j), nuLOWORD(i), &blkOut);
					SetBlockID(k, blockID, blkOut.pBlkFixData[0].nBlkIdx);
				}
			}
			*/
			BLK_OUT_DATA blkOut = {0};
			if( m_pFileFh->BlockIDToMap(nuLOWORD(j), nuLOWORD(i), &blkOut) )
			{
				blockID = i*XBLOCKNUM + j;
				for(nuWORD l = 0; l < blkOut.nMapFixed; ++l)
				{
					SetBlockID(	blkOut.pBlkFixData[l].nMapIdx, 
								blockID,
								blkOut.pBlkFixData[l].nBlkIdx );
				}
			}
			//2009.06.12
		}
	}
#ifdef _USE_LOG_FILE_FILESYSTEM
	/*
	logFile.OutTicksPast();
	logFile.OutString(nuTEXT("CollectBlocks:"));
	logFile.SaveTicks();
	*/
#endif
//	DEBUGSTRING(nuTEXT("LoadBlock 2"));
	CollectBlocks();
#ifdef _USE_LOG_FILE_FILESYSTEM
	/*
	logFile.OutTicksPast();
	logFile.OutString(nuTEXT("Read blocks:"));
	logFile.SaveTicks();
	*/
#endif
//	DEBUGSTRING(nuTEXT("LoadBlock 3"));
	nuBOOL bres = ReadBlocks();
#ifdef _USE_LOG_FILE_FILESYSTEM
	/*
	logFile.OutTicksPast();
	logFile.CloseLogFile();
	*/
#endif
	return bres;
}

nuBOOL CBlockDw::SetBlockID(nuWORD nDwIdx, nuDWORD nBlockID, nuDWORD nBlockIdx/* = 0*/, nuBYTE nBlockExtend/* = 0*/, 
						  nuBYTE nBlockZoom/* = 0*/, nuBYTE nBlockZoomExtended/* = 0*/)
{
	if( nDwIdx >= m_nDwCount )
	{
		return nuFALSE;
	}
	m_pDwDataList[nDwIdx].bCover	= 1;
	PPBLOCKDATA ppBlockLastNode = &m_pDwDataList[nDwIdx].pBlockList;
	while( *ppBlockLastNode != NULL )
	{
		if( (*ppBlockLastNode)->nBlockID == nBlockID )
		{
			(*ppBlockLastNode)->bCover	= 1;
			return nuTRUE;
		}
		ppBlockLastNode = &((*ppBlockLastNode)->pNext);
	}
	if( m_pDwDataList[nDwIdx].nNumOfBlocksCovered >= MAX_BLOCKS_OF_EACH_DW )
	{
		return nuFALSE;
	}
	PBLOCKDATA pTmpBlock = new BLOCKDATA;
//------ New operator --------
	if( pTmpBlock == NULL )
	{
		return nuFALSE;
	}
	pTmpBlock->bCover		= 1;
	pTmpBlock->bBlockExtended	= nBlockExtend;
	pTmpBlock->bBlockZoom		= nBlockZoom;
	pTmpBlock->bBlockZoomExtend	= nBlockZoomExtended;
	pTmpBlock->pNext		= NULL;
	pTmpBlock->nBlockID		= nBlockID;
	pTmpBlock->nBlockIdx	= nBlockIdx;
	pTmpBlock->nBdwAreaMassIdx	= -1;
	pTmpBlock->nBdwLineMassIdx	= -1;
	pTmpBlock->nPdwMassIdx	= -1;
	pTmpBlock->nRdwMassIdx	= -1;
	(*ppBlockLastNode) = pTmpBlock;
	m_pDwDataList[nDwIdx].nNumOfBlocksCovered ++;
	return nuTRUE;
}

nuVOID CBlockDw::CollectBlocks()
{
	//Delete Blocks unused, which have been loaden.
	for(nuWORD i = 0; i < m_nDwCount; i++)
	{
		if( m_pDwDataList[i].bCover )
		{
			ReleaseBlocksUnused(i);
			m_pDwDataList[i].bCover = 0;
		}
		else
		{
			ReleaseAllBlocks(i);
		}
	}
}

nuBOOL CBlockDw::ReadBlocks()
{
	//Loas all blocks used
//	DEBUGSTRING(nuTEXT("ReadBk in"));
	for(nuWORD i = 0; i < m_nDwCount; i++)
	{
		if( m_pDwDataList[i].pBlockList != NULL )//Blocks used
		{
//			_nuDGSI(nuTEXT("LoadBlock bk"), i);
			ReadDwBlocks(i);
		}
	}
//	DEBUGSTRING(nuTEXT("ReadBk out"));
	return nuTRUE;
}

nuBOOL CBlockDw::ReleaseAllBlocks(nuWORD nDwIdx)
{
	PBLOCKDATA pBlockData = NULL;
	while( m_pDwDataList[nDwIdx].pBlockList != NULL )
	{
		pBlockData = m_pDwDataList[nDwIdx].pBlockList;
		m_pDwDataList[nDwIdx].pBlockList = m_pDwDataList[nDwIdx].pBlockList->pNext;
		g_pFsMmApi->MM_RelDataMemMass(&pBlockData->nBdwAreaMassIdx);
		g_pFsMmApi->MM_RelDataMemMass(&pBlockData->nBdwLineMassIdx);
		g_pFsMmApi->MM_RelDataMemMass(&pBlockData->nPdwMassIdx);
		g_pFsMmApi->MM_RelDataMemMass(&pBlockData->nRdwMassIdx);
		delete pBlockData;
		pBlockData=NULL;
//---- Delete operator -------
	}
	m_pDwDataList[nDwIdx].nNumOfBlocksCovered	= 0;
	return nuTRUE;
}

nuBOOL CBlockDw::ReleaseBlocksUnused(nuWORD nDwIdx)
{
	PPBLOCKDATA ppBlockData = &m_pDwDataList[nDwIdx].pBlockList;
	while( (*ppBlockData) != NULL )
	{
		PBLOCKDATA pBlockTmp = *ppBlockData;
		if( !pBlockTmp->bCover )
		{
			*ppBlockData = pBlockTmp->pNext;//
			g_pFsMmApi->MM_RelDataMemMass(&pBlockTmp->nBdwAreaMassIdx);
			g_pFsMmApi->MM_RelDataMemMass(&pBlockTmp->nBdwLineMassIdx);
			g_pFsMmApi->MM_RelDataMemMass(&pBlockTmp->nPdwMassIdx);
			g_pFsMmApi->MM_RelDataMemMass(&pBlockTmp->nRdwMassIdx);
			delete pBlockTmp;
			pBlockTmp=NULL;
//---- Delete operator -------
			m_pDwDataList[nDwIdx].nNumOfBlocksCovered --;
		}
		else
		{
			pBlockTmp->bCover = 0;
			ppBlockData = &(*ppBlockData)->pNext;
		}
	}
	return nuTRUE;
}

nuBOOL CBlockDw::ReadDwBlocks(nuWORD nDwIdx)
{
	PBLOCKDATA	pBlockData = NULL;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuFILE* pFileBh = NULL;
	nuFILE* pFile = NULL;
	BHFILE bhFile = {0};
	nuPVOID pMem = NULL;
//	nuLONG *pAddr = NULL;
//	nuDWORD i;
	nuBOOL bFind = nuFALSE;
	nuTcscpy(tsFile, nuTEXT(".BH"));
	if( !LibFS_FindFileWholePath(nDwIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	pFileBh = nuTfopen(tsFile, NURO_FS_RB);
	if( pFileBh == NULL )
	{
		return nuFALSE;
	}
	if( nuFseek(pFileBh, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&(bhFile.bhStaticHead), sizeof(BHSTATICHEAD), 1, pFileBh) != 1 )
	{ 
		nuFclose(pFileBh);
		return nuFALSE;
	}//Read Bh header
	/* 2009.06.11
	bhFile.pBlockIDList = new nuDWORD[bhFile.bhStaticHead.nBlockCount];
	if( bhFile.pBlockIDList == NULL )
	{
		nuFclose(pFileBh);
		return nuFALSE;
	}
	if( nuFread(bhFile.pBlockIDList, 4, bhFile.bhStaticHead.nBlockCount, pFileBh) != bhFile.bhStaticHead.nBlockCount )
	{
		delete []bhFile.pBlockIDList;
		nuFclose(pFileBh);
		return nuFALSE;
	}
	*///2009.06.11

	nuLONG nSize = 0;
	nuLONG nAddress = 0, nBhDis = 0;
	//Read Road data...
	nuTcscpy(tsFile, nuTEXT(".RDW"));//Get Rdw file path
	if( LibFS_FindFileWholePath(nDwIdx, tsFile, NURO_MAX_PATH) )
	{
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if(	pFile != NULL )
		{
			pBlockData = m_pDwDataList[nDwIdx].pBlockList;
			while( pBlockData != NULL )
			{
				if( g_pFsMmApi->MM_GetDataMassAddr(pBlockData->nRdwMassIdx) == NULL )//This block Rdw hasn't been loaden.
				{
					bFind = nuFALSE;
					/* 2009.06.11
					for(i = 0; i < bhFile.bhStaticHead.nBlockCount; i++)
					{
						if( pBlockData->nBlockID == bhFile.pBlockIDList[i] )
						{
							if( pBlockData->nBlockIdx != i )
							{
								nuINT llls = 2;
							}
							pBlockData->nBlockIdx = i;
							nBhDis = sizeof(BHSTATICHEAD) + bhFile.bhStaticHead.nBlockCount * 12 + 
								i * 4;
							if( nuFseek(pFileBh, nBhDis, NURO_SEEK_SET) == 0 &&
								nuFread(&nAddress, 4, 1, pFileBh) == 1 && 
								nAddress >= 0 )
							{
								bFind = nuTRUE;
							}
							break;
						}
					}
					*/
					nBhDis = sizeof(BHSTATICHEAD) + bhFile.bhStaticHead.nBlockCount * 12 + 
						pBlockData->nBlockIdx * 4;
					if( nuFseek(pFileBh, nBhDis, NURO_SEEK_SET) == 0 &&
						nuFread(&nAddress, 4, 1, pFileBh) == 1 && 
						nAddress >= 0 )
					{
						bFind = nuTRUE;
					}
					//2009.06.11
					if( bFind )
					{
						DWROADHEAD dwRoadHead = {0};
						if( nuFseek(pFile, nAddress, NURO_SEEK_SET) == 0 &&
							nuFread(&dwRoadHead, sizeof(DWROADHEAD), 1, pFile) == 1 )
						{
							nSize = sizeof(DWROADHEAD) + dwRoadHead.nRoadCount * sizeof(DWROADDATA) + 
								dwRoadHead.nRoadCommonCount * sizeof(DWROADCOMMON) + 
								dwRoadHead.nVertexCount * sizeof(NUROSPOINT);//Col Rdw Block size;
							pMem = g_pFsMmApi->MM_GetDataMemMass(nSize+dwRoadHead.nRoadCount*sizeof(NUROSRECT), &pBlockData->nRdwMassIdx);
							
							if( pMem != NULL )
							{
								nuMemcpy(pMem, &dwRoadHead, sizeof(DWROADHEAD));
								pMem = (nuPVOID)(((nuLONG)pMem)+sizeof(DWROADHEAD));
								if( nuFread(pMem, nSize-sizeof(DWROADHEAD), 1, pFile) != 1 )
								{
									g_pFsMmApi->MM_RelDataMemMass(&pBlockData->nRdwMassIdx);
								}
								else
								{
									DWROADBLK roadBlk = {0};
									ColRdwBlock(pBlockData->nRdwMassIdx, &roadBlk);
									for(nuDWORD n = 0; n < roadBlk.pDwRoadHead->nRoadCount; n++)
									{
										roadBlk.pRoadRect[n].left = roadBlk.pRoadRect[n].right = 
											roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx ].x;
										roadBlk.pRoadRect[n].top = roadBlk.pRoadRect[n].bottom = 
											roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx ].y;
										
//										nuroRECT rtBlockID;
//										nuBlockIDtoRect(pBlockData->nBlockID, &rtBlockID);
										
										for(nuDWORD j = 1; j < roadBlk.pDwRoadData[n].nVertexCount; j++)
										{
											if( roadBlk.pRoadRect[n].left > roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].x )
											{
												roadBlk.pRoadRect[n].left = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].x;
											}
											else if( roadBlk.pRoadRect[n].right < roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].x )
											{
												roadBlk.pRoadRect[n].right = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].x;
											}
											//
											if( roadBlk.pRoadRect[n].top > roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].y )
											{
												roadBlk.pRoadRect[n].top = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].y;
											}
											else if( roadBlk.pRoadRect[n].bottom < roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].y )
											{
												roadBlk.pRoadRect[n].bottom = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+j ].y;
											}
											
										}
									}
								}
							}
						}
					}
				}
				pBlockData = pBlockData->pNext;
			}
			nuFclose(pFile);
			pFile = NULL;
		}
	}

//	DEBUGSTRING(nuTEXT("ReadBk 3"));
	//Read Poi Data...
	nuTcscpy(tsFile, nuTEXT(".PDW"));//Get Pdw file path
	if( LibFS_FindFileWholePath(nDwIdx, tsFile, NURO_MAX_PATH) )
	{
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if(	pFile != NULL )
		{
			pBlockData = m_pDwDataList[nDwIdx].pBlockList;
			while( pBlockData != NULL )
			{
				//Don't need to read poi in Extended blocks
				if( g_pFsMmApi->MM_GetDataMassAddr(pBlockData->nPdwMassIdx) == NULL )//This block Pdw hasn't been loaden.
				{
					/* 2009.06.11
					bFind = nuFALSE;
					if( pBlockData->nBlockIdx > 0 )
					{
						i = pBlockData->nBlockIdx;
						bFind = nuTRUE;
					}
					else
					{
						for(i = 0; i < bhFile.bhStaticHead.nBlockCount; i++)
						{
							if( pBlockData->nBlockID == bhFile.pBlockIDList[i] )
							{
								pBlockData->nBlockIdx = i;
								bFind = nuTRUE;
								break;
							}
						}
					}
					*/
					bFind = nuTRUE;
					//2009.06.11
					if( bFind )
					{
						nBhDis = sizeof(BHSTATICHEAD) + bhFile.bhStaticHead.nBlockCount * 4 + 
							pBlockData->nBlockIdx * 4;
						if( nuFseek(pFileBh, nBhDis, NURO_SEEK_SET) != 0 ||
							nuFread(&nAddress, 4, 1, pFileBh) != 1 || 
							nAddress < 0 )
						{
							bFind = nuFALSE;
						}
					}
					if( bFind && nuFseek(pFile, nAddress, NURO_SEEK_SET) == 0 )
					{
						DWPOIHEAD poiHead = {0};
						if( nuFread(&poiHead, sizeof(DWPOIHEAD), 1, pFile) == 1 )
						{
							nSize = poiHead.nPOICount * sizeof(DWPOIDATA) + sizeof(DWPOIHEAD);
							pMem = g_pFsMmApi->MM_GetDataMemMass(nSize, &pBlockData->nPdwMassIdx);
							if( pMem != NULL )
							{
								nuMemcpy(pMem, &poiHead, sizeof(DWPOIHEAD));
								pMem = (nuPVOID)(((nuLONG)pMem) + sizeof(DWPOIHEAD));
								if( nuFread(pMem, nSize - sizeof(DWPOIHEAD), 1, pFile) != 1 )
								{
									g_pFsMmApi->MM_RelDataMemMass(&pBlockData->nPdwMassIdx);
								}
								/* test file
								else
								{
									DWPOIBLK poiBlk;
									if( ColPdwBlock(pBlockData->nPdwMassIdx, &poiBlk) )
									{
										for( nuINT m = 0; m < poiBlk.pDwPOIHead->nPOICount; m++ )
										{
											poiBlk.pDwPOIData[m].spCoor;
										}
									}
								}
								*/
							}
						}
					}
				}
				pBlockData = pBlockData->pNext;
			}
			nuFclose(pFile);
			pFile = NULL;
		}
	}

//	DEBUGSTRING(nuTEXT("ReadBk 4"));
	//Read BgLine Data...
	nuTcscpy(tsFile, nuTEXT(".BDW"));//Get Pdw file path
	if( LibFS_FindFileWholePath(nDwIdx, tsFile, NURO_MAX_PATH) )
	{
		pFile = nuTfopen(tsFile, NURO_FS_RB);
		if(	pFile != NULL )
		{
			pBlockData = m_pDwDataList[nDwIdx].pBlockList;
			while( pBlockData != NULL )
			{
				if( g_pFsMmApi->MM_GetDataMassAddr(pBlockData->nBdwLineMassIdx) == NULL )//This block BgLine hasn't been loaden.
				{
					/* 2009.06.11
					bFind = nuFALSE;
					if( pBlockData->nBlockIdx > 0 )
					{
						i = pBlockData->nBlockIdx;
						bFind = nuTRUE;
					}
					else
					{
						for(i = 0; i < bhFile.bhStaticHead.nBlockCount; i++)
						{
							if( pBlockData->nBlockID == bhFile.pBlockIDList[i] )
							{
								pBlockData->nBlockIdx = i;
								bFind = nuTRUE;
								break;
							}
						}
					}
					*/
					bFind = nuTRUE;
					//2009.06.11
					if( bFind )
					{
						nBhDis = sizeof(BHSTATICHEAD) + bhFile.bhStaticHead.nBlockCount * 24 + 
							pBlockData->nBlockIdx * 4;
						if( nuFseek(pFileBh, nBhDis, NURO_SEEK_SET) != 0 ||
							nuFread(&nAddress, 4, 1, pFileBh) != 1 || 
							nAddress < 0 )
						{
							bFind = nuFALSE;
						}
					}
					if( bFind && nuFseek(pFile, nAddress, NURO_SEEK_SET) == 0 )
					{
						DWBGLINEHEAD bgLineHead = {0};
						if( nuFread(&bgLineHead, sizeof(DWBGLINEHEAD), 1, pFile) == 1 )
						{
							nSize = sizeof(DWBGLINEHEAD) + 
								bgLineHead.nBgLineCount * sizeof(DWBGLINEDATA) + 

								bgLineHead.nVertexCount * sizeof(NUROSPOINT);
							pMem = g_pFsMmApi->MM_GetDataMemMass(nSize, &pBlockData->nBdwLineMassIdx);
							if( pMem != NULL )
							{
								nuMemcpy(pMem, &bgLineHead, sizeof(DWBGLINEHEAD));
								pMem = (nuPVOID)((nuLONG)pMem + sizeof(DWBGLINEHEAD));
								if( nuFread(pMem, nSize - sizeof(DWBGLINEHEAD), 1, pFile) != 1 )
								{
									g_pFsMmApi->MM_RelDataMemMass(&pBlockData->nBdwLineMassIdx);
								}
							}
						}
					}
				}
				pBlockData = pBlockData->pNext;
			}
//			nuFclose(pFile);//It will been used in reading BgArea
//			pFile = NULL;
		}
	}	
//	DEBUGSTRING(nuTEXT("ReadBk 6"));
	//Read BgArea Data...
	/*
	if( nuFseek(pFileBh, sizeof(BHSTATICHEAD) + 32*bhFile.bhStaticHead.nBlockCount, NURO_SEEK_SET) == 0	&&
		nuFread(pAddr, 4, bhFile.bhStaticHead.nBlockCount, pFileBh) == bhFile.bhStaticHead.nBlockCount	)*/
	{
		if(	pFile != NULL )
		{
			pBlockData = m_pDwDataList[nDwIdx].pBlockList;
			while( pBlockData != NULL )
			{
				if( g_pFsMmApi->MM_GetDataMassAddr(pBlockData->nBdwAreaMassIdx) == NULL )//This block BgArea hasn't been loaden.
				{
					/* 2009.06.11
					bFind = nuFALSE;
					if( pBlockData->nBlockIdx > 0 )
					{
						i = pBlockData->nBlockIdx;
						bFind = nuTRUE;
					}
					else
					{
						for(i = 0; i < bhFile.bhStaticHead.nBlockCount; i++)
						{
							if( pBlockData->nBlockID == bhFile.pBlockIDList[i] )
							{
								pBlockData->nBlockIdx = i;
								bFind = nuTRUE;
								break;
							}
						}
					}
					*/
					bFind = nuTRUE;
					//2009.06.11
					if( bFind )
					{
						nBhDis = sizeof(BHSTATICHEAD) + bhFile.bhStaticHead.nBlockCount * 32 + 
							pBlockData->nBlockIdx * 4;
						if( nuFseek(pFileBh, nBhDis, NURO_SEEK_SET) != 0 ||
							nuFread(&nAddress, 4, 1, pFileBh) != 1 || 
							nAddress < 0 )
						{
							bFind = nuFALSE;
						}
					}
					if( bFind && nuFseek(pFile, nAddress, NURO_SEEK_SET) == 0)
					{
						DWBGAREAHEAD bgAreaHead = {0};
						if( nuFread(&bgAreaHead, sizeof(DWBGAREAHEAD), 1, pFile) == 1 )
						{
							nSize = sizeof(DWBGAREAHEAD) + 
									bgAreaHead.nBgAreaCount * sizeof(DWBGAREADATA) + 
									bgAreaHead.nVertexCount * sizeof(NUROSPOINT);
							pMem = g_pFsMmApi->MM_GetDataMemMass(nSize, &pBlockData->nBdwAreaMassIdx);
							if( pMem != NULL )
							{
								nuMemcpy(pMem, &bgAreaHead, sizeof(DWBGAREAHEAD));
								pMem = (nuPVOID)((nuLONG)pMem + sizeof(DWBGAREAHEAD));
								if( nuFread(pMem, nSize - sizeof(DWBGAREAHEAD), 1, pFile) != 1 )
								{
									g_pFsMmApi->MM_RelDataMemMass(&pBlockData->nBdwAreaMassIdx);
								}
							}
						}
					}
				}
				pBlockData = pBlockData->pNext;
			}
			nuFclose(pFile);
			pFile = NULL;
		}
	}/**/
	//
//	delete[] bhFile.pBlockIDList;// 2009.06.11
	nuFclose(pFileBh);
	if( pFile != NULL )
	{
		nuFclose(pFile);
	}
	return nuTRUE;
}

nuBOOL CBlockDw::ColRdwBlock(nuWORD nMassIdx, PDWROADBLK pRoadBlk)
{
	nuPBYTE pByte = (nuPBYTE)g_pFsMmApi->MM_GetDataMassAddr(nMassIdx);
	if( pByte == NULL )
	{
		return nuFALSE;
	}
	pRoadBlk->pDwRoadHead = (PDWROADHEAD)pByte;
	pByte += sizeof(DWROADHEAD);
	pRoadBlk->pDwRoadData = (PDWROADDATA)pByte;
	pByte += sizeof(DWROADDATA)*pRoadBlk->pDwRoadHead->nRoadCount;
	pRoadBlk->pDwRoadCommon = (PDWROADCOMMON)pByte;
	pByte += sizeof(DWROADCOMMON)*pRoadBlk->pDwRoadHead->nRoadCommonCount;
	pRoadBlk->pSPointCoor = (PNUROSPOINT)pByte;
	pByte += sizeof(PNUROSPOINT)*pRoadBlk->pDwRoadHead->nVertexCount;
	pRoadBlk->pRoadRect = (PNUROSRECT)pByte;
	return nuTRUE;
}

nuBOOL CBlockDw::ColPdwBlock(nuWORD nMassIdx, PDWPOIBLK pPoiBlk)
{
	nuPBYTE pByte = (nuPBYTE)g_pFsMmApi->MM_GetDataMassAddr(nMassIdx);
	if( pByte == NULL )
	{
		return nuFALSE;
	}
	pPoiBlk->pDwPOIHead = (PDWPOIHEAD)pByte;
	pByte += sizeof(DWPOIHEAD);
	pPoiBlk->pDwPOIData = (PDWPOIDATA)pByte;
	return nuTRUE;
}

nuBOOL CBlockDw::ColBdwBgLineBlock(nuWORD nMassIdx, PDWBGLINEBLK pDwBgLineBlk)
{
	nuPBYTE pByte = (nuPBYTE)g_pFsMmApi->MM_GetDataMassAddr(nMassIdx);
	if( pByte == NULL )
	{
		return nuFALSE;
	}
	pDwBgLineBlk->pDwBgLineHead = (PDWBGLINEHEAD)pByte;
	pByte += sizeof(DWBGLINEHEAD);
	pDwBgLineBlk->pDwBgLineData = (PDWBGLINEDATA)pByte;
	pByte += sizeof(DWBGLINEDATA)*pDwBgLineBlk->pDwBgLineHead->nBgLineCount;
	pDwBgLineBlk->pSPointCoor = (PNUROSPOINT)pByte;
	return nuTRUE;
}

nuBOOL CBlockDw::ColBdwBgAreaBlock(nuWORD nMassIdx, PDWBGAREABLK pDwBgAreaBlk)
{
	nuPBYTE pByte = (nuPBYTE)g_pFsMmApi->MM_GetDataMassAddr(nMassIdx);
	if( pByte == NULL )
	{
		return nuFALSE;
	}
	pDwBgAreaBlk->pDwBgAreaHead = (PDWBGAREAHEAD)pByte;
	pByte += sizeof(DWBGAREAHEAD);
	pDwBgAreaBlk->pDwBgAreaData = (PDWBGAREADATA)pByte;
	pByte += sizeof(DWBGAREADATA)*pDwBgAreaBlk->pDwBgAreaHead->nBgAreaCount;
	pDwBgAreaBlk->pSPointCoor = (PNUROSPOINT)pByte;
	return nuTRUE;
}

nuPVOID CBlockDw::GetScreenMap()
{
	if( ColScreenMap() )
	{
		return &m_screenMap;
	}
	else
	{
		return NULL;
	}
}

nuBOOL CBlockDw::ColScreenMap()
{
	RelScreenMap();
	nuWORD i;
	for(i = 0; i < m_nDwCount; i++)
	{
		if(m_pDwDataList[i].pBlockList != NULL )
		{
			m_screenMap.nDwCount ++;
		}
	}
	if( m_screenMap.nDwCount == 0 )
	{
		return nuFALSE;
	}
//	_nuDGSI(nuTEXT("m_screenMap.nDwCount"), m_screenMap.nDwCount);
	m_screenMap.pDwList = new MAPNODE[m_screenMap.nDwCount];
	if( m_screenMap.pDwList == NULL )
	{
		m_screenMap.nDwCount = 0;
		return nuFALSE;
	}
	nuMemset(m_screenMap.pDwList, 0, sizeof(MAPNODE)*m_screenMap.nDwCount);
	nuWORD j = 0;
	for( i = 0; i < m_nDwCount && j < m_screenMap.nDwCount; i++ )
	{
		if( m_pDwDataList[i].pBlockList != NULL )
		{
			m_screenMap.pDwList[j].pBlockList = new BLOCKNODE[m_pDwDataList[i].nNumOfBlocksCovered];
			if( m_screenMap.pDwList[j].pBlockList == NULL )
			{
				m_screenMap.nDwCount--;
				continue;
			}
			m_screenMap.pDwList[j].nNumOfBlocks = m_pDwDataList[i].nNumOfBlocksCovered;
			m_screenMap.pDwList[j].nDwIdx = i;
			nuMemset(m_screenMap.pDwList[j].pBlockList, 0, sizeof(BLOCKNODE)*m_pDwDataList[i].nNumOfBlocksCovered);
			//
			nuWORD k = 0;
			PBLOCKDATA pBlockData = m_pDwDataList[i].pBlockList;
			while( pBlockData != NULL && k < m_screenMap.pDwList[j].nNumOfBlocks )
			{
				m_screenMap.pDwList[j].pBlockList[k].nBlockID = pBlockData->nBlockID;
				m_screenMap.pDwList[j].pBlockList[k].nBlockIdx= pBlockData->nBlockIdx;
				ColRdwBlock(pBlockData->nRdwMassIdx, &m_screenMap.pDwList[j].pBlockList[k].roadBlk);
				ColPdwBlock(pBlockData->nPdwMassIdx, &m_screenMap.pDwList[j].pBlockList[k].poiBlk);
				ColBdwBgLineBlock(pBlockData->nBdwLineMassIdx, &m_screenMap.pDwList[j].pBlockList[k].bgLineBlk);
				ColBdwBgAreaBlock(pBlockData->nBdwAreaMassIdx, &m_screenMap.pDwList[j].pBlockList[k].bgAreaBlk);
				k++;
				pBlockData = pBlockData->pNext;
			}
			j++;
		}
	}
	return nuTRUE;
}

nuVOID CBlockDw::RelScreenMap()
{
	if( m_screenMap.pDwList != NULL )
	{
		for(nuWORD i = 0; i < m_screenMap.nDwCount; i++)
		{
			if( m_screenMap.pDwList[i].pBlockList != NULL )
			{
				delete []m_screenMap.pDwList[i].pBlockList;
				m_screenMap.pDwList[i].pBlockList=NULL;
			}
		}
		delete []m_screenMap.pDwList;
		m_screenMap.pDwList = NULL;
	}
	m_screenMap.nDwCount = 0;
}

//
nuBOOL CBlockDw::SeekForNaviPt(nuLONG x, nuLONG y, nuDWORD nCoverDis, nuLONG nAngle, PSEEKEDROADDATA pRoadData)
{
	if( pRoadData == NULL )
	{
		return nuFALSE;
	}
	pRoadData->nDis	= nuLOWORD(nCoverDis);
	//
	BLOCKSNODE	pBlockList[MAX_BLOCKSNODE_LIST] = {0};
	nuWORD	nBlockCount = 0;
	nuroRECT rect = {0};
	BLOCKIDARRAY blockIDArray= {0};
	nuDWORD blockID = 0;
//	nuWORD dwIdx;
	rect.left	= x - nCoverDis;
	rect.top	= y - nCoverDis;
	rect.right	= x + nCoverDis;
	rect.bottom = y + nCoverDis;
	nuRectToBlockIDArray(rect, &blockIDArray);
	nuBOOL bReadNew = nuFALSE;
	DWROADBLK roadBlk = {0};
	PBLOCKDATA pBlockData = NULL;
	nuBOOL bFind = nuFALSE;
	//
	nuLONG i;
	for( i = 0; i < MAX_BLOCKSNODE_LIST; ++i )
	{
		pBlockList[i].nMemMassIdx = nuWORD(-1);
	}
	for( i = blockIDArray.nYstart; i <= blockIDArray.nYend; i++ )
	{
		for( nuLONG j = blockIDArray.nXstart; j <= blockIDArray.nXend; j++ )
		{
			if( nBlockCount == MAX_BLOCKSNODE_LIST )
			{
				break;
			}
			//2009.06.16
			BLK_OUT_DATA blkOut = {0};
			if( m_pFileFh->BlockIDToMap(nuLOWORD(j), nuLOWORD(i), &blkOut) )
			{
				blockID = i*XBLOCKNUM + j;
				for(nuBYTE k = 0; k < blkOut.nMapFixed; ++k)
				{
					bReadNew = nuTRUE;
					pBlockData = m_pDwDataList[ blkOut.pBlkFixData[k].nMapIdx ].pBlockList;
					while( pBlockData != NULL )
					{
						if( pBlockData->nBlockID == blockID )
						{
							if( ColRdwBlock(pBlockData->nRdwMassIdx, &roadBlk) )
							{
								if( SeekBlkForRoad(x, y, nAngle, pRoadData, &roadBlk, blockID) )
								{
									pRoadData->nDwIdx	= blkOut.pBlkFixData[k].nMapIdx;
									pRoadData->nBlockID	= pBlockData->nBlockID;
									pRoadData->nBlkIdx	= pBlockData->nBlockIdx;
									bFind = nuTRUE;
								}
							}
							bReadNew = nuFALSE;
							break;
						}
						pBlockData = pBlockData->pNext;
					}
					if( bReadNew )
					{
						pBlockList[nBlockCount].nDwIdx	= blkOut.pBlkFixData[k].nMapIdx;
						pBlockList[nBlockCount].nBlkIdx	= blkOut.pBlkFixData[k].nBlkIdx;
						pBlockList[nBlockCount].nBlkID	= blockID;
						nBlockCount++;
						if( nBlockCount == MAX_BLOCKSNODE_LIST )
						{
							break;
						}
					}//need to read Block;
				}
			}

			/* 2009.06.16
			blockID = i*XBLOCKNUM + j;
			for (nuWORD k = 0 ; k < m_nDwCount; k++ )
			{
				if( !m_fileFB.BlockIDInMap(blockID, k) )
				{
					continue;
				}
				pBlockList[nBlockCount].nDwIdx	= k;
				bReadNew = nuTRUE;
				pBlockData = m_pDwDataList[k].pBlockList;
				while( pBlockData != NULL )
				{
					if( pBlockData->nBlockID == blockID )
					{
						if( ColRdwBlock(pBlockData->nRdwMassIdx, &roadBlk) )
						{
							if( SeekBlkForRoad(x, y, nAngle, pRoadData, &roadBlk, blockID) )
							{
								pRoadData->nDwIdx	= k;
								pRoadData->nBlockID	= pBlockData->nBlockID;
								pRoadData->nBlkIdx	= pBlockData->nBlockIdx;
								bFind = nuTRUE;
							}
						}
						bReadNew = nuFALSE;
						break;
					}
					pBlockData = pBlockData->pNext;
				}
				if( bReadNew )
				{
					pBlockList[nBlockCount].nBlkID	= blockID;
					nBlockCount++;
				}//need to read Block;
			}*/
		}
		if( nBlockCount == MAX_BLOCKSNODE_LIST )
		{
			break;
		}
	}
	if( nBlockCount > 0 )
	{
		ReadRdwBlocks(pBlockList, nBlockCount);
		//
		for(nuWORD i = 0; i < nBlockCount; i++ )
		{
			if( ColRdwBlock(pBlockList[i].nMemMassIdx, &roadBlk) )
			{
				if( SeekBlkForRoad(x, y, nAngle, pRoadData, &roadBlk, pBlockList[i].nBlkID) )
				{
					pRoadData->nDwIdx	= pBlockList[i].nDwIdx;
					pRoadData->nBlockID	= pBlockList[i].nBlkID;
					pRoadData->nBlkIdx	= pBlockList[i].nBlkIdx;
					bFind = nuTRUE;
				}
			}
			g_pFsMmApi->MM_RelDataMemMass(&pBlockList[i].nMemMassIdx);
		}
	}
	return bFind;
	/*
	if( pRoadData->nDis < (nuLONG)nCoverDis )
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}*/
}

nuBOOL CBlockDw::PaddingRoad(PSEEKEDROADDATA pRoadData)
{
	if( pRoadData == NULL || pRoadData->nDwIdx >= m_nDwCount )
	{
		return nuFALSE;
	}
	PBLOCKDATA pBlockData = m_pDwDataList[pRoadData->nDwIdx].pBlockList;
	if( pBlockData == NULL )
	{
		return nuFALSE;
	}
	nuBOOL bFind = nuFALSE;
	DWROADBLK roadBlk = {0};
	while( pBlockData != NULL )
	{
		if( pBlockData->nBlockIdx == pRoadData->nBlkIdx )
		{
			if( ColRdwBlock(pBlockData->nRdwMassIdx, &roadBlk) &&
				pRoadData->nRoadIdx < roadBlk.pDwRoadHead->nRoadCount )
			{
				bFind = nuTRUE;
				pRoadData->nLimitSpeed	= roadBlk.pDwRoadCommon[ roadBlk.pDwRoadData[ pRoadData->nRoadIdx ].nRoadCommonIdx ].nSpeedLimit;
			}
			break;
		}
		pBlockData = pBlockData->pNext;
	}
	return bFind;
}
nuBOOL CBlockDw::SeekForPoi(nuLONG x, nuLONG y, nuDWORD nCoverDis, nuLONG nAngle, PSEEKEDPOIDATA pPoiData)
{
	if( pPoiData == NULL )
	{
		return nuFALSE;
	}
	pPoiData->nDis	= nCoverDis;
	nuBOOL bFind = nuFALSE;
	nuroRECT rect = {0}, rtBlock = {0};
	DWPOIBLK dwPoiBlk = {0};
	PBLOCKDATA pBlockData = NULL;
	rect.left	= x - nCoverDis;
	rect.top	= y - nCoverDis;
	rect.right	= x + nCoverDis;
	rect.bottom = y + nCoverDis;
	PGLOBALENGINEDATA	pData = (PGLOBALENGINEDATA)LibFS_GetData(DATA_GLOBAL);
	pData->drawInfoMap.nNowCenterPoiCount = 0;
	pData->drawInfoMap.nPoiSltIdx	= 0;
	
	for( nuWORD i = 0; i < m_nDwCount; i++)
	{
		if( m_pDwDataList[i].pBlockList == NULL )
		{
			continue;
		}
		pBlockData = m_pDwDataList[i].pBlockList;
		while( pBlockData != NULL )
		{
			nuBlockIDtoRect(pBlockData->nBlockID, &rtBlock);
			if( nuRectCoverRect(&rect, &rtBlock) )
			{
				if( ColPdwBlock(pBlockData->nPdwMassIdx, &dwPoiBlk) )
				{
//					nuLONG nPidx = pData->drawInfoMap.nNowCenterPoiCount;
					BLOCKINFO blockInfo  = {0};
					blockInfo.nMapIdx	= i;
					blockInfo.nBlockID	= pBlockData->nBlockID;
					blockInfo.nBlockIdx	= pBlockData->nBlockIdx;
					if( SeekBlkForPoi(x, y, nAngle, pPoiData, &dwPoiBlk, pBlockData->nBlockID, &blockInfo, CENTERPOI_SHOW_MAX_DIS) )
					{
						pPoiData->nBlockID	= pBlockData->nBlockID;
						pPoiData->nBlockIdx	= pBlockData->nBlockIdx;
						pPoiData->nMapIdx	= i;
						bFind = nuTRUE;
					}
				}
			}
			pBlockData = pBlockData->pNext;
		}
	}
	return bFind;
}

nuVOID CBlockDw::SortBlockNodesByDw(CBlockDw::tagBLOCKSNODE* pBlockNode, nuWORD nCount)
{
	;
}

nuBOOL CBlockDw::ReadRdwBlocks(CBlockDw::tagBLOCKSNODE* pBlockNode, nuWORD nCount)
{
	if( pBlockNode == NULL )
	{
		return nuFALSE;
	}
	nuLONG nOldDwIdx = -1;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuFILE *pFileBh = NULL, *pFileRdw = NULL;
	BHFILE bhFile = {0};
//	nuLONG* pAddr = NULL;
//	bhFile.pBlockIDList	= NULL;
//	nuBOOL bRead;
	for(nuWORD i = 0; i < nCount; i++)
	{
		if( nOldDwIdx != pBlockNode[i].nDwIdx )
		{
			if( pFileBh != NULL )
			{
				nuFclose(pFileBh);
			}
			pFileBh = NULL;
			if( pFileRdw != NULL )
			{
				nuFclose(pFileRdw);
				pFileRdw = NULL;
			}
			nOldDwIdx = pBlockNode[i].nDwIdx;
		}
		if( pFileBh == NULL )//Open BhFile and read bhFile used.
		{
			nuTcscpy(tsFile, nuTEXT(".BH"));
			if( !LibFS_FindFileWholePath(nOldDwIdx, tsFile, NURO_MAX_PATH) )
			{
				continue;
			}
			pFileBh = nuTfopen(tsFile, NURO_FS_RB);
			if( pFileBh == NULL )
			{
				continue;
			}
			if( nuFseek(pFileBh, 0, NURO_SEEK_SET) != 0 ||
				nuFread(&(bhFile.bhStaticHead), sizeof(BHSTATICHEAD), 1, pFileBh) != 1 )
			{ 
				nuFclose(pFileBh);
				continue;
			}//Read Bh header
			/* 2009.06.16
			if( bhFile.pBlockIDList != NULL )
			{
				delete []bhFile.pBlockIDList;
				bhFile.pBlockIDList = NULL;
			}
			bhFile.pBlockIDList = new nuDWORD[bhFile.bhStaticHead.nBlockCount];
			if( bhFile.pBlockIDList == NULL )
			{
				nuFclose(pFileBh);
				continue;
			}
			if( nuFread(bhFile.pBlockIDList, 4, bhFile.bhStaticHead.nBlockCount, pFileBh) != bhFile.bhStaticHead.nBlockCount )
			{
				delete []bhFile.pBlockIDList;
				bhFile.pBlockIDList = NULL;
				nuFclose(pFileBh);
				continue;
			}//Read BlockID list
			if( pAddr != NULL )
			{
				delete []pAddr;
				pAddr = NULL;
			}
			pAddr = new nuLONG[bhFile.bhStaticHead.nBlockCount];
			if( pAddr == NULL )
			{
				delete[] bhFile.pBlockIDList;
				bhFile.pBlockIDList = NULL;
				nuFclose(pFileBh);
				continue;
			}
			if( nuFseek(pFileBh, 8 * bhFile.bhStaticHead.nBlockCount, NURO_SEEK_CUR) != 0 ||
				nuFread(pAddr, 4, bhFile.bhStaticHead.nBlockCount, pFileBh) != bhFile.bhStaticHead.nBlockCount )
			{
				delete []pAddr;
				pAddr = NULL;
				delete[] bhFile.pBlockIDList;
				bhFile.pBlockIDList = NULL;
				nuFclose(pFileBh);
				continue;
			}
			*/
		}
		/*2009.06.16
		bRead = nuFALSE;
		nuDWORD j;
		for(j = 0; j < bhFile.bhStaticHead.nBlockCount; j++)
		{
			if( bhFile.pBlockIDList[j] == pBlockNode[i].nBlkID )
			{
				if( pAddr[j] != -1 )
				{
					bRead = nuTRUE;
				}
				break;
			}
		}
		if( !bRead )
		{
			continue;
		}
		*/
		//2009.06.16
		nuLONG nBhDis = sizeof(BHSTATICHEAD) + bhFile.bhStaticHead.nBlockCount * 12 + 
			pBlockNode[i].nBlkIdx * 4;
		nuLONG nAddress = 0;
		if( nuFseek(pFileBh, nBhDis, NURO_SEEK_SET) != 0 ||
			nuFread(&nAddress, 4, 1, pFileBh) != 1		 || 
			nAddress <= 0 )
		{
			continue;
		}
		//
		if( pFileRdw == NULL )//Open Rdw file
		{
			nuTcscpy(tsFile, nuTEXT(".RDW"));//Get Rdw file path
			if( !LibFS_FindFileWholePath(nOldDwIdx, tsFile, NURO_MAX_PATH) )
			{
				continue;
			}
			pFileRdw = nuTfopen(tsFile, NURO_FS_RB);
			if(	pFileRdw == NULL )
			{
				continue;
			}
		}
		if( nuFseek(pFileRdw, nAddress, NURO_SEEK_SET) != 0 )
		{
			continue;
		}
		DWROADHEAD dwRoadHead = {0};
		if( nuFread(&dwRoadHead, sizeof(DWROADHEAD), 1, pFileRdw) != 1 )
		{
			continue;
		}
		nuDWORD nSize = sizeof(DWROADHEAD) + dwRoadHead.nRoadCount * sizeof(DWROADDATA) + 
			dwRoadHead.nRoadCommonCount * sizeof(DWROADCOMMON) + dwRoadHead.nVertexCount * sizeof(NUROSPOINT) +
			dwRoadHead.nRoadCount * sizeof(NUROSRECT);
		nuPBYTE pByte = (nuPBYTE)g_pFsMmApi->MM_GetDataMemMass(nSize, &pBlockNode[i].nMemMassIdx);
		if( pByte == NULL )
		{
			continue;
		}
		nuMemcpy(pByte, &dwRoadHead, sizeof(DWROADHEAD));
		pByte = pByte + sizeof(DWROADHEAD);
		nSize -= sizeof(DWROADHEAD);
		if( nuFread(pByte, nSize, 1, pFileRdw) != 1 )
		{
			g_pFsMmApi->MM_RelDataMemMass(&pBlockNode[i].nMemMassIdx);
			continue;
		}
		DWROADBLK roadBlk = {0};
//2009.06.16		pBlockNode[i].nBlkIdx	= j;
		ColRdwBlock(pBlockNode[i].nMemMassIdx, &roadBlk);
		for(nuDWORD n = 0; n < roadBlk.pDwRoadHead->nRoadCount; n++)
		{
			roadBlk.pRoadRect[n].left = roadBlk.pRoadRect[n].right = 
			roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx ].x;
			roadBlk.pRoadRect[n].top = roadBlk.pRoadRect[n].bottom = 
			roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx ].y;
			for(nuDWORD m = 1; m < roadBlk.pDwRoadData[n].nVertexCount; m++)
			{
				if( roadBlk.pRoadRect[n].left > roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+m ].x )
				{
					roadBlk.pRoadRect[n].left = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+m ].x;
				}
				else if( roadBlk.pRoadRect[n].right < roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+m ].x )
				{
					roadBlk.pRoadRect[n].right = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+m ].x;
				}
				if( roadBlk.pRoadRect[n].top > roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+m ].y )
				{
					roadBlk.pRoadRect[n].top = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+m ].y;
				}
				else if( roadBlk.pRoadRect[n].bottom < roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+m ].y )
				{
					roadBlk.pRoadRect[n].bottom = roadBlk.pSPointCoor[ roadBlk.pDwRoadData[n].nVertexAddrIdx+m ].y;
				}				
			}
		}
	}
	if( pFileRdw != NULL )
	{
		nuFclose(pFileRdw);
	}
	if( pFileBh != NULL )
	{
		nuFclose(pFileBh);
	}
	/* 2009.06.16
	if( bhFile.pBlockIDList != NULL )
	{
		delete []bhFile.pBlockIDList;
	}
	if( pAddr != NULL )
	{
		delete []pAddr;
	}
	*/
	return nuTRUE;
}

nuVOID CBlockDw::RelsRdwBlocks(CBlockDw::tagBLOCKSNODE* pBlockNode, nuWORD nCount)
{
	for(nuWORD i = 0; i < nCount; i++)
	{
		g_pFsMmApi->MM_RelDataMemMass(&pBlockNode[i].nMemMassIdx);
	}
}

nuBOOL CBlockDw::SeekBlkForRoad(nuLONG x, nuLONG y, nuLONG nAngle, PSEEKEDROADDATA pRoadData, PDWROADBLK pRoadBlk, nuLONG nBlockID)
{
	nuroRECT rtBlock = {0}, rtRoad = {0};
	nuroPOINT point = {0}, pt1 = {0}, pt2 = {0}, ptFixed = {0};
	nuLONG dx = 0, dy = 0, tmpAngle = 0, tmpDis = 0;
	nuBlockIDtoRect(nBlockID, &rtBlock);
	point.x = x - rtBlock.left;
	point.y = y - rtBlock.top;
	nuBOOL bFind = nuFALSE;
	nuBYTE nAnti = 0;
	for( nuDWORD i = 0; i < pRoadBlk->pDwRoadHead->nRoadCount; i++ )
	{
#ifdef VALUE_EMGRT
		if( pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nRoadType >= ROAD_CLASS_IGNORE )
		{
			continue;
		}
#endif
		rtRoad.left		= pRoadBlk->pRoadRect[i].left - pRoadData->nDis;
		rtRoad.top		= pRoadBlk->pRoadRect[i].top - pRoadData->nDis;
		rtRoad.right	= pRoadBlk->pRoadRect[i].right + pRoadData->nDis;
		rtRoad.bottom	= pRoadBlk->pRoadRect[i].bottom + pRoadData->nDis;
		if( !nuPtInRect(point, rtRoad) )
		{
			continue;
		}
		pt2.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx ].x;
		pt2.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx ].y;
		for( nuDWORD j = 1; j < pRoadBlk->pDwRoadData[i].nVertexCount; j++ )
		{
			pt1 = pt2;
			pt2.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx + j ].x;
			pt2.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx + j ].y;
			rtRoad.left		= NURO_MIN(pt1.x, pt2.x) - pRoadData->nDis;
			rtRoad.right	= NURO_MAX(pt1.x, pt2.x) + pRoadData->nDis;
			rtRoad.top		= NURO_MIN(pt1.y, pt2.y) - pRoadData->nDis;
			rtRoad.bottom	= NURO_MAX(pt1.y, pt2.y) + pRoadData->nDis;
			if( !nuPtInRect(point, rtRoad) )
			{
				continue;
			}
			nuBOOL bAnti = nuFALSE;
			if( nAngle >= 0 )
			{
				dx = pt2.x - pt1.x;
				dy = pt2.y - pt1.y;
				tmpAngle = (nuLONG)(nuAtan2(dy, dx) * 180/PI);
				tmpAngle = (nAngle - tmpAngle) % 360;
				tmpAngle = NURO_ABS(tmpAngle);
				if( tmpAngle > 180 )
				{
					tmpAngle = 360 - tmpAngle;
				}
				if( pRoadBlk->pDwRoadData[i].nOneWay == 1 )
				{
					if( tmpAngle > ANGLE_FIXTOROAD )
					{
						continue;
					}
				}
				else if( pRoadBlk->pDwRoadData[i].nOneWay == 2 )
				{
					if( tmpAngle < 180 - ANGLE_FIXTOROAD )
					{
						continue;
					}
					bAnti = nuTRUE;
					dx = -dx;
					dy = -dy;
				}
				else
				{
					if( tmpAngle > ANGLE_FIXTOROAD && tmpAngle < 180 - ANGLE_FIXTOROAD )
					{
						continue;
					}
					else if( tmpAngle >= 180 - ANGLE_FIXTOROAD )
					{
						bAnti = nuTRUE;
						dx = -dx;
						dy = -dy;
					}
				}
			}
			if( nuPtFixToLineSeg(point, pt1, pt2, &ptFixed, &tmpDis) && tmpDis < pRoadData->nDis )
			{
				pRoadData->nDis			= (short)tmpDis;
				pRoadData->nRoadIdx		= i;
				pRoadData->nNameAddr	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nRoadNameAddr;
				pRoadData->nRoadClass	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nRoadType;
				pRoadData->nCityID		= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nCityID;
				pRoadData->nTownID		= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nTownID;
				pRoadData->ptMapped.x 	= ptFixed.x + rtBlock.left;
				pRoadData->ptMapped.y	= ptFixed.y + rtBlock.top;
				pRoadData->nPtIdx		= (nuWORD)(j - 1);
				pRoadData->nLimitSpeed	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nSpeedLimit;
				pRoadData->nRdDx		= nuLOWORD(dx);
				pRoadData->nRdDy		= nuLOWORD(dy);
				pRoadData->bGPSState    = pRoadBlk->pDwRoadData[i].bGPSState;
				bFind = nuTRUE;
				if( bAnti )
				{
					nAnti = 2;
				}
				else
				{
					nAnti = 1;
				}
			}
		}
	}
	if( bFind )
	{
		if( nAnti == 1 )
		{
			nuINT nVertexIdx = pRoadBlk->pDwRoadData[ pRoadData->nRoadIdx ].nVertexAddrIdx;
			nuINT nVertexcount = pRoadBlk->pDwRoadData[ pRoadData->nRoadIdx ].nVertexCount;
			pt1.x = pRoadBlk->pSPointCoor[ nVertexIdx + pRoadData->nPtIdx + 1 ].x;
			pt1.y = pRoadBlk->pSPointCoor[ nVertexIdx + pRoadData->nPtIdx + 1 ].y;
			nuINT dx = pt1.x - (pRoadData->ptMapped.x - rtBlock.left);
			nuINT dy = pt1.y - (pRoadData->ptMapped.y - rtBlock.top);
			pRoadData->nDisToNextCoor = (nuWORD)nuSqrt(dx*dx + dy*dy);
			pRoadData->ptNextCoor.x = pRoadBlk->pSPointCoor[ nVertexIdx + nVertexcount - 1 ].x + rtBlock.left;
			pRoadData->ptNextCoor.y = pRoadBlk->pSPointCoor[ nVertexIdx + nVertexcount - 1 ].y + rtBlock.top;
			for(nuWORD m = pRoadData->nPtIdx+2; m < nVertexcount; ++m )
			{
				dx = pRoadBlk->pSPointCoor[nVertexIdx + m].x - pRoadBlk->pSPointCoor[nVertexIdx + m -1].x;
				dy = pRoadBlk->pSPointCoor[nVertexIdx + m].y - pRoadBlk->pSPointCoor[nVertexIdx + m -1].y;
				pRoadData->nDisToNextCoor += (nuWORD)nuSqrt(dx*dx + dy*dy);
			}
		}
		else if( nAnti == 2)
		{
			nuINT nVertexIdx = pRoadBlk->pDwRoadData[ pRoadData->nRoadIdx ].nVertexAddrIdx;
			nuINT nVertexcount = pRoadBlk->pDwRoadData[ pRoadData->nRoadIdx ].nVertexCount;
			pt1.x = pRoadBlk->pSPointCoor[ nVertexIdx + pRoadData->nPtIdx].x;
			pt1.y = pRoadBlk->pSPointCoor[ nVertexIdx + pRoadData->nPtIdx].y;
			nuINT dx = pt1.x - (pRoadData->ptMapped.x - rtBlock.left);
			nuINT dy = pt1.y - (pRoadData->ptMapped.y - rtBlock.top);
			pRoadData->nDisToNextCoor = (nuWORD)nuSqrt(dx*dx + dy*dy);
			pRoadData->ptNextCoor.x = pRoadBlk->pSPointCoor[ nVertexIdx ].x + rtBlock.left;
			pRoadData->ptNextCoor.y = pRoadBlk->pSPointCoor[ nVertexIdx ].y + rtBlock.top;
			for(nuWORD m = 0; m < pRoadData->nPtIdx; ++m )
			{
				dx = pRoadBlk->pSPointCoor[nVertexIdx + m].x - pRoadBlk->pSPointCoor[nVertexIdx + m + 1].x;
				dy = pRoadBlk->pSPointCoor[nVertexIdx + m].y - pRoadBlk->pSPointCoor[nVertexIdx + m + 1].y;
				pRoadData->nDisToNextCoor += (nuWORD)nuSqrt(dx*dx + dy*dy);
			}
		}
	}
	return bFind;
}

nuBOOL CBlockDw::SeekBlkForPoi(nuLONG x, nuLONG y, nuLONG nAngle, PSEEKEDPOIDATA pPoiData, PDWPOIBLK pPoiblk,
							 nuLONG nBlockID, PBLOCKINFO pBlockInfo, nuDWORD nCoverDis)
{
	nuroRECT rtBlock = {0}, rtPoi = {0}, rtCenter = {0};
	nuroPOINT point = {0};
	nuLONG dx = 0, dy = 0;//, tmpAngle;
	nuLONG tmpDis = 0;
	nuBlockIDtoRect(nBlockID, &rtBlock);
	point.x = x - rtBlock.left;
	point.y = y - rtBlock.top;

	rtCenter.left	= point.x - nCoverDis;
	rtCenter.top	= point.y - nCoverDis;
	rtCenter.right	= point.x + nCoverDis;
	rtCenter.bottom	= point.y + nCoverDis;
	nuBOOL bFind = nuFALSE;
	PMAPCONFIG pMapCfg = (PMAPCONFIG)LibFS_GetData(DATA_MAPCONFIG);
	PGLOBALENGINEDATA	pData = (PGLOBALENGINEDATA)LibFS_GetData(DATA_GLOBAL);
	for( nuDWORD i = 0; i < pPoiblk->pDwPOIHead->nPOICount; i++ )
	{
		if( /*pData->drawInfoMap.nNowCenterPoiCount < POI_CENTER_MAX_NUM &&*/
			nuSPtInRect(pPoiblk->pDwPOIData[i].spCoor, rtCenter) )
		{
			dx = pPoiblk->pDwPOIData[i].spCoor.x - point.x;
			dy = pPoiblk->pDwPOIData[i].spCoor.y - point.y;
			tmpDis = nulSqrt(dx, dy);
			if ( tmpDis <= (nuLONG)nCoverDis )
			{
				nuINT nPIdx = GetPoiIndex(tmpDis, pData->drawInfoMap.poiCenterNearList, pData->drawInfoMap.nNowCenterPoiCount);
				if( nPIdx >= 0 )
				{
					pData->drawInfoMap.poiCenterNearList[nPIdx].nDis			= tmpDis;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nMapIdx			= (nuWORD)pBlockInfo->nMapIdx;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nBlockID		= pBlockInfo->nBlockID;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nBlockIdx		= pBlockInfo->nBlockIdx;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nCityID			= pPoiblk->pDwPOIData[i].nCityID;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nTownID			= pPoiblk->pDwPOIData[i].nTownID;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nNameAddr		= pPoiblk->pDwPOIData[i].nNameAddr;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nPOINameLen		= pPoiblk->pDwPOIData[i].nPOINameLen;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nPOIType1		= pPoiblk->pDwPOIData[i].nPOIType1;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nPOIType2		= pPoiblk->pDwPOIData[i].nPOIType2;
					pData->drawInfoMap.poiCenterNearList[nPIdx].nReserve		= pPoiblk->pDwPOIData[i].nReserve;
					pData->drawInfoMap.poiCenterNearList[nPIdx].OnterInfoAddr	= pPoiblk->pDwPOIData[i].OnterInfoAddr;
					pData->drawInfoMap.poiCenterNearList[nPIdx].pCoor.x			= pPoiblk->pDwPOIData[i].spCoor.x + rtBlock.left;
					pData->drawInfoMap.poiCenterNearList[nPIdx].pCoor.y			= pPoiblk->pDwPOIData[i].spCoor.y + rtBlock.top;
					if( pData->drawInfoMap.nNowCenterPoiCount < POI_CENTER_MAX_NUM )
					{
						++(pData->drawInfoMap.nNowCenterPoiCount);
					}
				}
			}
		}
		rtPoi.left		= pPoiblk->pDwPOIData[i].spCoor.x - pPoiData->nDis;
		rtPoi.right		= pPoiblk->pDwPOIData[i].spCoor.x + pPoiData->nDis;
		rtPoi.top		= pPoiblk->pDwPOIData[i].spCoor.y - pPoiData->nDis;
		rtPoi.bottom	= pPoiblk->pDwPOIData[i].spCoor.y + pPoiData->nDis;
		if( !pMapCfg->scaleSetting.pPoiSetting[pPoiblk->pDwPOIData[i].nPOIType1].nShowIcon || 
			!nuPtInRect(point, rtPoi) )
		{
			continue;
		}
		dx = pPoiblk->pDwPOIData[i].spCoor.x - point.x;
		dy = pPoiblk->pDwPOIData[i].spCoor.y - point.y;
		tmpDis = (nuLONG)nuSqrt(dx*dx + dy*dy);
		if( pPoiblk->pDwPOIData[i].nNameAddr != -1 && tmpDis < pPoiData->nDis )
		{
			bFind = nuTRUE;
			pPoiData->nDis = tmpDis;
			pPoiData->nCityID	= pPoiblk->pDwPOIData[i].nCityID;
			pPoiData->nTownID	= pPoiblk->pDwPOIData[i].nTownID;
			pPoiData->nNameAddr	= pPoiblk->pDwPOIData[i].nNameAddr;
			pPoiData->nPOINameLen	= pPoiblk->pDwPOIData[i].nPOINameLen;
			pPoiData->nPOIType1		= pPoiblk->pDwPOIData[i].nPOIType1;
			pPoiData->nPOIType2		= pPoiblk->pDwPOIData[i].nPOIType2;
			pPoiData->nReserve		= pPoiblk->pDwPOIData[i].nReserve;
			pPoiData->OnterInfoAddr	= pPoiblk->pDwPOIData[i].OnterInfoAddr;
			pPoiData->pCoor.x		= pPoiblk->pDwPOIData[i].spCoor.x + rtBlock.left;
			pPoiData->pCoor.y		= pPoiblk->pDwPOIData[i].spCoor.y + rtBlock.top;
		}
	}
	return bFind;
}
nuINT CBlockDw::GetPoiIndex(nuLONG nLen, PSEEKEDPOIDATA pPoiList, nuWORD nNowCount)
{
	nuINT k = 0;
	nuINT nCount = 0;
	if( nNowCount < POI_CENTER_MAX_NUM )
	{
		k = nNowCount;
		nCount = nNowCount;
	}
	else
	{
		k = -1;
		nCount = nNowCount - 1;
	}
	for(nuINT i = 0; i < nNowCount; ++i)
	{
		if( nLen >= pPoiList[i].nDis )
		{
			continue;
		}
		k = i;
		//move the following pois
		for(nuINT m = nCount; m > i; --m)
		{
			pPoiList[m] = pPoiList[m-1];
		}
		break;
	}
	return k;
}

nuBOOL CBlockDw::SeekForRoads(PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut, nuBOOL bSetMarket)
{
	if( pSeekRdsIn == NULL || pSeekRdsOut == NULL )
	{
		return nuFALSE;
	}
	pSeekRdsOut->nCountSeeked = 0;
	//
	BLOCKSNODE	pBlockList[MAX_BLOCKSNODE_LIST] = {0};
	nuWORD	nBlockCount = 0;
	nuroRECT rect = {0};
	BLOCKIDARRAY blockIDArray = {0};
	nuDWORD blockID = 0;
//	nuWORD dwIdx;
	rect.left	= pSeekRdsIn->point.x - pSeekRdsIn->nSeekDis;
	rect.top	= pSeekRdsIn->point.y - pSeekRdsIn->nSeekDis;
	rect.right	= pSeekRdsIn->point.x + pSeekRdsIn->nSeekDis;
	rect.bottom = pSeekRdsIn->point.y + pSeekRdsIn->nSeekDis;
	nuRectToBlockIDArray(rect, &blockIDArray);
	nuBOOL bReadNew = nuFALSE;
	DWROADBLK roadBlk = {0};
	PBLOCKDATA pBlockData = NULL;
	nuBOOL bFind = nuFALSE;
	//
	nuLONG i;
	for( i = 0; i < MAX_BLOCKSNODE_LIST; ++i )
	{
		pBlockList[i].nMemMassIdx = nuWORD(-1);
	}
	for( i = blockIDArray.nYstart; i <= blockIDArray.nYend; i++ )
	{
		for( nuLONG j = blockIDArray.nXstart; j <= blockIDArray.nXend; j++ )
		{
			if( nBlockCount == MAX_BLOCKSNODE_LIST )
			{
				break;
			}
			//2009.06.16
			BLK_OUT_DATA blkOut = {0};
			if( m_pFileFh->BlockIDToMap(nuLOWORD(j), nuLOWORD(i), &blkOut) )
			{
				blockID = i * XBLOCKNUM + j;
				for( nuBYTE k = 0; k < blkOut.nMapFixed; ++k )
				{
					bReadNew = nuTRUE;
					pBlockData = m_pDwDataList[ blkOut.pBlkFixData[k].nMapIdx ].pBlockList;
					while( pBlockData != NULL )
					{
						if( pBlockData->nBlockID == blockID )
						{
							if( ColRdwBlock(pBlockData->nRdwMassIdx, &roadBlk) )
							{
								if( SeekBlkForRoads(bSetMarket, 
									pSeekRdsIn, 
										pSeekRdsOut, 
										&roadBlk,
										blockID, 
										blkOut.pBlkFixData[k].nMapIdx, 
										pBlockData->nBlockIdx, 
										pBlockData->nBlockID ) )
									{
										bFind = nuTRUE;
									}
							}
							bReadNew = nuFALSE;
							break;
						}
						pBlockData = pBlockData->pNext;
					}
					if( bReadNew )
					{
						pBlockList[nBlockCount].nDwIdx	= blkOut.pBlkFixData[k].nMapIdx;
						pBlockList[nBlockCount].nBlkIdx	= blkOut.pBlkFixData[k].nBlkIdx;
						pBlockList[nBlockCount].nBlkID	= blockID;
						++nBlockCount;
						if( nBlockCount == MAX_BLOCKSNODE_LIST )
						{
							break;
						}
					}//need to read Block;
				}
			}
			/*2009.06.16
			blockID = i * XBLOCKNUM + j;
			for (nuWORD k = 0 ; k < m_nDwCount; k++ )
			{
				if( !m_fileFB.BlockIDInMap(blockID, k) )
				{
					continue;
				}
				pBlockList[nBlockCount].nDwIdx	= k;
				bReadNew = nuTRUE;
				pBlockData = m_pDwDataList[k].pBlockList;
				while( pBlockData != NULL )
				{
					if( pBlockData->nBlockID == blockID )
					{
						if( ColRdwBlock(pBlockData->nRdwMassIdx, &roadBlk) )
						{
							if( SeekBlkForRoads( pSeekRdsIn, 
												 pSeekRdsOut, 
												 &roadBlk,
												 blockID, 
												 k, 
												 pBlockData->nBlockIdx, 
												 pBlockData->nBlockID ) )
							{
								bFind = nuTRUE;
							}
						}
						bReadNew = nuFALSE;
						break;
					}
					pBlockData = pBlockData->pNext;
				}
				if( bReadNew )
				{
					pBlockList[nBlockCount].nBlkID	= blockID;
					++nBlockCount;
				}//need to read Block;
			}
			*///2009.06.16
		}
		if( nBlockCount == MAX_BLOCKSNODE_LIST )
		{
			break;
		}
	}
	if( nBlockCount > 0 )
	{
		ReadRdwBlocks(pBlockList, nBlockCount);
		//
		for(nuWORD i = 0; i < nBlockCount; i++ )
		{
			if( ColRdwBlock(pBlockList[i].nMemMassIdx, &roadBlk) )
			{
				if( SeekBlkForRoads(bSetMarket, 
					pSeekRdsIn, 
						pSeekRdsOut, 
						&roadBlk,
						pBlockList[i].nBlkID, 
						pBlockList[i].nDwIdx, 
						pBlockList[i].nBlkIdx, 
						pBlockList[i].nBlkID ) )
					{
						bFind = nuTRUE;
					}
				}
			g_pFsMmApi->MM_RelDataMemMass(&pBlockList[i].nMemMassIdx);
		}
	}
	return bFind;
}

nuBOOL CBlockDw::SeekBlkForRoads(nuBOOL bNameMapping, PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut,
							   PDWROADBLK pRoadBlk, nuLONG nBlockID, nuWORD nDwIdx, 
							   nuDWORD nBlkIdx, nuDWORD nBlkID )
{
	nuroRECT rtBlock = {0}, rtRoad = {0};
	nuroPOINT point = {0}, pt1= {0}, pt2 = {0}, pt3 = {0}, ptFixed = {0};
	nuLONG dx = 0, dy = 0, tmpAngle = 0, tmpDis = 0;
	nuBlockIDtoRect(nBlockID, &rtBlock);
	point.x = pSeekRdsIn->point.x - rtBlock.left;
	point.y = pSeekRdsIn->point.y - rtBlock.top;
	nuBOOL bFind = nuFALSE;
	nuINT nRoadNameAddr = -2;
	nuLONG nDisCover = 0;
	nuDWORD nDiffCarAngle=0;

	for( nuDWORD i = 0; i < pRoadBlk->pDwRoadHead->nRoadCount; i++ )
	{
		//Find the distance to cover
		if( nRoadNameAddr != pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nRoadNameAddr )
		{
			nRoadNameAddr = pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nRoadNameAddr;
			nDisCover = pSeekRdsIn->nSeekDis;
			for(nuBYTE ni = 0; ni < pSeekRdsOut->nCountSeeked; ++ni)
			{
				//if( pSeekRdsOut->roadsList[ni].nRoadClass == nNowRoadType && 
				//	pSeekRdsOut->roadsList[ni].nDis < nDisCover )
				
				if(bNameMapping && pSeekRdsOut->roadsList[ni].nNameAddr	== nRoadNameAddr)
				{
					nDisCover = pSeekRdsOut->roadsList[ni].nDis;
				}
				else if( pSeekRdsOut->roadsList[ni].nRoadIdx	== i &&
					pSeekRdsOut->roadsList[ni].nBlkIdx	== nBlkIdx )
				{
					nDisCover = pSeekRdsOut->roadsList[ni].nDis;
				}
			}
		}
		rtRoad.left		= pRoadBlk->pRoadRect[i].left - nDisCover;
		rtRoad.top		= pRoadBlk->pRoadRect[i].top - nDisCover;
		rtRoad.right	= pRoadBlk->pRoadRect[i].right + nDisCover;
		rtRoad.bottom	= pRoadBlk->pRoadRect[i].bottom + nDisCover;
		if( !nuPtInRect(point, rtRoad) )
		{
			continue;
		}
		pt2.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx ].x;
		pt2.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx ].y;
		for( nuDWORD j = 1; j < pRoadBlk->pDwRoadData[i].nVertexCount; j++ )
		{
			pt1 = pt2;
			pt2.x = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx + j ].x;
			pt2.y = pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx + j ].y;
			rtRoad.left		= NURO_MIN(pt1.x, pt2.x) - nDisCover;
			rtRoad.right	= NURO_MAX(pt1.x, pt2.x) + nDisCover;
			rtRoad.top		= NURO_MIN(pt1.y, pt2.y) - nDisCover;
			rtRoad.bottom	= NURO_MAX(pt1.y, pt2.y) + nDisCover;
			
			if( !nuPtInRect(point, rtRoad) || (NURO_ABS(pt1.x - pt2.x) < 2 && NURO_ABS(pt1.y - pt2.y) < 2))
			{
				continue;
			}
			nuBOOL bAnti = nuFALSE;
			if( pSeekRdsIn->nAngle >= 0 )
			{
				dx = pt2.x - pt1.x;
				dy = pt2.y - pt1.y;
				tmpAngle = (nuLONG)(nuAtan2(dy, dx) * 180/PI);
				tmpAngle = (pSeekRdsIn->nAngle - tmpAngle) % 360;
				tmpAngle = NURO_ABS(tmpAngle);
				if( tmpAngle > 180 )
				{
					tmpAngle = 360 - tmpAngle;
				}
				if( pRoadBlk->pDwRoadData[i].nOneWay == 1 )
				{
					if( tmpAngle > ANGLE_FIXTOROAD )
					{
						continue;
					}
				}
				else if( pRoadBlk->pDwRoadData[i].nOneWay == 2 )
				{
					if( tmpAngle < 180 - ANGLE_FIXTOROAD )
					{
						continue;
					}
					bAnti = nuTRUE;
					dx = -dx;
					dy = -dy;
				}
				else
				{
					if( tmpAngle > ANGLE_FIXTOROAD && tmpAngle < 180 - ANGLE_FIXTOROAD )
					{
						continue;
					}
					else if( tmpAngle >= 180 - ANGLE_FIXTOROAD )
					{
						bAnti = nuTRUE;
						dx = -dx;
						dy = -dy;
					}
				}
			}
			if( nuPtFixToLineSeg(point, pt1, pt2, &ptFixed, &tmpDis) && tmpDis <= nDisCover )
			{
				nuBYTE nSIdx;
				nuBOOL bFindRoad = nuFALSE;
				for(nSIdx = 0; nSIdx < pSeekRdsOut->nCountSeeked; ++nSIdx)
				{
					if(bNameMapping && pSeekRdsOut->roadsList[nSIdx].nNameAddr == 
						pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nRoadNameAddr)
					{
						bFindRoad = nuTRUE;
						break;
					}
					else if( pSeekRdsOut->roadsList[nSIdx].nRoadIdx	== i &&
						pSeekRdsOut->roadsList[nSIdx].nBlkIdx	== nBlkIdx )
				{
						bFindRoad = nuTRUE;
						break;
				}
			}
				if(bFindRoad && tmpDis > pSeekRdsOut->roadsList[nSIdx].nDis)
					{
						continue;
					}
				if( nSIdx == pSeekRdsOut->nCountSeeked )
				{
					if( pSeekRdsOut->nCountSeeked == SEEK_ROADS_MAX_COUNT )
					{
						break;
					}
					++pSeekRdsOut->nCountSeeked;
				}
				//nDisCover = tmpDis;
				pSeekRdsOut->roadsList[nSIdx].nDwIdx		= nDwIdx;
				pSeekRdsOut->roadsList[nSIdx].nBlockID		= nBlockID;
				pSeekRdsOut->roadsList[nSIdx].nBlkIdx		= nBlkIdx;
				pSeekRdsOut->roadsList[nSIdx].nDis			= (nuSHORT)tmpDis;
				pSeekRdsOut->roadsList[nSIdx].nRoadIdx		= i;
				pSeekRdsOut->roadsList[nSIdx].nNameAddr		= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nRoadNameAddr;
				pSeekRdsOut->roadsList[nSIdx].nRoadClass	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nRoadType;
				pSeekRdsOut->roadsList[nSIdx].nCityID		= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nCityID;
				pSeekRdsOut->roadsList[nSIdx].nTownID		= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nTownID;
				pSeekRdsOut->roadsList[nSIdx].ptMapped.x 	= ptFixed.x + rtBlock.left;
				pSeekRdsOut->roadsList[nSIdx].ptMapped.y	= ptFixed.y + rtBlock.top;
				pSeekRdsOut->roadsList[nSIdx].nPtIdx		= (nuWORD)(j - 1);
				pSeekRdsOut->roadsList[nSIdx].nLimitSpeed	= pRoadBlk->pDwRoadCommon[ pRoadBlk->pDwRoadData[i].nRoadCommonIdx ].nSpeedLimit;
				pSeekRdsOut->roadsList[nSIdx].nRdDx			= nuLOWORD(dx);
				pSeekRdsOut->roadsList[nSIdx].nRdDy			= nuLOWORD(dy);
				pSeekRdsOut->roadsList[nSIdx].bGPSState		= pRoadBlk->pDwRoadData[i].bGPSState;
			#ifdef ZENRIN
				pSeekRdsOut->roadsList[nSIdx].nSlope		= pRoadBlk->pDwRoadData[i].nSlope;
				pSeekRdsOut->roadsList[nSIdx].nLevel		= pRoadBlk->pDwRoadData[i].nLevel;
			#endif
				
				pSeekRdsOut->roadsList[nSIdx].ptNode1.x		= pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx ].x+rtBlock.left;
				pSeekRdsOut->roadsList[nSIdx].ptNode1.y		= pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx ].y+rtBlock.top;
				pSeekRdsOut->roadsList[nSIdx].ptNode2.x		= pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx + pRoadBlk->pDwRoadData[i].nVertexCount-1].x+rtBlock.left;
				pSeekRdsOut->roadsList[nSIdx].ptNode2.y		= pRoadBlk->pSPointCoor[ pRoadBlk->pDwRoadData[i].nVertexAddrIdx + pRoadBlk->pDwRoadData[i].nVertexCount-1].y+rtBlock.top;

				//if(pSeekRdsOut->roadsList[nSIdx].nRoadClass<8) pSeekRdsOut->roadsList[nSIdx].nDis+=10000; //Louis 20120330 用等級來排除行駛間可拉回道路 
				bFind = nuTRUE;
				if( bAnti )
				{
					pSeekRdsOut->roadsList[nSIdx].nOneWay   = 2;
					pSeekRdsOut->roadsList[nSIdx].nRoadAngle  = 180 - tmpAngle;
				}
				else
				{
					pSeekRdsOut->roadsList[nSIdx].nOneWay   = 1;
					pSeekRdsOut->roadsList[nSIdx].nRoadAngle  = tmpAngle;
				}
			}
		}
	}
	
	for(nuWORD n = 0; n < pSeekRdsOut->nCountSeeked; ++n)
	{
		if(pSeekRdsOut->roadsList[n].nBlkIdx != nBlkIdx) continue;

		if( pSeekRdsOut->roadsList[n].nOneWay == 1 )
		{
			nuINT nVertexIdx = pRoadBlk->pDwRoadData[ pSeekRdsOut->roadsList[n].nRoadIdx ].nVertexAddrIdx;
			nuINT nVertexcount = pRoadBlk->pDwRoadData[ pSeekRdsOut->roadsList[n].nRoadIdx ].nVertexCount;
			pt1.x = pRoadBlk->pSPointCoor[ nVertexIdx + pSeekRdsOut->roadsList[n].nPtIdx + 1 ].x;
			pt1.y = pRoadBlk->pSPointCoor[ nVertexIdx + pSeekRdsOut->roadsList[n].nPtIdx + 1 ].y;
			nuINT dx = pt1.x - (pSeekRdsOut->roadsList[n].ptMapped.x - rtBlock.left);
			nuINT dy = pt1.y - (pSeekRdsOut->roadsList[n].ptMapped.y - rtBlock.top);
			pSeekRdsOut->roadsList[n].nDisToNextCoor = (nuWORD)nuSqrt(dx*dx + dy*dy);
			pSeekRdsOut->roadsList[n].ptNextCoor.x = pRoadBlk->pSPointCoor[ nVertexIdx + nVertexcount - 1 ].x + rtBlock.left;
			pSeekRdsOut->roadsList[n].ptNextCoor.y = pRoadBlk->pSPointCoor[ nVertexIdx + nVertexcount - 1 ].y + rtBlock.top;
			for(nuWORD m = pSeekRdsOut->roadsList[n].nPtIdx+2; m < nVertexcount; ++m )
			{
				dx = pRoadBlk->pSPointCoor[nVertexIdx + m].x - pRoadBlk->pSPointCoor[nVertexIdx + m -1].x;
				dy = pRoadBlk->pSPointCoor[nVertexIdx + m].y - pRoadBlk->pSPointCoor[nVertexIdx + m -1].y;
				pSeekRdsOut->roadsList[n].nDisToNextCoor += (nuWORD)nuSqrt(dx*dx + dy*dy);
			}
		}
		else if( pSeekRdsOut->roadsList[n].nOneWay == 2 )
		{
			nuINT nVertexIdx = pRoadBlk->pDwRoadData[ pSeekRdsOut->roadsList[n].nRoadIdx ].nVertexAddrIdx;
			nuINT nVertexcount = pRoadBlk->pDwRoadData[ pSeekRdsOut->roadsList[n].nRoadIdx ].nVertexCount;
			pt1.x = pRoadBlk->pSPointCoor[ nVertexIdx + pSeekRdsOut->roadsList[n].nPtIdx].x;
			pt1.y = pRoadBlk->pSPointCoor[ nVertexIdx + pSeekRdsOut->roadsList[n].nPtIdx].y;
			nuINT dx = pt1.x - (pSeekRdsOut->roadsList[n].ptMapped.x - rtBlock.left);
			nuINT dy = pt1.y - (pSeekRdsOut->roadsList[n].ptMapped.y - rtBlock.top);
			pSeekRdsOut->roadsList[n].nDisToNextCoor = (nuWORD)nuSqrt(dx*dx + dy*dy);
			pSeekRdsOut->roadsList[n].ptNextCoor.x = pRoadBlk->pSPointCoor[ nVertexIdx ].x + rtBlock.left;
			pSeekRdsOut->roadsList[n].ptNextCoor.y = pRoadBlk->pSPointCoor[ nVertexIdx ].y + rtBlock.top;
			for(nuWORD m = 0; m < pSeekRdsOut->roadsList[n].nPtIdx; ++m )
			{
				dx = pRoadBlk->pSPointCoor[nVertexIdx + m].x - pRoadBlk->pSPointCoor[nVertexIdx + m + 1].x;
				dy = pRoadBlk->pSPointCoor[nVertexIdx + m].y - pRoadBlk->pSPointCoor[nVertexIdx + m + 1].y;
				pSeekRdsOut->roadsList[n].nDisToNextCoor += (nuWORD)nuSqrt(dx*dx + dy*dy);
			}
		}
	}
	return bFind;
}
