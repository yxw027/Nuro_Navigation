// TransfByRTZK.h: interface for the CTransfByRTZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSFBYRTZK_H__3F54C1E5_87A2_48D1_AE78_E314138DDAC0__INCLUDED_)
#define AFX_TRANSFBYRTZK_H__3F54C1E5_87A2_48D1_AE78_E314138DDAC0__INCLUDED_

#include <windows.h>
#include <stdio.h>

class CTransfByRTZK  
{
public:
	typedef struct tagRT_HEAD_COUNT
	{
		WORD	nMapID;
		WORD	nRtAreaCount;
	}RT_HEAD_COUNT, *PRT_HEAD_COUNT;
	typedef struct tagRT_DATA_A3
	{
		WORD	nBlkIdx;
		WORD	nRdIdx;
		DWORD	nFNodeID;
		DWORD	nTNodeID;
	}RT_DATA_A3, *PRT_DATA_A3;
	typedef struct tagRT_DATA_A_HEADER
	{
		DWORD	nTotalNodeCount;
		DWORD	nTotalRoadCount;
		DWORD	nTotalConnectCount;
		DWORD	nIndexAddr;
		DWORD	nConnectAddr;
	}RT_DATA_A_HEADER, *PRT_DATA_A_HEADER;

public:
	CTransfByRTZK();
	virtual ~CTransfByRTZK();

	bool ReadRTDataA3(const TCHAR *ptzFileRT);
	bool FindNodeIDByIdx(DWORD& nFNodeID, DWORD& nTNodeID, WORD nBlkIdx, WORD nRdIdx);
	
	void Free();

protected:
	WORD			m_nRtAreaCount;
	RT_DATA_A3**	m_ppstuDataA3;
	DWORD*			m_pdwA3Count;

};

#endif // !defined(AFX_TRANSFBYRTZK_H__3F54C1E5_87A2_48D1_AE78_E314138DDAC0__INCLUDED_)
