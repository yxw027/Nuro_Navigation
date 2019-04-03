// FileNameMgr.h: interface for the CFileNameMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILENAMEMGR_H__5CD24538_BA4C_4BEC_8619_8BA5EC95F046__INCLUDED_)
#define AFX_FILENAMEMGR_H__5CD24538_BA4C_4BEC_8619_8BA5EC95F046__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"

class CFileNameMgr  
{
public:
	CFileNameMgr();
	virtual ~CFileNameMgr();

	nuBOOL  Initialize();
	nuVOID  Free();
	nuBOOL  FindFileWholePath(nuLONG nMapIdx, nuTCHAR* sWholePath, nuWORD num);
	const nuTCHAR *GetAppPath() 
	{ 
		return m_sAppPath; 
	}
	nuBOOL  GetFilePath(nuWORD nFileType, nuTCHAR *sWholePath, nuWORD len, nuINT nLanguage = -1);
	nuWORD  GetMapCount() 
	{ 
		return m_nMapCount; 
	}
	nuLONG  MapIDIndex(nuLONG nID, nuBYTE nType);
	nuBOOL  ReadMapFriendName(nuWORD nMapIdx, nuWCHAR *pWsName, nuWORD nLen);
	nuPVOID GetEffectiveMapData() 
	{ 
		return m_pMapNameInfo; 
	}

protected:
//	nuBOOL SearchMapFolder(const nuTCHAR* sAppPath);
	nuBOOL ReadMapPath();

protected:
	nuTCHAR*			m_sAppPath;
	nuWORD				m_nMapCount;
	PMAPNAMEINFO		m_pMapNameInfo;
};

#endif // !defined(AFX_FILENAMEMGR_H__5CD24538_BA4C_4BEC_8619_8BA5EC95F046__INCLUDED_)
