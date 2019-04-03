// FileCb.cpp: implementation of the CFileCb class.
//
//////////////////////////////////////////////////////////////////////

#include "FileCb.h"
#include "NuroModuleApiStruct.h"
#include "libFileSystem.h"

extern PMEMMGRAPI	g_pFsMmApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileCb::CFileCb()
{

}

CFileCb::~CFileCb()
{

}

nuBOOL CFileCb::Initialize()
{
	return nuTRUE;
}

nuVOID CFileCb::Free()
{
	
}

nuBOOL CFileCb::ReadCityTownName(nuLONG nMapIdx, nuDWORD nCityTownCode, nuWCHAR *wsName, nuWORD nWsNum)
{
	if( nMapIdx < 0 || wsName == NULL /*|| nWsNum < TOWN_NAME_NUM + CITY_NAME_NUM */)
	{
		return nuFALSE;
	}
	nuBOOL bFind = nuFALSE;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, nuTEXT(".cb"));
	if( !LibFS_FindFileWholePath(nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	nuFILE *pfile = nuTfopen(tsFile, NURO_FS_RB);
	if( pfile == NULL )
	{
		return nuFALSE;
	}
	CB_HEADER cbHeader = {0};
	if( nuFseek(pfile, 0, NURO_SEEK_SET) != 0 )
	{
		nuFclose(pfile);
		return nuFALSE;
	}
	if( nuFread(&cbHeader, sizeof(CB_HEADER), 1, pfile ) != 1 )
	{
		nuFclose(pfile);
		return nuFALSE;
	}
	/*
	if( nuFseek(pfile, sizeof(NURO_CHAR)*CITY_NAME_NUM*cbHeader.nCityCount, NURO_SEEK_CUR) != 0 )
	{
		nuFclose(pfile);
		return nuFALSE;
	}
	*/
	nuWORD idx = -1;
	nuLONG nSize = 4*cbHeader.nTownCount;
	nuDWORD *pAddr = (nuDWORD*)g_pFsMmApi->MM_GetDataMemMass(nSize, &idx);//new nuDWORD[cbHeader.nTownCount];
	if( pAddr == NULL )
	{
		nuFclose(pfile);
		return nuFALSE;
	}
	
	CB_TOWNINFO cbTownInfo = {0};
	if( nuFseek(pfile, -nSize, NURO_SEEK_END) == 0 && 
		nuFread(pAddr, nSize, 1, pfile) == 1 )//二分法找City和Town
	{
		nuLONG nF = 0, nT = cbHeader.nTownCount;
		nuLONG i = nT/2;
		nuWORD nCity = nuHIWORD(nCityTownCode);
		nuWORD nTown = nuLOWORD(nCityTownCode);
		while( nF <= nT )
		{
			if( nuFseek(pfile, pAddr[i], NURO_SEEK_SET) != 0 ||
				nuFread(&cbTownInfo, 4, 1, pfile) != 1)
			{
				break;
			}
			if( cbTownInfo.nCityCode == nCity &&
				cbTownInfo.nTownCode == nTown )
			{
				NURO_CHAR wsCity[CITY_NAME_NUM] = {0};
				if( nuFread(&cbTownInfo.wsTownName, sizeof(NURO_CHAR)*TOWN_NAME_NUM, 1, pfile) != 1 ||
					nuFseek(pfile, sizeof(CB_HEADER)+sizeof(NURO_CHAR)*CITY_NAME_NUM*nCity, NURO_SEEK_SET) != 0 ||
					nuFread(wsCity, sizeof(NURO_CHAR)*CITY_NAME_NUM, 1, pfile) != 1 )
				{
					break;
				}
				nuWcsncpy(wsName, wsCity, nWsNum/2);
				wsName[nWsNum/2 - 1] = 0;
				nuWcsncpy(&wsName[nWsNum/2], cbTownInfo.wsTownName, nWsNum/2);
				wsName[nWsNum -1 ] = 0;
				bFind = nuTRUE;
				break;
			}//Find the city and town
			if( nF == nT )
			{
				break;
			}
			if( nCity > cbTownInfo.nCityCode ||
				(nCity == cbTownInfo.nCityCode && nTown > cbTownInfo.nTownCode) )
			{
				if( nF == i )
				{
					nF ++;
				}
				else
				{
					nF = i;
				}
			}
			else
			{
				nT = i;
			}
			i = (nF + nT)/2;
		}
	}
//	delete []pAddr;
	g_pFsMmApi->MM_RelDataMemMass(&idx);
	nuFclose(pfile);
	return bFind;
}
