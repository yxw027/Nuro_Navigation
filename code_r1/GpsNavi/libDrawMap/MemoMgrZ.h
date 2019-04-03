// MemoMgrZ.h: interface for the CMemoMgrZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMOMGRZ_H__A2F79E67_44B1_4835_A09F_CFF801B0194A__INCLUDED_)
#define AFX_MEMOMGRZ_H__A2F79E67_44B1_4835_A09F_CFF801B0194A__INCLUDED_

#include "NuroClasses.h"

#define _MEMO_VOICED_LIST_MAX					10

#define _MEMO_BMP_FILE_NAME						nuTEXT("Media\\Pic\\Memo.bmp")

class CMemoMgrZ  
{
public:
	typedef struct tagMEMOVOICED
	{
		nuBYTE	  nState;
		nuroPOINT point;
	}MEMOVOICED, *PMEMOVOICED;

public:
	CMemoMgrZ();
	virtual ~CMemoMgrZ();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL RemoveOutMemo(nuroRECT* pRange);
	nuBOOL AddVoiceMemo(nuLONG x, nuLONG y);
	nuBOOL DrawMemoIcon(nuLONG x, nuLONG y, nuLONG nIconIdx);

protected:
//	nuBOOL LoadMemoBmp();

protected:
	NURO_BMP	m_bmpMemo;
	MEMOVOICED	m_memoList[_MEMO_VOICED_LIST_MAX];
	nuBYTE		m_nMemoCount;
};

#endif // !defined(AFX_MEMOMGRZ_H__A2F79E67_44B1_4835_A09F_CFF801B0194A__INCLUDED_)
