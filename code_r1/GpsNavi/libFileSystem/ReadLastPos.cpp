// ReadLastPos.cpp: implementation of the CReadLastPos class.
//
//////////////////////////////////////////////////////////////////////

#include "ReadLastPos.h"
#include "NuroOpenedDefine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

nuroPOINT	CReadLastPos::m_ptLastPoint;
nuINT	CReadLastPos::m_nSaveTimer = 0;
CReadLastPos::CReadLastPos()
{

}

CReadLastPos::~CReadLastPos()
{

}

/*
nuBOOL CReadLastPos::ReadLastPos(const nuTCHAR *tsPath, nuLONG& x, nuLONG& y, short& angle)
{
	nuTCHAR tsFile[NURO_MAX_PATH];
	nuTcscpy(tsFile, tsPath);
	nuTcscat(tsFile, nuTEXT("LastPosition.txt"));
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	float LastPos_X, LastPos_Y, Speed, LastPos_Y2;
	nuWCHAR Date_84[8],Time_84[16];
	fscanf((FILE*)pFile, "%f %f %s %s %f %f", &LastPos_Y, &LastPos_X, Date_84, Time_84, &Speed, &LastPos_Y2);
	if( NURO_ABS(LastPos_Y-LastPos_Y2) > 0.5 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	nuDegreeToMeter(LastPos_X, x);
	nuDegreeToMeter(LastPos_Y, y);
	angle = 90;
	nuFclose(pFile);
	return nuTRUE;
}
*/
nuBOOL CReadLastPos::SaveLastGps(const nuTCHAR *tsPath, nuPVOID lpGPS)
{
	if( lpGPS == NULL )
	{
		return nuFALSE;
	}
	PGPSDATA pGpsData = (PGPSDATA)lpGPS;
	nuLONG ndis = NURO_ABS(m_ptLastPoint.x - pGpsData->nLongitude) +
				NURO_ABS(m_ptLastPoint.y - pGpsData->nLatitude);
	__android_log_print(ANDROID_LOG_INFO, "LastPoint", "SaveLastGps X %d, Y %d\n", pGpsData->nLongitude, pGpsData->nLatitude);
	if( ndis < 30 )
	{
		return nuFALSE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, tsPath);
	m_nSaveTimer %= 2;
	if( m_nSaveTimer )
	{
		nuTcscat(tsFile, /*FILE_LASTPOS*/FILE_LASTPOS2);
	}
	else
	{
		nuTcscat(tsFile, FILE_LASTPOS);
	}
	++m_nSaveTimer;
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_OB);
	if( pFile == NULL )
	{
		__android_log_print(ANDROID_LOG_INFO, "LastPoint", "SaveLastGps fopen Fail\n");
		return nuFALSE;
	}
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 )
	{
		__android_log_print(ANDROID_LOG_INFO, "LastPoint", "SaveLastGps Fseek Fail\n");
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFwrite(pGpsData, sizeof(GPSDATA) - sizeof(STARDATA)*MAX_STAR_NUMBER, 1, pFile) != 1 || 
		nuFwrite(&pGpsData->nLongitude, 4, 1, pFile) != 1 )
	{
		__android_log_print(ANDROID_LOG_INFO, "LastPoint", "SaveLastGps Fwrite Fail\n");
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFclose(pFile);
	return nuTRUE;
}

nuBOOL CReadLastPos::NewReadLastPos(const nuTCHAR *tsPath, nuLONG &x, nuLONG &y, nuSHORT &angle)
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, tsPath);
	nuTcscat(tsFile, FILE_LASTPOS);
	if( ReadLastPos(tsFile, x, y, angle) )
	{
		return nuTRUE;
	}
	nuTcscpy(tsFile, tsPath);
	nuTcscat(tsFile, FILE_LASTPOS2);
	return ReadLastPos(tsFile, x, y, angle);
}

nuBOOL CReadLastPos::ReadLastPos(const nuTCHAR *tsFile, nuLONG &x, nuLONG &y, nuSHORT &angle)
{
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		__android_log_print(ANDROID_LOG_INFO, "LastPoint", "ReadLastPos fopen Fail\n");
		return nuFALSE;
	}
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 )
	{
		__android_log_print(ANDROID_LOG_INFO, "LastPoint", "ReadLastPos Fseek Fail\n");
		nuFclose(pFile);
		return nuFALSE;
	}
	m_ptLastPoint.x = 0;
	m_ptLastPoint.y = 0;
	nuBYTE	pByte[sizeof(GPSDATA) - sizeof(STARDATA)*MAX_STAR_NUMBER];
	PGPSDATA pGpsData = (PGPSDATA)pByte;
	nuLONG	nCheck;
	if( nuFread(&y, sizeof(y), 1, pFile) != 1 ||
		nuFread(&x, sizeof(x), 1, pFile) != 1 ||
		nuFread(&angle, sizeof(angle), 1, pFile) != 1)
	{
		__android_log_print(ANDROID_LOG_INFO, "LastPoint", "ReadLastPos Fread Fail\n");
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFclose(pFile);
	__android_log_print(ANDROID_LOG_INFO, "LastPoint", "ReadLastPos X %d, Y %d\n", x, y);
	m_ptLastPoint.x = x;//GpsData.nLongitude;
	m_ptLastPoint.y = y;//GpsData.nLatitude;
	return nuTRUE;
	/*if( pGpsData->nLongitude != nCheck )
	{
		return nuFALSE;
	}
	m_ptLastPoint.x = x = pGpsData->nLongitude;
	m_ptLastPoint.y = y = pGpsData->nLatitude;
	angle  = pGpsData->nAngle;
	return nuTRUE;*/
}

