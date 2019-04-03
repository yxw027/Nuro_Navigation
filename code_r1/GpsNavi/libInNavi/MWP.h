// MWP.h: interface for the CMWP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MWP_H__4DE01341_C8DB_4B07_A9F2_1A46511AB9A9__INCLUDED_)
#define AFX_MWP_H__4DE01341_C8DB_4B07_A9F2_1A46511AB9A9__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"

typedef struct tagNODELISTINFO
{
    nuLONG		  nNodeID;
	nuLONG        nFileIndex;
	nuLONG        nRTBIndex;
	nuDWORD       nNodeIndex;
}NODELISTINFO, *PNODELISTINFO;
class CMWP  
{
public:
	nuBOOL  FreeNewMem( );
	nuBOOL  AllocNewMem(nuULONG nMemOfByte);
	nuBOOL  GetNodeName(nuTCHAR *sFIleName, nuULONG nNameAdd, nuULONG nNameLen, nuCHAR *sName, nuULONG nOutNameLen);
	nuBOOL  GetNodeNameAddr(nuTCHAR *sFIleName, NODELISTINFO NodeInfo, nuULONG &nNameAddr, nuULONG &nNameLen );
	CMWP();
	virtual ~CMWP();

protected:
	NODELISTINFO   m_NowNodeInfo;
	nuBYTE         *m_pNewMem;
	nuLONG         m_nNodeTotal;
};

#endif // !defined(AFX_MWP_H__4DE01341_C8DB_4B07_A9F2_1A46511AB9A9__INCLUDED_)
