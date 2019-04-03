#if !defined(AFX_MAPFILEMPNY_H__890FFF0C_9E04_4988_BD99_D08CBA098521__INCLUDED_)
#define AFX_MAPFILEMPNY_H__890FFF0C_9E04_4988_BD99_D08CBA098521__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"
#include "nuSearchDefine.h"

class CMapFileMpnY : public CFileMapBaseZ  
{
public:
	CMapFileMpnY();
	virtual ~CMapFileMpnY();
	nuBOOL Open();
	nuBOOL IsOpen();
	nuVOID Close();
	nuUINT GetMapNum() const;
	nuUINT GetMapName(nuUINT nStartID, nuUINT nNum, nuVOID* pBuffer, nuUINT nBufferLen);
private:
	nuCFile file;
	nuUINT maptotal;
};

#endif // !defined(AFX_MAPFILEMPNY_H__890FFF0C_9E04_4988_BD99_D08CBA098521__INCLUDED_)
