// LineCollect.h: interface for the CLineCollect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINECOLLECT_H__C101E395_5782_4871_9901_25A5587D5934__INCLUDED_)
#define AFX_LINECOLLECT_H__C101E395_5782_4871_9901_25A5587D5934__INCLUDED_

#include "NuroDefine.h"

#define CONJ_ERROR							0x00
#define CONJ_FIRSTLINE						0x01
#define CONJ_FROM_FROM						0x02
#define CONJ_FROM_TO						0x03
#define CONJ_TO_FROM						0x04
#define CONJ_TO_TO							0x05

#define DIS_CONJIONT						3

typedef struct tagLINESEG
{
	nuBYTE	nScreenDwIdx;
	nuBYTE	nConjWay;
//	nuWORD	nReserve;
	nuWORD	nScreenBlkIdx;
	nuDWORD	nScreenRoadIdx;
	tagLINESEG*	pNext;
}LINESEG, *PLINESEG;
typedef struct tagCONJLINE
{
	nuDWORD		nLineSegCount;
	NUROPOINT	ptFrom;
	NUROPOINT	ptTo;
	PLINESEG	pLineHead;
	PLINESEG	pLineEnd;
	tagCONJLINE*	pNext;
}CONJLINE, *PCONJLINE;
typedef struct tagCLASSLINE
{
	nuWORD	nClass;//Not used
	nuWORD	nConjLineCount;
	PCONJLINE	pConjHead;
	PCONJLINE	pConjEnd;
}CLASSLINE, *PCLASSLINE;

#define ROAD_COL_MEM_SIZE				(3200*sizeof(LINESEG) + 1500 * sizeof(CONJLINE))
#define ZOOM_ROAD_COL_MEM_SIZE			(200*sizeof(LINESEG) + 50 * sizeof(CONJLINE))
#define BGLINE_COL_MEM_SIZE				(320*sizeof(LINESEG) + 750 * sizeof(CONJLINE))

class CLineCollect  
{
public:
	CLineCollect();
	virtual ~CLineCollect();

	nuBOOL Initialize(nuWORD nClassCount, nuDWORD nMemSize = ROAD_COL_MEM_SIZE);
	nuVOID Free();

	nuBOOL AddLineSeg(nuLONG nScreenDwIdx, nuLONG nScreenBlkIdx, nuLONG nLineSegIdx, 
				      const NUROPOINT& ptFrom, const NUROPOINT& ptTo, nuWORD nClass);
	PCONJLINE GetConjLine(PCLASSLINE pClassLine, const NUROPOINT& ptFrom, const NUROPOINT& ptTo, nuPBYTE pConjWay);

	nuVOID DelConjLine(PCONJLINE pConjLine);
	nuVOID DelClassLine(PCLASSLINE pClassLine);

	nuVOID DelLineList();
	nuVOID FreeDataMem();

	nuBOOL AddCrossLineSeg(nuLONG nScreenDwIdx, nuLONG nScreenBlkIdx, nuLONG nLineSegIdx, 
					       const NUROPOINT& ptFrom, const NUROPOINT& ptTo, nuWORD nClass);
protected:
	nuVOID ColClassLine(PCLASSLINE pClassLine);
	nuLONG IsConjiontOfTwoConjLines(PCONJLINE pFConjLine, PCONJLINE pSConjLine);

public:
	nuDWORD		m_nLineSegCount;
	PCLASSLINE	m_pLineList;
	nuWORD		m_nClassCount;

protected:
	nuWORD		m_nMemIdx;
	nuPBYTE		m_pMemStartAddr;
	nuDWORD		m_nMemUsed;
	nuDWORD		m_nMemSize;
};

#endif // !defined(AFX_LINECOLLECT_H__C101E395_5782_4871_9901_25A5587D5934__INCLUDED_)
