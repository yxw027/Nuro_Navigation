// FileSystemBaseZ.h: interface for the CFileSystemBaseZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILESYSTEMBASEZ_H__A442FD70_41AA_4A9C_B2DA_6519A32C172F__INCLUDED_)
#define AFX_FILESYSTEMBASEZ_H__A442FD70_41AA_4A9C_B2DA_6519A32C172F__INCLUDED_

#include "NuroModuleApiStruct.h"
#include "NuroClasses.h"

#define CONST_STR_LIMIT						14
#define CONST_STR_DIS_LIMIT					15

class CFileSystemBaseZ 
{
public:
	CFileSystemBaseZ();
	virtual ~CFileSystemBaseZ();
	
	static nuBOOL Initialize(PAPISTRUCTADDR pApiAddr);
	static nuVOID Free();

	static nuUINT GetConstStr(nuINT nIdx, nuWCHAR *wsBuff, nuWORD nWsNum);
	
protected:
	static PFILESYSAPI		m_pFsApi;
	static PMEMMGRAPI		m_pMmApi;
};

#endif // !defined(AFX_FILESYSTEMBASEZ_H__A442FD70_41AA_4A9C_B2DA_6519A32C172F__INCLUDED_)
