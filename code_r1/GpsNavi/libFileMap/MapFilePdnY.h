#if !defined(AFX_MAPFILEPDNY_H__A4B6CFB2_587B_4968_B231_6B807493F0AD__INCLUDED_)
#define AFX_MAPFILEPDNY_H__A4B6CFB2_587B_4968_B231_6B807493F0AD__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"
#include "nuSearchDefine.h"

class CMapFilePdnY : public CFileMapBaseZ  
{
public:
	CMapFilePdnY();
	virtual ~CMapFilePdnY();
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();
	nuUINT GetCityNum();
	nuUINT GetCityDoorInfoStartAddr(nuWORD nCityID);
	nuUINT GetDoorInfo(nuWORD nCityID, nuVOID* pBuffer, nuUINT nMaxLen);
	nuBOOL GetData(nuUINT startaddr, nuVOID *pBuffer, nuUINT len);
    nuSIZE ReadData(nuUINT startaddr, nuVOID* pBuff, nuSIZE size, nuSIZE count);

private:
	nuCFile file;
	nuUINT pdnNum;
	nuUINT cityNum;
};

#endif // !defined(AFX_MAPFILEPDNY_H__A4B6CFB2_587B_4968_B231_6B807493F0AD__INCLUDED_)
