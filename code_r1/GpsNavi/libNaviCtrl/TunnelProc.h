#pragma once

#include "NuroDefine.h"
#include "NuroClasses.h"
#include "nurotypes.h"
#include "NuroModuleApiStruct.h"

#define _TUNNEL_STATE_1	   1
#define _TUNNEL_STATE_2	   6
#define _TUNNEL_ROAD_COUNT 100
typedef struct tag_TUNNEL_DW
{
	nuLONG     PtCount;
	NUROPOINT *pptTunnel;
}TUNNEL_DW,*PTUNNEL_DW;

class CTunnelProc
{
public:
	CTunnelProc(void);
	~CTunnelProc(void);
	nuBOOL				TunnelProc(PACTIONSTATE pActionState, nuLONG lSpeed);
	nuVOID				GetTunnelRoadInfo();
	nuVOID				GetTunnelDW(const CROSSROADINFO &CrossRoadData, const NUROPOINT &ptLastPoint, TUNNEL_DW &TunnelDW);
	nuVOID				TunnelSimulation(PACTIONSTATE pActionState);
	nuVOID				LeaveTunnel();
public:
	TUNNEL_DW			m_pTunnelDW[_TUNNEL_ROAD_COUNT];
	nuBYTE			    m_nTunnelRoadCount;
	nuBOOL				m_bGetTunnelData;
	nuLONG				m_lEnterTunnelSpeed;
	nuLONG				m_lPreTick;
	nuLONG			    m_lSegDis;
	nuLONG			    m_lDis;
	nuLONG				m_lSegTunnelDis;
	nuLONG			    m_PtTunnelIndex;
	nuLONG				m_RdTunnelIndex;
};
