// FileMapBaseZ.h: interface for the CFileMapBaseZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMAPBASEZ_H__15B529B5_B8F3_413E_B66E_B48F03576227__INCLUDED_)
#define AFX_FILEMAPBASEZ_H__15B529B5_B8F3_413E_B66E_B48F03576227__INCLUDED_

#include "NuroModuleApiStruct.h"
#include "NuroClasses.h"
#include "nuSearchDefine.h"

#define FCLOSE_RETURN(pFile, a)	nuFclose(pFile); return (a)

class CFileMapBaseZ  
{
public:
	CFileMapBaseZ();
	virtual ~CFileMapBaseZ();
	
	static nuBOOL InitData(PAPISTRUCTADDR pApiAddr);
	static nuVOID FreeData();

	nuBOOL Initialize();
	nuVOID Free();

protected:
	static PFILESYSAPI			m_pFsApi;
	static PMEMMGRAPI			m_pMmApi;
	static PMEMMGRAPI_SH		m_pMm_SHApi;
};

#endif // !defined(AFX_FILEMAPBASEZ_H__15B529B5_B8F3_413E_B66E_B48F03576227__INCLUDED_)
