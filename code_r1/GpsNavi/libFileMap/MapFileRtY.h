#if !defined(AFX_MAPFILERTY_H__543E5FF2_695A_4332_A635_AADD57933AFF__INCLUDED_)
#define AFX_MAPFILERTY_H__543E5FF2_695A_4332_A635_AADD57933AFF__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"
#include "nuSearchDefine.h"

class CMapFileRtY : public CFileMapBaseZ  
{
public:
	CMapFileRtY();
	virtual ~CMapFileRtY();
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();

	nuWORD GetRtNum();
	
	nuUINT GetRtIdxInfo(nuUINT rtIdx, nuVOID* pBuffer, nuUINT nMaxLen);
	nuBOOL GetRtA1Info(nuUINT addr, nuUINT a1idx, SEARCH_RTINFO_A1 *pa1);
	nuBOOL GetRtA2Info(nuUINT addr, nuUINT a2idx, SEARCH_RTINFO_A2 *pa2);
private:
	nuUINT GetRtAddr(nuWORD idx);
	nuBOOL GetRtBasePos(nuWORD idx, SEARCH_RTINFO_M *prtim);
	nuCFile file;
	nuWORD rtcount;
};

#endif // !defined(AFX_MAPFILERTY_H__543E5FF2_695A_4332_A635_AADD57933AFF__INCLUDED_)
