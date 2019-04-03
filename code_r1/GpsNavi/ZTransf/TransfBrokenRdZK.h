// TransfBrokenRdZK.h: interface for the CTransfBrokenRdZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSFBROKENRDZK_H__96FF55B7_EDA8_42F4_A4B3_2990708BCAD8__INCLUDED_)
#define AFX_TRANSFBROKENRDZK_H__96FF55B7_EDA8_42F4_A4B3_2990708BCAD8__INCLUDED_

#include "NuroDefine.h"

class CTransfBrokenRdZK  
{
public:
	typedef struct tagDISASTER_RAW
	{
		nuWORD	nStartYear;
		nuWORD	nStartMonth;
		nuWORD	nStartDay;
		nuWORD	nEndYear;
		nuWORD	nEndMonth;
		nuWORD	nEndDay;
		nuroPOINT	point;
	}DISASTER_RAW, *PDISASTER_RAW;
	typedef struct tagDISATER_OUT
	{
		nuWORD	nStartY;
		nuBYTE	nStartM;
		nuBYTE	nStartD;
		nuWORD	nEndY;
		nuBYTE	nEndM;
		nuBYTE	nEndD;
		nuDWORD	nFNodeID;
		nuDWORD	nTNodeID;
	}DISATER_OUT, *PDISATER_OUT;
public:
	CTransfBrokenRdZK();
	virtual ~CTransfBrokenRdZK();

	bool TransfRoadPoint(const TCHAR *ptzFileRaw, const TCHAR *ptzFileRT, const TCHAR *ptzFileOut);
protected:
	bool ReadDisasterRawData(const TCHAR *ptzFileRaw);
	void FreeData();
	void SortDstOut(PDISATER_OUT pDstOut, nuDWORD nCount);
protected:
	nuDWORD			m_dwDisasterCount;
	PDISASTER_RAW	m_pstuDisaterHead;

};

#endif // !defined(AFX_TRANSFBROKENRDZK_H__96FF55B7_EDA8_42F4_A4B3_2990708BCAD8__INCLUDED_)
