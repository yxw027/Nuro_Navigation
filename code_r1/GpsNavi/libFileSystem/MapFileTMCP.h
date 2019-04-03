#if !defined(AFX_MapFileTMCP_H__BF925B01_6688_4476_AFCB_F23AC5F1388B__INCLUDED_)
#define AFX_MapFileTMCP_H__BF925B01_6688_4476_AFCB_F23AC5F1388B__INCLUDED_

#include "FileMapBaseZ.h"
#include "nuSearchDefine.h"

class CMapFileTMCP : public CFileMapBaseZ
{
public:
	CMapFileTMCP();
	virtual ~CMapFileTMCP();
	nuBOOL  Open(long nMapID);
	nuBOOL  IsOpen();
	nuVOID  Close();
	nuUINT  GetLength();
	nuBOOL  ReadData(nuUINT nDataAddr, nuVOID *pBuffer, nuUINT nLen);

private:
	nuCFile file;

	typedef struct tag_TMC_Header
	{
		nuDWORD DataCnt;
		nuDWORD VertexDataAddr;
		nuDWORD TrafficDataAddr;
		nuWORD TrafficCnt;
		nuWORD Reserve;
	} TMC_Header;

    TMC_Header tmcHeader;
};

#endif // !defined(AFX_MapFileTMCP_H__BF925B01_6688_4476_AFCB_F23AC5F1388B__INCLUDED_)
