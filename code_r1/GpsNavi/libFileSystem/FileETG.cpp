#include "FileETG.h"
#include "NuroModuleApiStruct.h"
#include "libFileSystem.h"

extern PMEMMGRAPI	g_pFsMmApi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileETG::CFileETG()
{

}

CFileETG::~CFileETG()
{
	Free();
}

nuBOOL CFileETG::Initialize()
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuLONG nMapIdx = 0;	
	m_lTotoalCount = 0;
	m_lIndex	   = -1;
	m_lMappingRoadIdx = -1;
	m_lMappingBlockIdx = -1;
	m_pptETG = NULL;
	m_pFile = NULL;
	nuTcscpy(tsFile, nuTEXT(".ETG"));
	if( !LibFS_FindFileWholePath(nMapIdx, tsFile, NURO_MAX_PATH) )
	{
		return nuFALSE;
	}
	m_pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( m_pFile == NULL )
	{
		return nuFALSE;
	}
	if(1 != nuFread(&m_lTotoalCount, sizeof(nuLONG), 1,m_pFile))
	{
		return nuFALSE;
	}
	if(m_lTotoalCount == 0 || NULL != nuFseek(m_pFile, 100, NURO_SEEK_SET))
	{
		return nuFALSE;
	}
	m_pptETG = (NUROPOINT*)g_pFsMmApi->MM_GetStaticMemMass(sizeof(NUROPOINT) * m_lTotoalCount);
	if(m_pptETG == NULL)
	{
		return nuFALSE;
	}
	if(m_lTotoalCount != nuFread(m_pptETG, sizeof(NUROPOINT), m_lTotoalCount,m_pFile))
	{
		return nuFALSE;
	}
	if(NULL != m_pFile)
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
	m_bSendEtag = nuFALSE;
	return nuTRUE;
}
nuVOID CFileETG::Free()
{
	if(NULL != m_pFile)
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}
nuBOOL CFileETG::SearchETG(nuLONG lBlockIdx, nuLONG lRoadIdx, NUROPOINT ptRoad, NUROPOINT ptCar, NUROPOINT &ptEtagPoint)
{
	nuLONG lMax = m_lTotoalCount - 1, lMin = 0, lMid = 0;
	nuLONG lIndex = -1;
	nuLONG ldx = 0, ldy = 0, Distance = 0;
	
	/*while (lMin <= lMax)
    {
        lMid = (lMin + lMax) / 2;
		if (ptRoad.x > m_pptETG[lMid].x)
        {
            lMin = lMid + 1;
        }
        else if (ptRoad.x < m_pptETG[lMid].x)
        {
            lMax = lMid - 1;
        }
		else if(ptRoad.x == m_pptETG[lMid].x)
		{
			if (ptRoad.y > m_pptETG[lMid].y)
			{
				lMin = lMid + 1;
			}
			else if (ptRoad.y < m_pptETG[lMid].y)
			{
				lMax = lMid - 1;
			}
			else if(ptRoad.y == m_pptETG[lMid].y)
			{
				lIndex = lMid;
				break;
			}
		}
    }*/
	for(nuINT i = 0; i < m_lTotoalCount; i++)
	{
		if(NURO_ABS(ptRoad.y - m_pptETG[i].y) < 3 && NURO_ABS(ptRoad.x - m_pptETG[i].x) < 3)
		{
			lIndex = i;
		}
	}
	if(lIndex > -1)
	{
		m_lIndex = lIndex;
	}
	else
	{
		if(m_lIndex > -1)
		{
			m_lIndex = -1;
			return nuTRUE;
		}
		m_lIndex = -1;
	}
	return nuFALSE;
}
