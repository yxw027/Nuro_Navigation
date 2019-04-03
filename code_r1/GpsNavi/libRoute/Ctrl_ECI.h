#pragma once
#include "stdAfxRoute.h"
#include "NuroDefine.h"

typedef struct tag_ECI_Header
{
	nuLONG ECITotoalCount;
	nuLONG MaxNodeIDCount;
	nuBYTE Reserve[192];
}ECI_Header;

typedef struct tag_ECI_Data
{
	nuLONG   SNodeID : 20;//起始NodeID
	nuLONG   SRtBID : 12;
	nuLONG   PNodeID : 20;//起始NodeID
	nuLONG   PRtBID : 12;
	nuLONG   ALPReadID;
	nuSHORT  nNodeCount;
	nuSHORT  nNodeIdx;
	nuLONG	 B_Address;
}ECI_Data;

typedef struct tag_ECINode
{
	nuLONG   NodeID : 20;//起始NodeID
	nuLONG   RtBID : 12;
}ECINode;

class CCtrl_ECI
{
public:
	CCtrl_ECI(void);
	virtual ~CCtrl_ECI(void);
	nuTCHAR	ECIFName[NURO_MAX_PATH];
	nuFILE	*m_pECIFile;

	nuBOOL InitClass();
	nuVOID CloseClass();

	nuBOOL ReadFile();
	nuBOOL CheckECI(nuLONG RTB, nuDWORD *pNodeIDList, nuULONG &NodeCount, nuLONG &PicIdx, nuLONG NSSNodeCount);
private:
	nuVOID	InitConst();
	nuVOID  Free();
	ECI_Data **m_ppECIdata;
	ECINode  **m_ppECINode;
	ECI_Header m_ECIHeader;
};
