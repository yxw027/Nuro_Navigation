#if !defined(AFX_MAPFILEBHY_H__6FDD0C4A_7FC3_42F1_BC8C_4CF014A223CD__INCLUDED_)
#define AFX_MAPFILEBHY_H__6FDD0C4A_7FC3_42F1_BC8C_4CF014A223CD__INCLUDED_

#include "NuroDefine.h"

typedef struct tag_TMC_Header
{
	nuDWORD DataCnt;
	nuDWORD VertexDataAddr;
	nuDWORD TrafficDataAddr;
	nuWORD  TrafficCnt;
	nuWORD  Reserve;	
}TMC_Header, *PTMC_Header;

typedef struct tag_TMC_DATA
{ 
    nuWORD  LocationCode;
	nuWORD  LocationCodeNum;
	nuDWORD LocationVertexAddr;
	nuDWORD LocationUpAddr;
	nuDWORD LocationDownAddr;
}TMC_DATA, *PTMC_DATA;

typedef struct tag_VertexData
{
	nuDWORD rn_Addr;
	nuDWORD StartVertex;
	nuDWORD EndVertex;
    nuDWORD Reserve;
}TMC_VertexData, *PTMC_VertexData;

typedef struct tag_TMC_Info_Router
{
    nuWORD  location_id;  //--->nodes
    nuWORD  event_id;     //weighting
}TMC_Info_Router, *PTMC_Info_Router;

typedef struct tag_TMC_Info_Router2
{
    nuBOOL  TMCPass;
	nuLONG  TMCWeighting;
	nuSHORT TMCTime;
	nuSHORT TMCnodes;
	nuLONG  TMCLocationAddr;
	nuLONG  TMCNameaddr;   //
	nuWCHAR *TMCEventName; //
}StructTMCPassData, *PStructTMCPassData;
    
typedef struct tag_TMC_Info_UI
{
    nuWORD  location_len;
    nuWORD  location_name[50];
    nuWORD  event_len;
    nuWORD  event_name[50];		 
}TMC_Info_UI, *PTMC_Info_UI;

class CFileMapTMC   
{
public:
	CFileMapTMC();
	virtual ~CFileMapTMC();	
    nuVOID GetTMC();
	nuVOID Close();

private:
    nuVOID ShowEvent(nuINT); 
    nuVOID ShowLocation(nuINT);

private:
	PTMC_Info_Router   r1;
	PStructTMCPassData r2;
    PTMC_Info_UI       u1;
	nuINT              arry_idx;
};

#endif // !defined(AFX_MAPFILEBHY_H__6FDD0C4A_7FC3_42F1_BC8C_4CF014A223CD__INCLUDED_)
