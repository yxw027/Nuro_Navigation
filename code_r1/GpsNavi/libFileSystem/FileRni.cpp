
#include "FileRni.h"
#include "libFileSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CFileRni::CFileRni()
{
	m_nScaleValue = 0;
}

CFileRni::~CFileRni()
{
	Free();
}

nuBOOL CFileRni::Initialize()
{
	return nuTRUE;
}

nuVOID CFileRni::Free()
{

}

nuBOOL CFileRni::GetRoadIconInfo(nuWORD nDwIdx, nuLONG nScaleValue, nuLONG nBGLStartScale, PRNEXTEND pRnEx, nuWORD *RoadIconInfo)
{
	StrRniHeader RniHeader = {0};
	nuTCHAR RniFilePath[512] = {0};
	
	nuFILE *pFile = NULL;
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	if( !LibFS_GetFilePath(FILE_NAME_PATH, sFile, NURO_MAX_PATH) )
	{
		return NULL;
	}
	//if( m_nScaleValue != nScaleValue )
	{
		m_nScaleValue = nScaleValue;
		if( m_nScaleValue > BGL_SCALE_50K_START )
		{
			nuTcscat(sFile, _BGL_50K_RNI_);
		}
		else if( m_nScaleValue >= nBGLStartScale )
		{
			nuTcscat(sFile, _BGL_5K_RNI_);
		}
		else
		{
			nuMemset(sFile, 0, NURO_MAX_PATH);
			nuTcscpy(sFile, nuTEXT(".RNI"));
			if( !LibFS_FindFileWholePath(nDwIdx, sFile, NURO_MAX_PATH) )
			{
				return nuFALSE;
			}
		}
	}
	pFile = nuTfopen(sFile, NURO_FS_RB);
	if(pFile == NULL)
	{
		return nuFALSE;
	}
	if(1 != nuFread(&RniHeader, sizeof(nuLONG), 1, pFile))
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( 0 != nuFseek(pFile, pRnEx->nIconNum*RniHeader.RniLen*2 + sizeof(RniHeader), NURO_SEEK_SET) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( RniHeader.RniLen != nuFread(RoadIconInfo, sizeof(nuWORD), RniHeader.RniLen, pFile) )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( 0 == RoadIconInfo[0] )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFclose(pFile);
	return nuTRUE;
}
