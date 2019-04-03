// CCDCtrl.h: interface for the CCCDCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCDCTRL_H__FBA703E7_9EC1_4E3E_82E6_0E4605C65959__INCLUDED_)
#define AFX_CCDCTRL_H__FBA703E7_9EC1_4E3E_82E6_0E4605C65959__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"

#define CCD_CHECK_DISTANCE					500
#define CCD_CHECK_ANGLE						30

#define CCD_MAX_NUM_PLAYED					5

#define CCD_MIN_VOICE_PLAYED				5

class CCCDCtrl  
{
public:
	typedef struct tagCCDPLAYED
	{
		nuBYTE	bUsed:1;
		nuroPOINT point;
	}CCDPLAYED, *PCCDPLAYED;

public:
	CCCDCtrl();
	virtual ~CCCDCtrl();

	nuBOOL Initialize();
	nuVOID Free();

public:
	nuBOOL LoadCCDData();
	nuBOOL CheckCCDData();

	nuBOOL DrawCCDPlayed();
//	nuBOOL CCDVoice();

protected:
	nuBOOL IsInList(nuLONG x, nuLONG y);
	nuBOOL AddACCD(nuLONG x, nuLONG y);
	nuBOOL RemoveCCDOut(const nuroRECT& rtRange);

protected:
	PCCDDATA	m_pCcdData;
	CCDPLAYED	m_pCCDPlayed[CCD_MAX_NUM_PLAYED];
	nuBYTE		m_nNowCCDNum;
	NURO_BMP	m_bmpCCD;
	nuWORD		m_nMinTimePlayed;
};

#endif // !defined(AFX_CCDCTRL_H__FBA703E7_9EC1_4E3E_82E6_0E4605C65959__INCLUDED_)
