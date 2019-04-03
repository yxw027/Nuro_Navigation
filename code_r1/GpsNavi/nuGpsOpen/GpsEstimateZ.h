// GpsEstimateZ.h: interface for the CGpsEstimateZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPSESTIMATEZ_H__89CA454B_80D7_400E_996F_3BD199C23887__INCLUDED_)
#define AFX_GPSESTIMATEZ_H__89CA454B_80D7_400E_996F_3BD199C23887__INCLUDED_

#include "NuroClasses.h"
#include "NuroOpenedDefine.h"

#define NO_NEED_LIMIT_TIME
typedef unsigned int (*GPS_WgToChinaProc)(int wg_flag, unsigned int wg_lng, unsigned int wg_lat, int wg_heit, int wg_week, unsigned int wg_time, unsigned  int *china_lng, unsigned int *china_lat);

class CGpsEstimateZ  
{
public:
	CGpsEstimateZ();
	virtual ~CGpsEstimateZ();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL TryGetNewGPSData(PGPSDATA pGpsDat);
public:
	static nuLONG GetYearDay( nuLONG nYear);
	static nuLONG GetMonDays( nuLONG nYear,nuLONG nMon);
	static nuBOOL IsLeap(nuLONG nYear);
	static nuBOOL AddDays( NUROTIME &NowTime, nuLONG nAddDays);
	static nuBOOL JudgeTimePass(NUROTIME& NowTime);
	static nuBOOL CalTimeDiff( NUROTIME DTbefore, NUROTIME DTafter,nuINT *pnWeek, nuUINT  *pnTime );
	static nuBOOL FixToChina(PGPSDATA pGpsData);
	static GPS_WgToChinaProc	s_pWgToChina;

protected:
	nuroPOINT	m_ptLastForNC;
	nuroPOINT	m_ptLastRawUsed;
	nuBOOL		m_bGyroReady;
	nuBOOL		m_bGpsReady;
	nuWORD		m_nOldAngle;
private:
	nuWORD		m_nMinMoveDis;
	nuWORD		m_nMinMoveAngle;

};

#endif // !defined(AFX_GPSESTIMATEZ_H__89CA454B_80D7_400E_996F_3BD199C23887__INCLUDED_)
