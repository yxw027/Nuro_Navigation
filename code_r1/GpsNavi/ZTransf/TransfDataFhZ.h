// TransfDataFhZ.h: interface for the CTransfDataFhZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSFDATAFHZ_H__6F4B1654_0EC1_42E6_8946_7E75D7ACBB4F__INCLUDED_)
#define AFX_TRANSFDATAFHZ_H__6F4B1654_0EC1_42E6_8946_7E75D7ACBB4F__INCLUDED_

#include <windows.h>
#include <stdio.h>

#define BLOCK_COUNT_OF_EACH_LINE			18000
class CTransfDataFhZ  
{
public:
	typedef struct tagFHHEADER
	{
		BYTE	nMapVersion[64];
		DWORD	nBlockCount;
		DWORD	nBlockLines;
		WORD	nYBlockStart;
		WORD	nYBlockEnd;
		WORD	nXBlockStart;
		WORD	nXBlockEnd;
	}FHHEADER, *PFHHEADER;
	typedef struct tagXSTARTEND
	{
		WORD	nXStart;
		WORD	nXEnd;
	}XSTARTEND, *PXSTARTEND;
public:
	CTransfDataFhZ();
	virtual ~CTransfDataFhZ();

	bool TransfFh(const TCHAR *tsBh, const TCHAR*tsFh);
protected:
	bool FhDataProc(FILE* pFileBh, FILE *pFileFh);
};

#endif // !defined(AFX_TRANSFDATAFHZ_H__6F4B1654_0EC1_42E6_8946_7E75D7ACBB4F__INCLUDED_)
