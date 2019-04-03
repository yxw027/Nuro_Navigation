// GpsEstimateZ.cpp: implementation of the CGpsEstimateZ class.
//
//////////////////////////////////////////////////////////////////////

#include "GpsEstimateZ.h"
#include "NuroClasses.h"
#include "nuroGpsRelease.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GPS_WgToChinaProc CGpsEstimateZ::s_pWgToChina = NULL;

CGpsEstimateZ::CGpsEstimateZ()
{
	m_ptLastForNC.x	= 0;
	m_ptLastForNC.y	= 0;
	m_ptLastRawUsed.x	= 0;
	m_ptLastRawUsed.y	= 0;
	m_bGpsReady	= false;
	m_bGyroReady	= false;
}

CGpsEstimateZ::~CGpsEstimateZ()
{
	Free();
}

nuBOOL CGpsEstimateZ::Initialize()
{
	m_ptLastForNC.x	= 0;
	m_ptLastForNC.y	= 0;
	m_ptLastRawUsed.x	= 0;
	m_ptLastRawUsed.y	= 0;
	m_bGpsReady		= nuFALSE;
	m_bGyroReady	= nuFALSE;
	nuLONG nTmp;
	if( !nuReadConfigValue("MINMOVEDIS", &nTmp) )
	{
		nTmp = 15;
	}
	m_nMinMoveDis = (nuWORD)nTmp;
	if( !nuReadConfigValue("MINMOVEANGLE", &nTmp) )
	{
		nTmp = 0;
	}
	m_nMinMoveAngle = (nuWORD)nTmp;
	return nuTRUE;
}

nuVOID CGpsEstimateZ::Free()
{
	;
}

nuBOOL CGpsEstimateZ::TryGetNewGPSData(PGPSDATA pGpsDat)
{
	if( pGpsDat->nStatus == DATA_TYPE_GPS )
	{
		if( !m_bGpsReady )
		{
			m_bGpsReady = true;
		}
	}
	else if( pGpsDat->nStatus == DATA_TYPE_GYRO )
	{
		if( !m_bGyroReady )
		{
			m_bGyroReady = true;
		}
	}
	else
	{
		;
	}
	return true;
}

nuBOOL CGpsEstimateZ::FixToChina(PGPSDATA pGpsData )
{
	// Modified by Damon 20120702
	//if ( s_pWgToChina == NULL )
	//{
	//	return false;
	//}
	//unsigned int &nLng, unsigned int &nLat, const int &nHeight, const NUROTIME &TimeNow
	static nuBOOL bFirst = nuFALSE;

	nuUINT nLngIn = (nuUINT)( pGpsData->nLongitude * 1.024 * 36 );
	nuUINT nLatIn = (nuUINT)( pGpsData->nLatitude * 1.024 * 36 );
	nuUINT nLngOut = 0;
	nuUINT nLatOut = 0;
	nuINT nHeight       = pGpsData->nAntennaHeight;

	NUROTIME N_timeBef = {0},N_timeAfter = {0};
	N_timeBef.nYear     = 1980;
	N_timeBef.nMonth    = 1;
	N_timeBef.nDay      = 6;
	N_timeBef.nHour     = 0;
	N_timeBef.nMinute   = 0;
	N_timeBef.nSecond   = 0;

	N_timeAfter.nYear   = pGpsData->nTime.nYear;
	N_timeAfter.nMonth  = pGpsData->nTime.nMonth;
	N_timeAfter.nDay    = pGpsData->nTime.nDay;
	N_timeAfter.nHour   = pGpsData->nTime.nHour;
	N_timeAfter.nMinute = pGpsData->nTime.nMinute;
	N_timeAfter.nSecond = pGpsData->nTime.nSecond;

	nuINT nWeek;
	nuUINT nTime;
	nWeek = 0;
	nTime = 0;
	nuBOOL bRet = !JudgeTimePass( N_timeAfter );
	CalTimeDiff( N_timeBef, N_timeAfter, &nWeek, &nTime);
	pGpsData->nTime.nYear    = N_timeAfter.nYear;
	pGpsData->nTime.nMonth   = N_timeAfter.nMonth ;
	pGpsData->nTime.nDay     = N_timeAfter.nDay;
	pGpsData->nTime.nHour   = N_timeAfter.nHour;
	pGpsData->nTime.nMinute = N_timeAfter.nMinute;
	pGpsData->nTime.nSecond = N_timeAfter.nSecond ;
	if ( bRet ) // = true--Time Over
	{ 
		// Modified by Damon 20120702
		// Modified by Dengxu @ 2012 12 19
		if ( wgtochina_lb(EMG_DECRYPTION, bFirst, nLngIn, 
			nLatIn, nHeight, /*nWeek */1543, /*nTime*/153200231, &nLngOut, &nLatOut ) )
		{
			return nuFALSE;
		}
	}
	pGpsData->nLongitude	= (nuLONG)((nLngOut*1.0) / (36 * 1.024));
	pGpsData->nLatitude	= (nuLONG)((nLatOut*1.0) / (36 * 1.024));

	if( !bFirst )
	{
		bFirst = nuTRUE;	
	}
	return nuTRUE;
}

