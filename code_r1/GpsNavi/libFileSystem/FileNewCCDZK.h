// FileNewCCDZK.h: interface for the CFileNewCCDZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILENEWCCDZK_H__CD9AC68C_9F0A_4A57_B039_65FE1154DF92__INCLUDED_)
#define AFX_FILENEWCCDZK_H__CD9AC68C_9F0A_4A57_B039_65FE1154DF92__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroMapFormat.h"
#include "FileSystemBaseZ.h"

class CFileNewCCDZK : public CFileMapBaseZ
{
public:
	CFileNewCCDZK();
	virtual ~CFileNewCCDZK();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuUINT			CheckCcd2(PCCDINFO pCcdInfo);
	PCCD2_DATA		GetCcd2CheckData(nuWORD nMapIdx, const nuroRECT& rtRect, nuINT* pBlkCount);
	nuUINT			FreeCcd2Data(nuINT nMode);
protected:
	nuVOID			RemovePlayed(nuroRECT* pRect);
	nuUINT			GetCcd2TypeName(nuBYTE nType, nuBYTE* pBuff, nuWORD nBuffLen);
	nuUINT			ColCcd2Name(nuBYTE nType, nuBYTE nSpeed, nuWCHAR* pwsName, nuWORD nCount);

public:
	nuVOID AddLimitSpeed(nuINT nSpeed, nuWORD nType, nuWCHAR *wsBuff, nuINT nWsNum);
	nuUINT GetEEyeTypeName(nuBYTE nType, nuPVOID pBuff, nuUINT nBuffLen);
	CCD2_DATA		m_pCcd2Checking[_MAX_CCD_BLOCKS_FOR_CHECKING];
//	CCD2_DATA		m_pCcd2Drawing[_MAX_CCD_BLOCKS_FOR_DRAWING];
	nuBYTE			m_nCheckingCount;
	CCD2_PLAYED		m_pCcd2Played[_CCD2_MAX_CHECK_COUNT];

protected:
	nuWORD			m_nMapIdx;
	nuFILE*			m_pFile;
	nuWCHAR			m_wsSpeedLimit[6];
	nuBYTE			m_nSpeedLmtCount;
	nuBYTE			m_nSpeedUnitCount;
	nuWCHAR			m_wsSpeedUnit[6];
};

#endif // !defined(AFX_FILENEWCCDZK_H__CD9AC68C_9F0A_4A57_B039_65FE1154DF92__INCLUDED_)
