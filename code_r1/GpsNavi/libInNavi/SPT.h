// SPT.h: interface for the CSPT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPT_H__B10AD6AC_50EB_4B93_8CA6_523E8F39E7E9__INCLUDED_)
#define AFX_SPT_H__B10AD6AC_50EB_4B93_8CA6_523E8F39E7E9__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"

typedef struct tagNODEID
{
	nuUSHORT        nBlockID;
	nuUSHORT        nRoadID;
}NODEID, *PNODEID;

typedef struct tagSPTNODELISTINFO
{
	nuLONG        nFileIndex;
	NODEID        NodeId;
}SPTNODELISTINFO, *PSPTNODELISTINFO;

class CSPT  
{
public:
	nuBOOL  FreeNewMem( );
	nuBOOL  AllocNewMem(nuULONG nMemOfByte);
	nuBOOL  GetNodeName(nuTCHAR *sFIleName, nuULONG nNameAdd, nuULONG nNameLen, nuCHAR *sName, nuULONG nOutNameLen);
	nuBOOL  GetNodeNameAddr( nuTCHAR *sFIleName, SPTNODELISTINFO NodePreInfo, SPTNODELISTINFO NodeNextInfo, 
		                     nuULONG &nNameAddr, nuULONG &nNameLen );
	CSPT();
	virtual ~CSPT();

protected:
	SPTNODELISTINFO  m_NowNodeInfo;
	nuBYTE           *m_pNewMem;
	nuLONG           m_nNodeTotal;
	nuLONG           m_nSignpostDirTotal;
};

#endif // !defined(AFX_SPT_H__B10AD6AC_50EB_4B93_8CA6_523E8F39E7E9__INCLUDED_)
