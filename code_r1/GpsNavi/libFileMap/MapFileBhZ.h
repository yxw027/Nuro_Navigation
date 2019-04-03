// MapFileBhZ.h: interface for the CMapFileBhZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILEBHZ_H__B98B2CB2_E007_4DFB_B9E7_9F0ACCE3A49E__INCLUDED_)
#define AFX_MAPFILEBHZ_H__B98B2CB2_E007_4DFB_B9E7_9F0ACCE3A49E__INCLUDED_

#include "FileMapBaseZ.h"

typedef struct tagBHADDRESS
{
	nuDWORD nPdwAddr;
	nuDWORD nRdwAddr;
	nuDWORD nBLdwAddr;
	nuDWORD nBAdwAddr;
}BHADDRESS, *PBHADDRESS;

/*
#define BLOCK_ADDR_MODE_PDW					0x01
#define BLOCK_ADDR_MODE_RDW					0x02
#define BLOCK_ADDR_MODE_BLDW				0x04
#define BLOCK_ADDR_MODE_BADW				0x08
*/

class CMapFileBhZ : public CFileMapBaseZ
{
public:
	CMapFileBhZ();
	virtual ~CMapFileBhZ();

	nuDWORD	GetBlockCount(nuWORD nMapIdx);
	nuBOOL	ReadBlockIDList(nuDWORD* pBlockID, nuDWORD nCount);
	nuBOOL	ReadBlockAddr(nuDWORD nBlockIdx, PBHADDRESS pBlockAddr, nuBYTE nMode = -1);
	nuBOOL	OpenFile();
	nuVOID	CloseFile();

protected:
	nuFILE*		m_pFile;
	nuDWORD		m_nBlkCount;
	nuWORD		m_nMapIdx;
};

#endif // !defined(AFX_MAPFILEBHZ_H__B98B2CB2_E007_4DFB_B9E7_9F0ACCE3A49E__INCLUDED_)