nuBOOL CGpsEstimateZ::CalTimeDiff(NUROTIME DTbefore, NUROTIME DTafter, nuINT *pnWeek, nuUINT *pnTime)
{
	if ( DTafter.nYear < 100  && DTafter.nYear >= 80)
	{
		DTafter.nYear += 1900;
	}
	else if ( DTafter.nYear >= 0 && DTafter.nYear < 50)
	{
		DTafter.nYear += 2000;
	}
	/**/
	nuLONG nDayCount = 0;
	nuLONG i = 0;
	//Cal the Before Days
	nuINT nM2Day = 28;
	if(   ((DTbefore.nYear % 4 == 0) && (DTbefore.nYear % 100 != 0 ))
		|| DTbefore.nYear % 400 == 0 )
	{
		nM2Day = 29;
	}
	for ( i = 1; i < DTbefore.nMonth; i++ )
	{
		if( i == 2 )
		{
			nDayCount -= nM2Day;
		}
		else if( i == 4 || i == 6 || i == 9 || i == 11 )
		{
			nDayCount -= 30;
		}
		else
		{
			nDayCount -= 31;
		}
	}
	nDayCount -= DTbefore.nDay ;

	//Cal Days
	for(i = DTbefore.nYear; i < DTafter.nYear; ++i )
	{
		if(   ((i % 4 == 0) && (i % 100 != 0 ))
			|| i % 400 == 0 )
		{
			nDayCount += 366;
		}
		else
		{
			nDayCount += 365;
		}
	}

	nM2Day = 28;
	if(   ((i % 4 == 0) && (i % 100 != 0 ))
		|| i % 400 == 0 )
	{
		nM2Day = 29;
	}
	for( i = 1; i < DTafter.nMonth; ++i )
	{
		if( i == 2 )
		{
			nDayCount += nM2Day;
		}
		else if( i == 4 || i == 6 || i == 9 || i == 11 )
		{
			nDayCount += 30;
		}
		else
		{
			nDayCount += 31;
		}
	}
	nDayCount += DTafter.nDay ;
	*pnWeek = nDayCount / 7;
	*pnTime = ( (((nDayCount % 7) * 24 + DTafter.nHour) * 60 + DTafter.nMinute) * 60 + DTafter.nSecond ) *1000 + DTafter.nMilliseconds ;

	/*
	short int nDiffYear = DTafter.nYear - DTbefore.nYear;
	long  nDayTotal = 0;
	long  nSecondTotal = 0;
	int i;
	int daytab[13]={0,31,28,31,30,31,30,31,31,30,31,30,31}; 
	int DownDayTab[13] = {0,365,334,306,275,245,214,184,153,122,92,61,31};
	int UpDayTab[] = {0,0,31,59,90,120,151,181,212,243,273,304,334};
	if( nDiffYear == 0 )
	{
	for ( i = DTbefore.nMonth; i < DTafter.nMonth; i++ )
	{
	nDayTotal += daytab[i];
	}
	nDayTotal -= DTbefore.nDay;
	nDayTotal += DTafter.nDay;
	if ( DTbefore.nMonth <= 2 && DTafter.nMonth > 2 )
	{
	if (   ((DTbefore.nYear) % 4 == 0 && (DTbefore.nYear  ) % 100 != 0 )
	|| ( DTbefore.nYear) % 400 == 0 )
	{
	nDayTotal += 1;
	}

	}
	}
	else
	{
	for (  i = 0; i <= nDiffYear; i++ )
	{
	if( i == 0 )
	{
	nDayTotal += DownDayTab[DTbefore.nMonth];
	nDayTotal -= (DTbefore.nDay - 1 );

	}
	else if ( i < nDiffYear )
	{
	nDayTotal += 365;
	}
	else
	{
	nDayTotal += UpDayTab[DTafter.nMonth];
	nDayTotal += (DTafter.nDay - 1);
	}
	if (   ((DTbefore.nYear + i) % 4 == 0 && (DTbefore.nYear + i ) % 100 != 0 )
	|| ( DTbefore.nYear + i ) % 400 == 0 )
	{
	if(   i == 0 && DTbefore.nMonth <= 2
	||i == nDiffYear && DTafter.nMonth > 2
	|| i != 0 && i != nDiffYear )
	{
	nDayTotal += 1;
	}

	}
	}
	}

	nSecondTotal = DTafter.nHour * 3600 + DTafter.nMinute * 60 + DTafter.nSecond
	- DTbefore.nHour * 3600 - DTbefore.nMinute * 60 - DTbefore.nSecond; 
	if (   DTbefore.nYear != DTafter.nYear 
	|| DTbefore.nMonth  != DTafter.nMonth
	|| DTbefore.nDay  != DTafter.nDay )
	{
	if ( nSecondTotal < 0)
	{
	nDayTotal -= 1;
	nSecondTotal += 24*3600;
	}	
	}
	*pnWeek = nDayTotal / 7;
	*pnTime = ((nDayTotal % 7) * 24 * 3600 + nSecondTotal)*1000 + DTafter.nMilliseconds ;	
	*/
	return nuTRUE;
}

