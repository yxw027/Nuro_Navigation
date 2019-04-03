#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFilePzyY : public CFileMapBaseZ  
{
public:
	CMapFilePzyY();
	virtual ~CMapFilePzyY();

	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();
	
	nuUINT GetLength();
	nuBOOL ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len);
	nuUINT GetPzyInfo_CITY(nuWORD nCityID, nuBYTE* pBuffer, nuUINT nMaxLen);
private:
	nuCFile file;
	nuWORD totalcount;
};


