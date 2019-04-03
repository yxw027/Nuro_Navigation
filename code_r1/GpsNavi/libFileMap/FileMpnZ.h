// FileMpnZ.h: interface for the CFileMpnZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMPNZ_H__11243149_CBD1_48FA_BEF4_DC7B65C7F6CD__INCLUDED_)
#define AFX_FILEMPNZ_H__11243149_CBD1_48FA_BEF4_DC7B65C7F6CD__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"

#define MAP_MPN_PATH	nuTEXT("Map\\Major\\Map.mpn")

#define MPN_DATA_COUNT				4
#define MPN_DATA_MAPID				0x0001
#define MPN_DATA_FILENAME			0x0002//
#define MPN_DATA_FRIENDNAME			0x0004
#define MPN_DATA_XY					0x0008

class CFileMpnZ
{
public:
	CFileMpnZ();
	virtual ~CFileMpnZ();

	//Old data struts, not been used now.
//	static nuUINT GetMpnBuff(nuTCHAR *tsPath, nuPVOID pBuff, nuUINT nBufLen, nuUINT nDataMode);//return Bytes read
	static nuUINT GetMpnData(nuTCHAR *tsPath, nuPVOID pBuff, nuUINT& nBufLen);
};

#endif // !defined(AFX_FILEMPNZ_H__11243149_CBD1_48FA_BEF4_DC7B65C7F6CD__INCLUDED_)