nuBOOL CGpsEstimateZ::JudgeTimePass(NUROTIME& NowTime)
{
	if (  (NowTime.nYear % 100 ) > 80 
		&&(NowTime.nYear % 100 ) < 99 )
	{
		//Add Days = 7168 for Altina  Type
		AddDays( NowTime, 7168 );	 
	}
	NUROTIME LimitTime = {0};
	LimitTime.nYear  = 2036;
	LimitTime.nMonth = 12;
	LimitTime.nDay   = 1;
	if ( NowTime.nYear < 2000 )
	{
		NowTime.nYear += 2000;
	}

#ifdef NO_NEED_LIMIT_TIME
	return false;
#else
	if (   ( NowTime.nYear ) > ( LimitTime.nYear  )
		|| ( (( NowTime.nYear  ) == ( LimitTime.nYear  )) && ( NowTime.nMonth >  LimitTime.nMonth) )
		|| ( (( NowTime.nYear ) == ( LimitTime.nYear  )) && ( NowTime.nMonth ==  LimitTime.nMonth)  &&  ( NowTime.nDay >  LimitTime.nDay)  )
		)
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
#endif
}

nuBOOL CGpsEstimateZ::AddDays(NUROTIME& NowTime, nuLONG nAddDays)
{
	if ( NowTime.nYear > 80 && NowTime.nYear < 99  )
	{
		NowTime.nYear += 1900;
	}
	else
	{
		NowTime.nYear = ( NowTime.nYear % 100 ) + 2000;
	}
    
	nuLONG nDayCal = 0;
	nuLONG nYearIndex = NowTime.nYear;
	//Step 1:首先计算到本月末的时间
	nuLONG nDayTmp = GetMonDays( NowTime.nYear, NowTime.nMonth ) - NowTime.nDay;
	if ( nAddDays > nDayTmp  )
	{
		++NowTime.nMonth;
		if ( NowTime.nMonth > 12 )
		{
			++NowTime.nYear;
			NowTime.nMonth = 1;
		}
		NowTime.nDay = 0;
		nAddDays -= nDayTmp;
	}
	else//结束
	{
		NowTime.nDay += nAddDays;
		nAddDays = 0;
	}
	
	//Step 2:首先计算第一年到尾的天数或者小于的年尾日期就直接计算完成
	if ( nAddDays )
	{
		nuLONG nMon = NowTime.nMonth ;
		for ( nMon; nMon <= 12; nMon ++ )
		{
		    nDayTmp = GetMonDays( NowTime.nYear, NowTime.nMonth );
			if ( nAddDays > nDayTmp )
			{
				++NowTime.nMonth;
				if ( NowTime.nMonth > 12 )
				{
					++NowTime.nYear;
					NowTime.nMonth = 1;
				}
				nAddDays -= nDayTmp;
			}
			else
			{
				NowTime.nDay += nAddDays;
				nAddDays = 0;
			}
		}
	}
	//Step 3:
	while ( nAddDays )
	{
		nDayTmp = GetYearDay( NowTime.nYear );
		if ( nAddDays > nDayTmp )
		{
			++NowTime.nYear;
			nAddDays -= nDayTmp;
		}
		else //按每个月的时间计算
		{
			for( ; NowTime.nMonth <= 12 && nAddDays; ++NowTime.nMonth )
			{
				nDayTmp = GetMonDays( NowTime.nYear, NowTime.nMonth );
				if ( nAddDays > nDayTmp )
				{
					nAddDays -= nDayTmp;
				}
				else
				{
					NowTime.nDay += nAddDays;
					nAddDays = 0;
					break;
				}
			}

		}
	}
	
	

	return nuTRUE;
}

nuBOOL CGpsEstimateZ::IsLeap(nuLONG nYear)
{
	if (   (nYear % 4 == 0 && nYear % 100 != 0 )
		||  nYear % 400 == 0 )
	{
		return nuTRUE;
	}
	else 
	{
		return nuFALSE;
	}
}

nuLONG CGpsEstimateZ::GetMonDays(nuLONG nYear, nuLONG nMon)
{
	nuLONG nDayCal = 0;
	if ( nMon == 2 )
	{
		if ( IsLeap( nYear) )
		{
			nDayCal = 29;
		}
		else
		{
			nDayCal = 28;
		}
		
	}
	else if ( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 )
	{	
		nDayCal = 30;
	}
	else
	{
		nDayCal = 31;
	}
	return nDayCal;
}

nuLONG CGpsEstimateZ::GetYearDay(nuLONG nYear)
{
	if ( IsLeap( nYear) )
	{
		return 366;
	}
	else
	{
		return 365;
	}
}
