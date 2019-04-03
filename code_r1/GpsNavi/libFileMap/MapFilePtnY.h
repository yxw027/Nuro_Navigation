#if !defined(AFX_MAPFILEPTNY_H__A253DFA3_8854_4A8B_937F_C7D0E45F415D__INCLUDED_)
#define AFX_MAPFILEPTNY_H__A253DFA3_8854_4A8B_937F_C7D0E45F415D__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

struct NURO_PT1INFO
{
	nuWCHAR Type1Name[68];
	nuBYTE Type2Count;
	nuDWORD ptn2Addr;
};
struct NURO_PT2INFO
{
	nuWCHAR Type2Name[68];
};
class CMapFilePtnY : public CFileMapBaseZ  
{
public:
	CMapFilePtnY();
	virtual ~CMapFilePtnY();
	nuBOOL Open(nuLONG mapid);
	nuBOOL IsOpen();
	nuVOID Close();

	nuBYTE	GetTP1Num();
	nuUINT	GetTP1Name(nuWORD idx, nuBYTE* pBuffer, nuUINT nMaxLen);
	//return count
	nuUINT	GetTP1NameAll(nuBYTE* pBuffer, nuUINT nMaxLen);
	nuUINT	GetTP2NameAll(nuWORD TP1Idx, nuBYTE* pBuffer, nuUINT nMaxLen);
	
private:
	nuCFile file;
	nuBYTE	m_nTp1Count;
};

#endif // !defined(AFX_MAPFILEPTNY_H__A253DFA3_8854_4A8B_937F_C7D0E45F415D__INCLUDED_)
