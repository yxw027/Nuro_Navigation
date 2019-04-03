// MapFileFhZ.h: interface for the CMapFileFhZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILEFHZ_H__CFA77628_4443_4ED8_ACC7_F9C9265F54BC__INCLUDED_)
#define AFX_MAPFILEFHZ_H__CFA77628_4443_4ED8_ACC7_F9C9265F54BC__INCLUDED_

#include "FileMapBaseZ.h"

#define MAX_MAP_COUNT_OF_ONE_BLK			10
typedef struct tagBLK_FIX_DATA
{
	nuWORD	nMapIdx;
	nuDWORD	nBlkIdx;
}BLK_FIX_DATA;

typedef struct tagBLK_OUT_DATA
{
	nuWORD	nMapFixed;
	BLK_FIX_DATA	pBlkFixData[MAX_MAP_COUNT_OF_ONE_BLK];
}BLK_OUT_DATA, *PBLK_OUT_DATA;

class CMapFileFhZ  : public CFileMapBaseZ
{
public:
	typedef struct tagFHVERSION
	{
		nuBYTE	nMapVersion[64];
	}FHVERSION, *PFHVERSION;
	
	typedef struct tagFHHEADER
	{
		nuDWORD	nBlockCount;
		nuDWORD	YBlockLines;	
		nuWORD	YBlockStart;	
		nuWORD	YBlockEnd;		
		nuWORD	XBlockStart;		
		nuWORD	XBlockEnd;		
	}FHHEADER, *PFHHEADER;

	typedef struct tagFHXSTARTEND
	{
		nuWORD	nXStart;
		nuWORD	nXEnd;
	}FHXSTARTEND, *PFHXSTARTEND;
public:
	CMapFileFhZ();
	virtual ~CMapFileFhZ();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuBOOL BlockIDToMap(nuWORD nXBlock, nuWORD nYBlock, PBLK_OUT_DATA pBlkOut);
	nuBOOL PtInBoundary(const nuroPOINT& point);

protected:
	nuBOOL	ReadFhFileHeader();
	nuBOOL	ReadFhFileBody();

protected:
	NURORECT		m_rtBoundary;
	nuLONG			m_nXStEdSize;
	nuLONG			m_nBlkUsedSize;
	nuLONG			m_nBlkStateSize;
	nuWORD			m_nMemIdx;
	nuWORD			m_nMapCount;
	PFHHEADER		m_pFhHead;
};

#endif // !defined(AFX_MAPFILEFHZ_H__CFA77628_4443_4ED8_ACC7_F9C9265F54BC__INCLUDED_)
