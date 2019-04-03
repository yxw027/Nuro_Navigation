#ifndef __NURO_GPSOPEN_20080907
#define __NURO_GPSOPEN_20080907

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"



#ifdef _USRDLL
#ifdef GPSOPEN_EXPORTS
#define GPSOPEN_API extern "C" __declspec(dllexport)
#else
#define GPSOPEN_API //extern "C" __declspec(dllimport)
#endif
#else
#define GPSOPEN_API
#endif


GPSOPEN_API nuBOOL LibGPS_InitGpsOpen(PAPISTRUCTADDR pApiAddr);
GPSOPEN_API nuVOID LibGPS_FreeGpsOpen();

GPSOPEN_API nuBOOL LibGPS_Open();
GPSOPEN_API nuBOOL LibGPS_ReSet( PGPSCFG pGpsSetData);
GPSOPEN_API nuBOOL LibGPS_Close();

GPSOPEN_API nuBOOL LibGPS_RecvData( const nuCHAR *pBuff, nuINT len);
GPSOPEN_API nuBOOL LibGPS_GetNewData(PGPSDATA* ppGpsDat);
GPSOPEN_API nuBOOL LibGPS_GetNewData2(PGPSDATA* ppGpsDat);
GPSOPEN_API nuBOOL LibGPS_GetGpsXY(nuLONG *pX, nuLONG *pY);
GPSOPEN_API nuBYTE LibGPS_GetState();

GPSOPEN_API nuBOOL  LibGPS_FIXPOINT(nuUINT &nLon, nuUINT &nLat, const nuINT &nHeight, const NUROTIME & timeRecv  );
GPSOPEN_API nuVOID  LibGPS_FeedBackInfo(nuLONG nRoadx, nuLONG nRoady, nuSHORT dx, nuSHORT dy, nuSHORT nSpeed, nuINT nTime);
GPSOPEN_API nuVOID LibGPS_FeedBack(const FEEDBACKDATA* pFeedBackInfo);
GPSOPEN_API nuVOID LibGPS_SetGPSChina(nuPVOID lpFunc);

GPSOPEN_API nuVOID LibGPS_TOBEDEBUGINFO(PGPSMODINFO* ppGpsModInfo);
GPSOPEN_API nuVOID LibGPS_TOBERESETGYRO();
GPSOPEN_API nuVOID LibGPS_TOBEGETVERSION();
GPSOPEN_API nuVOID LibGPS_GetRawData(PGPSDATA pGpsData);//added by daniel @20120204
GPSOPEN_API nuBOOL LibGPS_GetGPSComConfig(nuLONG& GPSCommPort, nuLONG& GPSBaudrate); //Added by Damon 20120705 
GPSOPEN_API nuVOID LibGPS_RoadDelay(nuBOOL bDelay, nuBOOL b3DMode);
GPSOPEN_API nuBOOL LibGPS_ELEAD_TRAN(const GYROGPSDATA_EL* pEL_GyroData);//怡利資料轉換-ELEAD data transfer
GPSOPEN_API nuVOID LibGPS_WriteCom(nuBYTE *Data, nuLONG nLen);
#ifdef ANDROID_GPS
GPSOPEN_API nuBOOL LibGPS_JNI_RecvData( const nuCHAR *pBuff, nuINT len );
#endif
#endif