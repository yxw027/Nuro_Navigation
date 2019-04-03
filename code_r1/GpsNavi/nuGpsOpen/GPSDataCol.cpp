// GPSDataCol.cpp: implementation of the CGPSDataCol class.
//
//////////////////////////////////////////////////////////////////////

#include "GPSDataCol.h"

#include "GpsEstimateZ.h"
#include <stdio.h>
#include <stdlib.h>
//#include "nuOS.h"
#define NumComb(a,b,c,d) ((a<<24)+(b<<16)+(c<<8)+d)
#define	DEF_INIPASS			10

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGPSDataCol::CGPSDataCol()
{
	InitClass();
}

CGPSDataCol::~CGPSDataCol()
{

}

nuVOID CGPSDataCol::InitClass()
{
	nuMemset((nuVOID *)&m_FirstHandData,0,sizeof(m_FirstHandData));
	nuMemset((nuVOID *)&m_GpsModInfo,0,sizeof(m_GpsModInfo));
	nuMemset((nuVOID *)&m_BackData,0,sizeof(m_BackData));
	m_BackData.nAngle=90;
	bFindRMC			= nuFALSE;
	bFindGGA			= nuFALSE;
	bFindGSV			= nuFALSE;
	m_SpecialCaseData.l_DRMode	= FINDDR_NO;
	g_bHaveNewData		= nuFALSE;


	//陀螺儀監控結構的初始化
	stuGyro.bAngleInit=nuFALSE;
	stuGyro.bSpeedInit=nuFALSE;
	stuGyro.bAngleInput=nuFALSE;
	stuGyro.bSpeedInput=nuFALSE;
	stuGyro.lSystemStatus=DATA_TYPE_INVALID;
	stuGyro.lDeltaAngle=0;
	stuGyro.lDeltaDis=0;

	m_FirstHandData.nStatus=DATA_TYPE_INVALID;

	//Louis 090910 New GPSDataCollation
	nuMemset((nuVOID *)byTempSaveBuffer,0,MAXSAVEBUFFER);
	bStartSave=nuFALSE;
	lNowNEMACode=NMEAID_NONE;
	byCheckSum=0;
	nuMemset((nuVOID *)byChecksumBuffer,0,3);
	byChecksumBufferCount=0;
	bAddNewStar=nuFALSE;

	if ( !nuReadConfigValue("ZONETIME", &(m_nZoneTime)) )
	{
		m_nZoneTime = 8;
	}
	if ( !nuReadConfigValue("SERIALCHECK", &m_nChecrSerRec ))
	{
		m_nChecrSerRec = 0;
	}
	if ( !nuReadConfigValue("GPSCHINAFIX", &m_nChinaFix))
	{
		m_nChinaFix = 0;
	}
	if( !nuReadConfigValue("TUNELDELAY", &g_TunelDelay) )
	{
		g_TunelDelay = 3;
	}
	if( !nuReadConfigValue("HASDRSYSTEM", &g_hasDRSystem) )
	{
		g_hasDRSystem = 1;
	}


	//Louis 20110704 for LCMM
	nuMemset(&(m_SpecialCaseData.m_UBLOS.ModInfo),NULL,sizeof(GPSMODINFO));
	b_updateModInfo=nuFALSE;
	nuMemset(&g_FixedInfo,NULL,sizeof(FEEDBACKINFO));
	l_FixAngleStableCount=0;
	IsTunel=nuFALSE;
}

//Louis 090910 New GPSDataCollation
nuBOOL CGPSDataCol::NewConvertNewData(nuBYTE *NewData,nuLONG DataLen,nuLONG lGPSMode)
{
	nuLONG i = 0,j = 0;
	nuINT bySyscheck = 0;
	nuBOOL bReNum = 0;
	bReNum=nuFALSE;

	for(i=0;i<DataLen;i++)
	{
		if(byTempSaveBufferCount>=MAXSAVEBUFFER-1)//超過buffer上限
		{
			byTempSaveBufferCount=0;
			byTempSaveBuffer[MAXSAVEBUFFER-1]=NULL;
			bStartSave=nuFALSE;
		}

		if(!bStartSave)//資料尚未開始記錄時 就進入這個判定
		{
			if(byChecksumBufferCount==0)
			{
				if(NewData[i]==0xB5 || NewData[i]=='$')//各種開頭判定
				{
					InitNewConvertNewData();
					byTempSaveBuffer[byTempSaveBufferCount]=NewData[i];
					byTempSaveBufferCount++;
					continue;
				}
			}
		}
		else
		{
			//連續記錄
			byTempSaveBuffer[byTempSaveBufferCount]=NewData[i];
			byTempSaveBufferCount++;

			if(byTempSaveBuffer[byTempSaveBufferCount-2]==0xB5 && byTempSaveBuffer[byTempSaveBufferCount-1]==0x62)//ublos類表頭重置
			{
				InitNewConvertNewData();
				byTempSaveBuffer[byTempSaveBufferCount]=0xB5;
				byTempSaveBufferCount++;
				byTempSaveBuffer[byTempSaveBufferCount]=0x62;
				byTempSaveBufferCount++;
			}
			if(byTempSaveBuffer[byTempSaveBufferCount-2]=='$' && byTempSaveBuffer[byTempSaveBufferCount-1]=='G')//標準NMEA子句
			{
				InitNewConvertNewData();
				byTempSaveBuffer[byTempSaveBufferCount]='$';
				byTempSaveBufferCount++;
				byTempSaveBuffer[byTempSaveBufferCount]='G';
				byTempSaveBufferCount++;
			}
			if(byTempSaveBuffer[byTempSaveBufferCount-2]=='$' && byTempSaveBuffer[byTempSaveBufferCount-1]=='R')//鼎天子句
			{
				InitNewConvertNewData();
				byTempSaveBuffer[byTempSaveBufferCount]='$';
				byTempSaveBufferCount++;
				byTempSaveBuffer[byTempSaveBufferCount]='R';
				byTempSaveBufferCount++;
			}

			if(byTempSaveBuffer[0]==0xB5)//要處理ublos的資料
			{

				if(ColUBlosNAVSTATUS(byTempSaveBuffer,byTempSaveBufferCount))
				{
					byTempSaveBufferCount=0;
					bStartSave=nuFALSE;
					continue;
				}

				if(ColUBlosESFMEAS(byTempSaveBuffer,byTempSaveBufferCount))
				{
					byTempSaveBufferCount=0;
					bStartSave=nuFALSE;
					continue;
				}

				if(ColUBlosESFSTATUS(byTempSaveBuffer,byTempSaveBufferCount))
				{
					byTempSaveBufferCount=0;
					bStartSave=nuFALSE;
					continue;
				}

				if(ColUBlosEKFSTATUS(byTempSaveBuffer,byTempSaveBufferCount))
				{
					byTempSaveBufferCount=0;
					bStartSave=nuFALSE;
					continue;
				}
				if(ColUBlosVersion(byTempSaveBuffer,byTempSaveBufferCount))
				{
					byTempSaveBufferCount=0;
					bStartSave=nuFALSE;
					continue;
				}
				if(ColUBlosEKF(byTempSaveBuffer,byTempSaveBufferCount))
				{
					byTempSaveBufferCount=0;
					bStartSave=nuFALSE;
					continue;
				}
				if(ColUBlosEnable(byTempSaveBuffer,byTempSaveBufferCount)) //Class=0x0A, ID=0x09, Length=68
				{
					byTempSaveBufferCount=0;
					bStartSave=nuFALSE;
					continue;
				}
			}

			if(byTempSaveBuffer[0]=='$')//NMEA類資料判斷
			{
				if(byTempSaveBufferCount==6)
				{
					lNowNEMACode=CheckNEMATable((nuCHAR *)byTempSaveBuffer,lGPSMode);
					if(lNowNEMACode==NMEAID_NONE)
					{
						byTempSaveBufferCount=0;
						bStartSave=nuFALSE;
						continue;
					}
				}

				if(byTempSaveBufferCount>9 && byTempSaveBuffer[byTempSaveBufferCount-3]=='*')
				{

					byCheckSum=0;
					for(j=1;j<byTempSaveBufferCount-3;j++)
					{
						byCheckSum=byCheckSum^byTempSaveBuffer[j];
					}

					if(lNowNEMACode==NMEAID_NONE) continue;
					byTempSaveBuffer[byTempSaveBufferCount]=NULL;

					byChecksumBuffer[0]=byTempSaveBuffer[byTempSaveBufferCount-2];
					byChecksumBuffer[1]=byTempSaveBuffer[byTempSaveBufferCount-1];
					byChecksumBuffer[2]=NULL;
					//nuSscanf(byChecksumBuffer,"%X",&bySyscheck);
					bySyscheck = nuStrtol(byChecksumBuffer, NULL, 16);
					if(byCheckSum != bySyscheck)  
					{
						if( 1 == m_nChecrSerRec )
						{
							static nuINT g_nNum = 0;
							++g_nNum;
							nuTCHAR sFilePath[256] = {0};
							nuMemset( sFilePath, 0, sizeof(sFilePath) );
							#ifndef ANDROID							
							GetModuleFileName(NULL, sFilePath, NURO_MAX_PATH);
							for(nuINT i = nuTcslen(sFilePath) - 1; i >= 0; i--)
							{
								if( sFilePath[i] == nuTEXT('\\') )
								{
									sFilePath[i+1] = nuTEXT('\0');
									break;
								}
							}
							#endif 
							nuTcscat( sFilePath, nuTEXT("SerialLog.TXT"));
							nuFILE *fpLogFile = nuTfopen(sFilePath,NURO_FS_WB);
							if ( fpLogFile == NULL )
							{
								return nuFALSE;
							}
							nuCHAR sMsg[500] = {0};
							nuSprintf( sMsg,"Serial check Error num = %d", g_nNum );
							if ( nuFwrite( sMsg, sizeof(sMsg), 1,fpLogFile ) != 1)
							{
								nuFclose( fpLogFile );
								return nuFALSE;
							}
							nuFclose( fpLogFile );
						}
						byTempSaveBufferCount=0;
						bStartSave=nuFALSE;
						continue;
					}
					else
					{
						switch(lNowNEMACode)
						{
							case NMEAID_GGA:
								nuMemset(m_SpecialCaseData.m_UBLOS.ModInfo.TXTGPGGA,NULL,GPSNMEATXTLEN);
								nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.TXTGPGGA,byTempSaveBuffer,NURO_MIN(byTempSaveBufferCount,GPSNMEATXTLEN));
								break;
							case NMEAID_GSV:
								nuMemset(m_SpecialCaseData.m_UBLOS.ModInfo.TXTGPGSV,NULL,GPSNMEATXTLEN);
								nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.TXTGPGSV,byTempSaveBuffer,NURO_MIN(byTempSaveBufferCount,GPSNMEATXTLEN));
								break;
							case NMEAID_GPVTG:
								nuMemset(m_SpecialCaseData.m_UBLOS.ModInfo.TXTGPVTG,NULL,GPSNMEATXTLEN);
								nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.TXTGPVTG,byTempSaveBuffer,NURO_MIN(byTempSaveBufferCount,GPSNMEATXTLEN));
								break;
						}
						if(ColSingleLineNEMAData((nuCHAR *)byTempSaveBuffer,byTempSaveBufferCount,lNowNEMACode,lGPSMode))	bReNum=nuTRUE;
					}

					byTempSaveBufferCount=0;
					bStartSave=nuFALSE;
					continue;
				}
			}
		}
	}
	return bReNum;
}

nuVOID CGPSDataCol::InitNewConvertNewData()
{
	bStartSave=nuTRUE;
	byTempSaveBufferCount=0;
	byCheckSum=0;
	nuMemset((nuVOID *)byChecksumBuffer,0,3);
	byChecksumBufferCount=0;
}

nuLONG CGPSDataCol::CheckNEMATable(nuCHAR* NewData,nuLONG lGPSMode)
{
	if(NewData[3]=='R' && NewData[4]=='M' && NewData[5]=='C')
	{
		if(NewData[1]=='D' && NewData[2]=='R')
		{
			return NMEAID_DRRMC;
		}
		return NMEAID_GPRMC;
	}
	if(NewData[3]=='G' && NewData[4]=='G' && NewData[5]=='A') { return NMEAID_GGA; }
	if(NewData[3]=='G' && NewData[4]=='S' && NewData[5]=='A') { return NMEAID_GSA; }
	if(NewData[3]=='G' && NewData[4]=='S' && NewData[5]=='V') { return NMEAID_GSV; }
	if(NewData[3]=='T' && NewData[4]=='X' && NewData[5]=='T') { return NMEAID_GPTXT; }
	if(NewData[3]=='V' && NewData[4]=='T' && NewData[5]=='G') { return NMEAID_GPVTG; }
	//興聯
	if(NewData[3]=='G' && NewData[4]=='D' && NewData[5]=='R') { return NMEAID_GPGDR; }
    //鼎天
	if(NewData[1]=='R' && NewData[2]=='T' && NewData[3]=='O' && NewData[4]=='E' && NewData[5]=='M') { return NMEAID_RTOEM; }
	//北斗
    if(NewData[1]=='G' && NewData[2]=='Y' && NewData[3]=='R' && NewData[4]=='O') { return NMEAID_GYRO; }
    return NMEAID_NONE;
}

nuBOOL CGPSDataCol::ColSingleLineNEMAData(nuCHAR *ItemData,nuBYTE DataLen,nuLONG lNEMACode,nuLONG lGPSMode)
{
	NUROTIME TmpTime = {0};
	nuCHAR	DotBuffer[128] = {0};
	nuBYTE	DotBufferCount=0,DotItemCount=0;
	nuLONG i=0,myDiffAng=0;
	DotBufferCount=0;
	DotItemCount=0;
	nuMemset(DotBuffer,NULL,128);
	for(i=0;i<DataLen;i++)
	{
		if(ItemData[i]==',' || ItemData[i]=='*' || i==(DataLen-1))
		{
			if(ItemData[i]==',' ||  ItemData[i]=='*')
			{
				DotBuffer[DotBufferCount]=NULL;
			}
			else
			{
				DotBuffer[DotBufferCount]=ItemData[i];
				DotBufferCount++;
				DotBuffer[DotBufferCount]=NULL;
				DotBufferCount++;
			}

			switch(lNEMACode)
			{
				case NMEAID_GGA:
					ColGGA(DotItemCount,DotBuffer,DotBufferCount);
					break;
				case NMEAID_GPRMC:
					ColRMC(DotItemCount,DotBuffer,DotBufferCount);
					break;
				case NMEAID_GSV:
					ColGSV(DotItemCount,DotBuffer,DotBufferCount);
					break;
				case NMEAID_RTOEM:
					ColRTOEM(DotItemCount,DotBuffer,DotBufferCount);
					break;
				case NMEAID_GSA:
					ColGSA(DotItemCount,DotBuffer,DotBufferCount);
					break;
				case NMEAID_GPTXT:
					ColGPTXT(DotItemCount,DotBuffer,DotBufferCount);
					break;
				case NMEAID_GPVTG:
					break;
				case NMEAID_GPGDR:
					ColGPGDR(DotItemCount,DotBuffer,DotBufferCount);
					break;
				case NMEAID_GYRO:
					ColGYRO(DotItemCount,DotBuffer,DotBufferCount);
					break;
				case NMEAID_DRRMC:
					ColDRRMC(DotItemCount,DotBuffer,DotBufferCount);
					break;
			}
			nuMemset(DotBuffer,NULL,128);
			DotItemCount++;
			DotBufferCount=0;
			continue;
		}
		DotBuffer[DotBufferCount]=ItemData[i];
		DotBufferCount++;

		if(DotBufferCount>=128-1)//確保在最後一段句子的處理是ok的
		{
			DotBufferCount=0;		
			nuMemset(DotBuffer,NULL,128);
			return nuFALSE;
		}

	}
	DotBuffer[DotBufferCount]=NULL;

	if( CheckNEMAReady(lGPSMode) )
	{
		CombineGPSandDR(lGPSMode);
		CopyNEMAToBackupBuffer();//每一個循環一次
		if (1 == m_nChinaFix )
		{
			CGpsEstimateZ::FixToChina( &m_BackData );
		}
		return nuTRUE;
	}

	return nuFALSE;
}

nuBOOL CGPSDataCol::CombineGPSandDR(nuLONG lGPSMode)
{
	static nuLONG lLastangle=-1;
	static nuLONG lLastSystemStatus=DATA_TYPE_GPS;
	static nuLONG lKeepStatusCount=0;

	
	if(g_hasDRSystem==0)//無DR
	{
		m_SpecialCaseData.l_DRMode = FINDDR_NO;
		return nuFALSE;
	}
	else
	{
		if(m_SpecialCaseData.l_DRMode == FINDDR_NO )
		{
			m_SpecialCaseData.l_DRMode = FINDDR_GENERAL;
		}
	}

	nuMemset(&stuGyro,NULL,sizeof(GYROSTATUS));

	//先確認不同的DR模組 進行DR資料的採集
	switch(m_SpecialCaseData.l_DRMode)
	{
		case FINDDR_GENERAL:
			stuGyro.lSystemStatus	=	m_FirstHandData.nStatus;
			stuGyro.bAngleInit		=	nuTRUE;
			stuGyro.bAngleInput		=	nuTRUE;
			stuGyro.bSpeedInit		=	nuTRUE;
			stuGyro.bSpeedInput		=	nuTRUE;
			stuGyro.lcalibstatus	=	nuTRUE;
			if(lLastangle==-1)
			{
				lLastangle=m_FirstHandData.nAngle;
			}
			stuGyro.lDeltaAngle		=	(m_FirstHandData.nAngle-lLastangle)*EFFECTBASE;//新減舊 可以得出差異值
			lLastangle=m_FirstHandData.nAngle;
			stuGyro.lDeltaDis		=	(m_FirstHandData.nSpeed*EFFECTBASE*10+18)/36;
			break;
		case FINDDR_EL:
			switch(m_SpecialCaseData.m_EL.sELGyroStatus)
			{
				case 'D':
				case 'L':
					stuGyro.lSystemStatus	=	DATA_TYPE_GYRO;
					stuGyro.bAngleInit		=	nuTRUE;
					stuGyro.bAngleInput		=	nuTRUE;
					stuGyro.bSpeedInit		=	nuTRUE;
					stuGyro.bSpeedInput		=	nuTRUE;
					stuGyro.lcalibstatus	=	nuTRUE;
					if(lLastangle==-1)
					{
						lLastangle=m_SpecialCaseData.m_EL.nAngle;
					}
					stuGyro.lDeltaAngle		=	(m_SpecialCaseData.m_EL.nAngle-lLastangle)*EFFECTBASE;//新減舊 可以得出差異值
					lLastangle=m_SpecialCaseData.m_EL.nAngle;
					stuGyro.lDeltaDis		=	(m_SpecialCaseData.m_EL.nSpeed*EFFECTBASE*10+18)/36;
					break;
				case 'G':
					stuGyro.lSystemStatus	=	DATA_TYPE_GPS;
					stuGyro.bAngleInit		=	nuTRUE;
					stuGyro.bAngleInput		=	nuTRUE;
					stuGyro.bSpeedInit		=	nuTRUE;
					stuGyro.bSpeedInput		=	nuTRUE;
					stuGyro.lcalibstatus	=	nuTRUE;
					if(lLastangle==-1)
					{
						lLastangle=m_FirstHandData.nAngle;
					}
					stuGyro.lDeltaAngle		=	(m_FirstHandData.nAngle-lLastangle)*EFFECTBASE;//新減舊 可以得出差異值
					lLastangle=m_FirstHandData.nAngle;
					stuGyro.lDeltaDis		=	(m_FirstHandData.nSpeed*EFFECTBASE*10+18)/36;
					break;
				default:
					stuGyro.lSystemStatus	=	DATA_TYPE_INVALID;
					break;
			}
			break;
		case FINDDR_ROYALTEK://鼎天的機制 每秒角度差與實際的角度差有1~5度的差距 因此改以NMEA來處理
			stuGyro.lSystemStatus	=	m_SpecialCaseData.m_ROYALTEK.lSystemStatus;
			stuGyro.bAngleInit		=	m_SpecialCaseData.m_ROYALTEK.bAngleInit;
			stuGyro.bAngleInput		=	m_SpecialCaseData.m_ROYALTEK.bAngleInput;
			stuGyro.bSpeedInit		=	m_SpecialCaseData.m_ROYALTEK.bSpeedInit;
			stuGyro.bSpeedInput		=	m_SpecialCaseData.m_ROYALTEK.bSpeedInput;
			stuGyro.lcalibstatus	=	((m_SpecialCaseData.m_ROYALTEK.bSpeedInput==nuTRUE && m_SpecialCaseData.m_ROYALTEK.bAngleInput==nuTRUE)?nuTRUE:nuFALSE);
			if(lLastangle==-1)
			{
				lLastangle=m_FirstHandData.nAngle;
			}
			stuGyro.lDeltaAngle		=	(m_FirstHandData.nAngle-lLastangle)*EFFECTBASE;//新減舊 可以得出差異值
			lLastangle=m_FirstHandData.nAngle;
			stuGyro.lDeltaDis		=	m_SpecialCaseData.m_ROYALTEK.lDeltaDis;
			break;
		case FINDDR_UBLOS:
			stuGyro.lSystemStatus	=	((m_SpecialCaseData.m_UBLOS.lSystemStatus!=DATA_TYPE_INVALID)?m_SpecialCaseData.m_UBLOS.lSystemStatus:m_FirstHandData.nStatus);
			stuGyro.bAngleInit		=	m_SpecialCaseData.m_UBLOS.bAngleInit;
			stuGyro.bAngleInput		=	m_SpecialCaseData.m_UBLOS.bAngleInput;
			stuGyro.bSpeedInit		=	m_SpecialCaseData.m_UBLOS.bSpeedInit;
			stuGyro.bSpeedInput		=	m_SpecialCaseData.m_UBLOS.bSpeedInput;
			stuGyro.lcalibstatus	=	m_SpecialCaseData.m_UBLOS.lcalibstatus;
			stuGyro.lDeltaAngle		=	m_SpecialCaseData.m_UBLOS.lDeltaAngle;//新減舊 可以得出差異值
			stuGyro.lDeltaDis		=	(m_FirstHandData.nSpeed*EFFECTBASE*10+18)/36;
			break;
		case FINDDR_SMART:
			stuGyro.lSystemStatus	=	m_SpecialCaseData.m_SMART.lSystemStatus;
			stuGyro.bAngleInit		=	nuTRUE;
			stuGyro.bAngleInput		=	nuTRUE;
			stuGyro.bSpeedInit		=	nuTRUE;
			stuGyro.bSpeedInput		=	nuTRUE;
			stuGyro.lcalibstatus	=	nuTRUE;
			if(lLastangle==-1)
			{
				lLastangle=m_SpecialCaseData.m_SMART.nAngle;
			}
			stuGyro.lDeltaAngle		=	(m_SpecialCaseData.m_SMART.nAngle-lLastangle)*EFFECTBASE;//新減舊 可以得出差異值
			lLastangle=m_SpecialCaseData.m_SMART.nAngle;
			stuGyro.lDeltaDis		=	(m_SpecialCaseData.m_SMART.nOBD_Speed*EFFECTBASE*10+18)/36;
			break;
		case FINDDR_SIRFBINARY:
			stuGyro.lSystemStatus	=	m_FirstHandData.nStatus;
			stuGyro.bAngleInit		=	m_SpecialCaseData.m_SIRFBINARY.bAngleInit;
			stuGyro.bAngleInput		=	m_SpecialCaseData.m_SIRFBINARY.bAngleInput;
			stuGyro.bSpeedInit		=	m_SpecialCaseData.m_SIRFBINARY.bSpeedInit;
			stuGyro.bSpeedInput		=	m_SpecialCaseData.m_SIRFBINARY.bSpeedInput;
			stuGyro.lcalibstatus	=	((m_SpecialCaseData.m_SIRFBINARY.bAngleInput && m_SpecialCaseData.m_SIRFBINARY.bSpeedInput)?nuTRUE:nuFALSE);
			if(lLastangle==-1)
			{
				lLastangle=m_FirstHandData.nAngle;
			}
			stuGyro.lDeltaAngle		=	(m_FirstHandData.nAngle-lLastangle)*EFFECTBASE;//新減舊 可以得出差異值
			lLastangle=m_FirstHandData.nAngle;
			stuGyro.lDeltaDis		=	m_SpecialCaseData.m_SIRFBINARY.nOBD_Speed;
			break;
		case FINDDR_CN_DR:
			m_SpecialCaseData.m_EL.nAngle	=	m_FirstHandData.nAngle;
			m_SpecialCaseData.m_EL.nSpeed	=	m_FirstHandData.nSpeed;

			stuGyro.lSystemStatus	=	((m_FirstHandData.nStatus==DATA_TYPE_GPS)?DATA_TYPE_GPS:DATA_TYPE_GYRO);
			stuGyro.bAngleInit		=	nuTRUE;
			stuGyro.bAngleInput		=	nuTRUE;
			stuGyro.bSpeedInit		=	nuTRUE;
			stuGyro.bSpeedInput		=	nuTRUE;
			stuGyro.lcalibstatus	=	nuTRUE;
			if(lLastangle==-1)
			{
				lLastangle=m_SpecialCaseData.m_EL.nAngle;
			}
			stuGyro.lDeltaAngle		=	(m_SpecialCaseData.m_EL.nAngle-lLastangle)*EFFECTBASE;//新減舊 可以得出差異值
			lLastangle=m_SpecialCaseData.m_EL.nAngle;
			stuGyro.lDeltaDis		=	(m_SpecialCaseData.m_EL.nSpeed*EFFECTBASE*10+18)/36;
			break;
		default:
		case FINDDR_NO:
			stuGyro.lSystemStatus	=	DATA_TYPE_INVALID;
			break;
	}

	//下面開始添加一些我們平常用來進行特別處理的東西
	if( stuGyro.lSystemStatus==DATA_TYPE_INVALID ) return nuFALSE;
	if( stuGyro.lSystemStatus==DATA_TYPE_GYRO ) lKeepStatusCount=g_TunelDelay;

	//狀態切換的時候,短時間使用DR狀態延續,並依照角度的變化狀況,強制修正角度
	if( lLastSystemStatus != stuGyro.lSystemStatus && lKeepStatusCount>0)
	{
		lKeepStatusCount--;
		stuGyro.lSystemStatus=DATA_TYPE_GYRO;
		if(stuGyro.lDeltaAngle>5*EFFECTBASE) stuGyro.lDeltaAngle=0;
	}

	//低速不允許角度變化量的存在(這個部分日後有可能會有異動)
	if(stuGyro.lDeltaDis<EFFECTBASE/5)
	{
		stuGyro.lDeltaAngle=0;
	}

	lLastSystemStatus=stuGyro.lSystemStatus;
	return nuTRUE;
}



nuBOOL CGPSDataCol::Col_ELeadData(const GYROGPSDATA_EL* pEL_GyroData)
{
	static nuLONG lLastangle=-1;
	NUROTIME TmpTime = {0};
	nuCHAR	DotBuffer[128] = {0};
	nuBYTE	DotBufferCount=0,DotItemCount=0;
	nuLONG i=0,myDiffAng=0;
	DotBufferCount=0;
	DotItemCount=0;
	nuMemset(DotBuffer,NULL,128);

	bFindGGA	=	nuTRUE;
	bFindRMC	=	nuTRUE;
	bFindGSV	=	nuTRUE;

	//ColGPGGA();
	m_FirstHandData.nAntennaHeight	=	(nuLONG)(pEL_GyroData->dGpsAltitude*1);	//
	m_FirstHandData.nActiveStarNum	=	0;

	//ColGPRMC();
	m_FirstHandData.nLatitude  = (nuLONG) (pEL_GyroData->dLatitude*100000.0);
	m_FirstHandData.nLongitude = (nuLONG) (pEL_GyroData->dLongitude*100000.0);
	
	m_FirstHandData.nAngle			=	(nuSHORT)(pEL_GyroData->dCourse*1);
	m_FirstHandData.nAngle			=	( (90 - m_FirstHandData.nAngle + 360 ) % 360);

	m_FirstHandData.nSpeed			=	(nuSHORT)(pEL_GyroData->dSpeed);
	m_FirstHandData.nTime.nYear		=	pEL_GyroData->btYear;
	m_FirstHandData.nTime.nMonth	=	pEL_GyroData->btMonth;
	m_FirstHandData.nTime.nDay		=	pEL_GyroData->btDay;
	m_FirstHandData.nTime.nHour		=	pEL_GyroData->btHour;
	m_FirstHandData.nTime.nMinute	=	pEL_GyroData->btMinute;
	m_FirstHandData.nTime.nSecond	=	pEL_GyroData->btSecond;

	m_FirstHandData.nStatus_RMC		=	((pEL_GyroData->btStatus=='A')	?DATA_TYPE_GPS:DATA_TYPE_INVALID);
	m_FirstHandData.nStatus		=	((pEL_GyroData->blGyroMsg)	?DATA_TYPE_GYRO:DATA_TYPE_GPS);
	m_FirstHandData.nAntennaHeight	=	(nuSHORT)(pEL_GyroData->dGpsAltitude);	//
	m_FirstHandData.nActiveStarNum	=	0;
	m_FirstHandData.nStarNumUsed	=	0;	//
	switch(pEL_GyroData->nSlope)
	{
		case 'U':
			m_FirstHandData.nSlopeStatus = GPS_SLOPE_FASTUP;
			break;
		case 'D':
			m_FirstHandData.nSlopeStatus = GPS_SLOPE_FASTDOWN;
			break;
		case 'F':
		default:
			m_FirstHandData.nSlopeStatus = GPS_SLOPE_NO;
			break;
	}

	g_bHaveNewData = nuTRUE;
	m_GSAIndex=0;
	for(nuINT i=0;i<pEL_GyroData->nTotalSatsInView;i++)
	{
		if(MAX_STAR_NUMBER>m_FirstHandData.nStarNumUsed)
		{
			if((pEL_GyroData->starInfo[i].nPRN>0))
			{
				m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nAngle_elevation	=	pEL_GyroData->starInfo[i].nElevation;
				m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nAngle_position		=	pEL_GyroData->starInfo[i].nAzimuth;
				m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nSignalNoiseRatio	=	pEL_GyroData->starInfo[i].nSigQuality;
				m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nStarID				=	pEL_GyroData->starInfo[i].nPRN;
				m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].b_GPSOK			=	pEL_GyroData->starInfo[i].bActived;
				m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].b_GPSUsed			=	pEL_GyroData->starInfo[i].bActived;
				m_FirstHandData.nActiveStarNum++;
				m_FirstHandData.nStarNumUsed++;
			}
		}
	}

	//時間進行時區處理
	bCheckTimeForZone( &(m_FirstHandData.nTime), m_nZoneTime );
	m_SpecialCaseData.l_DRMode=FINDDR_GENERAL;
	CombineGPSandDR(GPS_OPEN_EL);
	CopyNEMAToBackupBuffer();//每一個循環一次

	
	if (1 == m_nChinaFix)
	{
		CGpsEstimateZ::FixToChina( &m_BackData );
	}

	return nuTRUE;
}
nuBOOL CGPSDataCol::CheckNEMAReady(nuLONG lGPSMode)
{
	switch(lGPSMode)
	{
		case GPS_OPEN_SIMPNEMA:
		case GPS_OPEN_FILE:
		case GPS_OPEN_COMM:
		case GPS_OPEN_LINUX:
		case GPS_OPEN_PS:
			if(bFindRMC)//使用最單純的判斷 只要有GPRMC就可以
			{
				return nuTRUE;
			}
			break;
		case GPS_OPEN_WOOJOY:
		case GPS_OPEN_SDC_3DR:
		case GPS_OPEN_JNI:
		case GPS_OPEN_JNI_2:
		case GPS_OPEN_NORMALMODE:
		case GPS_OPEN_PANA_LEA6R:
		case GPS_OPEN_UCOS:
		case GPS_OPEN_ECOS:
		case GPS_OPEN_ROYALTEK_3DR:
		case GPS_OPEN_ROYALTEK_ANDROID:
		case GPS_OPEN_HV:
		case GPS_OPEN_POWERNAVI:
			if(bFindRMC && bFindGGA)//使用GPRMC跟GPGGA判斷
			{
				return nuTRUE;
			}
			break;
	}
	return nuFALSE;
}


nuVOID CGPSDataCol::CopyNEMAToBackupBuffer()
{
	static nuINT KeepGSVCount=0;
	static nuLONG FixCount=0;
	static nuLONG l_Inipass=0;
	static nuLONG l_AngleStableCount=0;
	static nuLONG nowLen = 0,DisX=0,DisY=0;

	nuDOUBLE TmpCalValueDouble=0.0;
	nuLONG LatEffect = 0;
	NUROPOINT temppt = {0};
	nuLONG Sht_X = 0,Sht_Y = 0;
	//進行資料備份
	//資料的備份分成
	//必要性備份(如星歷圖/時間)
		//星歷圖
		if(!bFindGSV)
		{
			if(KeepGSVCount<10)
			{
				m_FirstHandData.nStarNumUsed=m_BackData.nStarNumUsed;
				nuMemcpy( m_FirstHandData.pStarList, m_SecondTimeData.pStarList, sizeof(STARDATA)*MAX_STAR_NUMBER );
				KeepGSVCount++;
			}
		}
		else
		{
			KeepGSVCount=0;
		}

	//特例備份(如GPS模組資訊)
		//9100 傳送模組資訊
		if(b_updateModInfo==nuTRUE)
		{
			nuMemcpy(&m_GpsModInfo,&(m_SpecialCaseData.m_UBLOS.ModInfo),sizeof(GPSMODINFO));
		}

	//混合式備份(如角度/座標/拉回機制的處理)
	//DR跟回校在這裡處理好 會比較適合
//		if(m_SpecialCaseData.l_DRMode!=FINDDR_NO && m_FirstHandData.nStatus==DATA_TYPE_INVALID)
//			m_FirstHandData.nStatus	= DATA_TYPE_GYRO;
		if(m_SpecialCaseData.l_DRMode!=FINDDR_NO )
		{
			m_FirstHandData.nStatus=stuGyro.lSystemStatus;
			if(m_FirstHandData.nStatus==DATA_TYPE_INVALID)
				m_FirstHandData.nStatus	= DATA_TYPE_GYRO;
		}

		nuMemcpy( &m_GPSRawInfo, &m_FirstHandData, sizeof(GPSDATA));

		if(l_Inipass<DEF_INIPASS)//初始化階段
		{
			nuMemcpy( &m_SecondTimeData, &m_FirstHandData, sizeof(GPSDATA));
			l_Inipass++;
		}
		else
		{
			//when DR State should Mix DR Data
			if(stuGyro.lSystemStatus==DATA_TYPE_GYRO)
			{
				m_FirstHandData.nLongitude		=	m_SecondTimeData.nLongitude;//修正位置
				m_FirstHandData.nLatitude		=	m_SecondTimeData.nLatitude;//修正位置
				m_FirstHandData.nAngle			=	m_SecondTimeData.nAngle;//修正位置
				m_FirstHandData.nStatus			=	DATA_TYPE_GYRO;
				nuMemcpy( &m_SecondTimeData, &m_FirstHandData, sizeof(GPSDATA));

				//持續維持穩定的方向 才可以拿道路拉回的資料進行校正動作
				if( (!(AUTOFIXANGLE*EFFECTBASE<NURO_ABS(stuGyro.lDeltaAngle) && NURO_ABS(stuGyro.lDeltaAngle)<(360-AUTOFIXANGLE)*EFFECTBASE)) || IsTunel )
				{//有大型轉彎(超過3度)
					l_AngleStableCount++;
					if(l_AngleStableCount>2 && (l_FixAngleStableCount>2||IsTunel))
					{
						m_SecondTimeData.nAngle			=	g_FixedInfo.Angle/EFFECTBASE;//替換角度
						m_SecondTimeData.nLongitude		+=	g_FixedInfo.nRoadx;//修正位置
						m_SecondTimeData.nLatitude		+=	g_FixedInfo.nRoady;//修正位置
						g_FixedInfo.nRoadx		=	0;
						g_FixedInfo.nRoady		=	0;
						l_AngleStableCount=0;
						l_FixAngleStableCount=0;
						IsTunel=nuFALSE;
					}
				}
				else
				{
					l_AngleStableCount=0;
					l_FixAngleStableCount=0;
				}

				m_SecondTimeData.nAngle += (nuSHORT)(stuGyro.lDeltaAngle/EFFECTBASE) ;
				if(m_SecondTimeData.nAngle<0)					m_SecondTimeData.nAngle+=360;
				if(m_SecondTimeData.nAngle>360*EFFECTBASE)		m_SecondTimeData.nAngle-=360;

				TmpCalValueDouble =	((nuCos(m_SecondTimeData.nLatitude/DEGBASE)*EFFECTBASE)*((m_SecondTimeData.nLatitude*10/DEGBASE)%10) + (nuCos(m_SecondTimeData.nLatitude/DEGBASE+1)*EFFECTBASE)*(10-((m_SecondTimeData.nLatitude*10/DEGBASE)%10)) )/10;
				LatEffect	=	(nuLONG)TmpCalValueDouble;
				nowLen		=	(stuGyro.lDeltaDis*100)/100; //以比例來還原情況
				if( nowLen > 0 ){
					if(LatEffect==0)
					{
						DisX += (nuLONG)(nowLen*nuCos(m_SecondTimeData.nAngle));
						DisY += nowLen*nuSin(m_SecondTimeData.nAngle);
						m_SecondTimeData.nLongitude += DisX/LONEFFECTCONST;
						m_SecondTimeData.nLatitude += DisY/LATEFFECTCONST;
						DisX = DisX%LONEFFECTCONST;
						DisY = DisY%LATEFFECTCONST;
					}
					else
					{
						DisX += nowLen*nuCos(m_SecondTimeData.nAngle)*EFFECTBASE;
						DisY += nowLen*nuSin(m_SecondTimeData.nAngle);
						m_SecondTimeData.nLongitude += DisX/(LatEffect*LONEFFECTCONST);
						m_SecondTimeData.nLatitude += DisY/LATEFFECTCONST;
						DisX = DisX%(LatEffect*LONEFFECTCONST);
						DisY = DisY%LATEFFECTCONST;
					}
				}
			}
			else
			{
				nuMemcpy( &m_SecondTimeData, &m_FirstHandData, sizeof(GPSDATA));
			}
		}
		nuMemcpy( &m_BackData, &m_SecondTimeData, sizeof(GPSDATA));

		bFindRMC=nuFALSE;
		bFindGGA=nuFALSE;
		bFindGSV=nuFALSE;
		nuMemset( &m_FirstHandData, NULL, sizeof(GPSDATA) );
		nuMemset( &m_SpecialCaseData, NULL, sizeof(SPECIALCASEDATA) );
		//9100 傳送模組資訊
		if(b_updateModInfo==nuTRUE)
		{
			nuMemcpy(&(m_SpecialCaseData.m_UBLOS.ModInfo),&m_GpsModInfo,sizeof(GPSMODINFO));
			b_updateModInfo=nuFALSE;
		}

}

nuVOID CGPSDataCol::GPSFeedBack(const FEEDBACKDATA* pFeedBackInfo)
{
	//讓
	nuINT i_NowAngle = 0;

	if(	(NURO_ABS(g_FixedInfo.nOldx-pFeedBackInfo->ptFixedOnRoad.x)+NURO_ABS(g_FixedInfo.nOldy-pFeedBackInfo->ptFixedOnRoad.y))<5) return;
	i_NowAngle=nulAtan2(pFeedBackInfo->nRoadDy,pFeedBackInfo->nRoadDx)*EFFECTBASE;
	if(!(AUTOFIXANGLE*EFFECTBASE<NURO_ABS(i_NowAngle-g_FixedInfo.Angle) && NURO_ABS(i_NowAngle-g_FixedInfo.Angle)<(360-AUTOFIXANGLE)*EFFECTBASE ))//紀錄新舊角度差異
	{
		l_FixAngleStableCount++;
	}
	g_FixedInfo.nRoadx	=	pFeedBackInfo->ptFixedOnRoad.x - pFeedBackInfo->ptRaw.x;	
	g_FixedInfo.nRoady	=	pFeedBackInfo->ptFixedOnRoad.y - pFeedBackInfo->ptRaw.y;
	g_FixedInfo.ndx		=	pFeedBackInfo->nRoadDx;
	g_FixedInfo.ndy		=	pFeedBackInfo->nRoadDy;
	g_FixedInfo.nOldx	=	pFeedBackInfo->ptFixedOnRoad.x;
	g_FixedInfo.nOldy	=	pFeedBackInfo->ptFixedOnRoad.y;
	g_FixedInfo.nTime	=	pFeedBackInfo->nDisFixedToRoad;
	g_FixedInfo.Angle	=	i_NowAngle;
}
nuVOID CGPSDataCol::GPSFeedBack_Speed(nuLONG nRoadx, nuLONG nRoady, nuSHORT dx, nuSHORT dy, nuSHORT nSpeed, nuINT nTime)
{
	if(nSpeed==0x0E)
	{
		IsTunel=nuTRUE;
	}
	else
	{
		IsTunel=nuFALSE;
	}
}

nuBOOL CGPSDataCol::bCheckTimeForZone(NUROTIME *nuTIME, nuINT iZoneTime)
{
	if(nuTIME->nYear<=0)
	{
		return nuFALSE;
	}
	nuINT iDayType = 0;	
	nuINT monNum[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	nuINT TmpHour = 0;
	TmpHour = nuTIME->nHour + iZoneTime;
	
	if (24 <= TmpHour)//加時區超過24hr 進一日
	{
		nuTIME->nHour = (nuTIME->nHour + iZoneTime) % 24;
		iDayType = 1;
	}
	else if (0 > TmpHour)//加時區低於0hr 退一日
	{
		nuTIME->nHour = 24 + TmpHour;
		iDayType = -1;
	}
	else//沒有進退日期的問題
	{
		nuTIME->nHour = nuTIME->nHour + iZoneTime;
		iDayType = 0;
		return nuTRUE;
	}

	//閏年的處理
	if (((nuTIME->nYear % 4 == 0) && (nuTIME->nYear % 100 != 0)) || (nuTIME->nYear % 400 == 0))
	{
		monNum[1] = 29;
	}
	else
	{
		monNum[1] = 28;
	}
	
	if(iDayType>0)
	{
		if( monNum[nuTIME->nMonth-1] < (nuTIME->nDay + iDayType) )
		{
			nuTIME->nDay = 1;
			nuTIME->nMonth=nuTIME->nMonth+1;
			if(nuTIME->nMonth>12)
			{
				nuTIME->nMonth=1;
				nuTIME->nYear=nuTIME->nYear+1;
			}
		}
		else
		{
			nuTIME->nDay = nuTIME->nDay + iDayType;
		}
	}
	else if(iDayType<0)
	{
		if( 0 >= (nuTIME->nDay + iDayType) )
		{
			nuTIME->nMonth=nuTIME->nMonth-1;
			if(nuTIME->nMonth<=0)
			{
				nuTIME->nMonth=12;			
				nuTIME->nYear=nuTIME->nYear-1;
			}
			nuTIME->nDay=monNum[nuTIME->nMonth-1];
		}
		else
		{
			nuTIME->nDay = nuTIME->nDay + iDayType;
		}
	}
	return nuTRUE;
}

//獨立處理項
//Panasonic SIRF Binary模組
	nuINT CGPSDataCol::ColPanaBinary(nuBYTE *NewData,nuLONG DataLen,nuBYTE &lGPSMode)
	{
		static nuLONG	l_SIRFDataInCount=0;
		//將新收與舊餘加在一起
		if(RcvLen+DataLen>=RCVCOUNT)
		{
			RcvLen=0;
			memset(Rcv,NULL,RCVCOUNT);
			return 0;
		}
		memcpy(Rcv+RcvLen,NewData,DataLen);
		RcvLen=RcvLen+DataLen;

		//切掉前段至b0 b3 a0 a2
		if (!(Rcv[0]==0xb0 && Rcv[1]==0xb3 && Rcv[2]==0xa0 && Rcv[3]==0xa2))
		{
			nuINT first=0;
			for(nuINT i=3;i<RcvLen;i++)
			{
				if (Rcv[i-3]==0xb0 && Rcv[i-2]==0xb3 && Rcv[i-1]==0xa0 && Rcv[i]==0xa2)
				{
						first=i-3;
						RcvLen=RcvLen-first;
						break;
				}
			}
			if (first>0) 
			{
				l_SIRFDataInCount=0;
				nuBYTE tmp[RCVCOUNT];
				memset(tmp,NULL,RCVCOUNT);
				memcpy(tmp,Rcv+first,RcvLen);
				memcpy(Rcv,tmp,RCVCOUNT);
			}
			else
			{
				if(DataLen>10)
				{
					l_SIRFDataInCount++;
					if(l_SIRFDataInCount>5)
					{
						lGPSMode=GPS_OPEN_NORMALMODE;//表示一直都沒有接收到SIRFBINARY的資料,主動將Mode轉換為萬用模式
					}
				}
				memset(Rcv,NULL,sizeof(Rcv));
				RcvLen=0;
				return 0;
			}
		}

		nuINT i=0,l_start=0,l_end=0;
		nuLONG checksum=0,checksum2=0;

		for(i=4;i<RcvLen;i++)
		{
			if (Rcv[i-3]==0xb0 && Rcv[i-2]==0xb3 && Rcv[i-1]==0xa0 && Rcv[i]==0xa2 && (((i + 1) < RcvLen) && Rcv[i+1]==0x00))
			{
				l_end=i-3;
				nuINT PackLen=NumComb(0,0,Rcv[l_start+4],Rcv[l_start+5]);
				
				checksum=0;
				//判斷checksum
				for(nuINT kkk=0;kkk<PackLen;kkk++)
				{
					checksum += Rcv[l_start+6+kkk];
				}
				checksum = checksum & 0x7FFF;
				checksum2= NumComb(0,0,Rcv[l_start+5+PackLen+1],Rcv[l_start+5+PackLen+2]);
				if(checksum!=checksum2)
				{
					l_start=i-3;
					continue;
				}
				if (PackLen==0x5b && Rcv[l_start+6]==0x29) //MessageID 41
				{
					m_FirstHandData.nTime.nYear		= NumComb(0,0,Rcv[l_start+17],Rcv[l_start+18]);
					m_FirstHandData.nTime.nMonth	= Rcv[l_start+19];
					m_FirstHandData.nTime.nDay		= Rcv[l_start+20];
					m_FirstHandData.nTime.nHour		= Rcv[l_start+21];
					m_FirstHandData.nTime.nMinute	= Rcv[l_start+22];
					m_FirstHandData.nTime.nSecond	= (NumComb(0,0,Rcv[l_start+23],Rcv[l_start+24]))/1000;
					//時間進行時區處理
					bCheckTimeForZone( &(m_FirstHandData.nTime), m_nZoneTime );
					m_FirstHandData.nLatitude		=	NumComb(Rcv[l_start+29],Rcv[l_start+30],Rcv[l_start+31],Rcv[l_start+32])/100;
					m_FirstHandData.nLongitude		=	NumComb(Rcv[l_start+33],Rcv[l_start+34],Rcv[l_start+35],Rcv[l_start+36])/100;
					m_FirstHandData.nAntennaHeight	=	NumComb(Rcv[l_start+41],Rcv[l_start+42],Rcv[l_start+43],Rcv[l_start+44])/100;
					static nuINT oldangle=-1;

					m_FirstHandData.nAngle			=	NumComb(0,0,Rcv[l_start+48],Rcv[l_start+49])/100;
					m_FirstHandData.nAngle			=	(360-(m_FirstHandData.nAngle-90))%360;
					m_FirstHandData.nSpeed			=	NumComb(0,0,Rcv[l_start+46],Rcv[l_start+47])*0.036;
					m_FirstHandData.nStatus		=	(((Rcv[l_start+10]%8)==4)?DATA_TYPE_GPS:DATA_TYPE_GYRO);
					if	((Rcv[l_start+10]%8) == 0)	m_FirstHandData.nStatus=DATA_TYPE_INVALID;
					
					if(((Rcv[l_start+10]%8)==4) && (Rcv[l_start+9]-(Rcv[l_start+9]%64) ==128))
					{
						RcvLen=0;
						memset(Rcv,NULL,sizeof(Rcv));
						return -1; // for Reset DR use
					}

					static nuLONG drDis=0, gpscount=0;
					static nuINT x,y,sumx=0,sumy=0,modx=0,mody=0,oldx=0,oldy=0;
					static nuINT totx=0,toty=0;
					static const nuINT efct_x = 1000;//11130;// why 10倍?
					static const nuINT efct_y = 1000;//11090;

					switch(Rcv[l_start+10]%8)
					{
						case 0://no navigation fix
						case 1://1-SV KF solution
						case 2://2-SV KF solution
						case 3://3-SV KF solution
							//都還在定位中
							m_FirstHandData.nStatus_RMC=DATA_TYPE_INVALID;
							m_FirstHandData.nStatus=DATA_TYPE_INVALID;
							break;
						case 4://4 or more SV KF solution
						case 5://2-D least-squares solution
						case 6://3-D least-squares solution
							//已定位
							m_FirstHandData.nStatus_RMC=DATA_TYPE_GPS;
							m_FirstHandData.nStatus=DATA_TYPE_GPS;
							break;
						case 7://DR
							m_FirstHandData.nStatus_RMC=DATA_TYPE_GPS;
							m_FirstHandData.nStatus=DATA_TYPE_GYRO;
							break;
					}
					nuMemset(m_FirstHandData.SIRFBINARY41INFO,NULL,sizeof(nuBYTE)*100);
					nuMemcpy(m_FirstHandData.SIRFBINARY41INFO,&(Rcv[l_start+6]),PackLen);
					g_bHaveNewData = nuTRUE;

					//時間進行時區處理
					CombineGPSandDR(GPS_OPEN_EL);
					CopyNEMAToBackupBuffer();//每一個循環一次
					if (1 == m_nChinaFix )
					{
						CGpsEstimateZ::FixToChina( &m_BackData );
					}
					nuSleep(200);
				}
				if (PackLen==0xBC && Rcv[l_start+6]==0x04)//ID 4的句子,可以用來知道衛星的接收狀態
				{
					nuINT chans=Rcv[l_start+13];//channel count
					for(nuINT j=0;j<chans;j++)
					{
						m_FirstHandData.pStarList[j].nStarID			=	Rcv[l_start+14+j*15+0];
						m_FirstHandData.pStarList[j].nAngle_position	=	((Rcv[l_start+14+j*15+1])*3/2)%360;
						m_FirstHandData.pStarList[j].nAngle_elevation	=	((Rcv[l_start+14+j*15+2])*3/2)%90;
						m_FirstHandData.pStarList[j].nSignalNoiseRatio	=	Rcv[l_start+14+j*15+14];
						m_FirstHandData.nStarNumUsed=chans;

						m_FirstHandData.pStarList[j].b_GPSOK=nuFALSE;
						m_FirstHandData.pStarList[j].b_GPSUsed=nuFALSE;

						if(m_FirstHandData.pStarList[j].nSignalNoiseRatio>0)
						{
							for(nuINT k=0;k<m_GSAIndex;k++)
							{
								if(m_FirstHandData.pStarList[j].nStarID==GSAID[k])
								{
									m_FirstHandData.pStarList[j].b_GPSUsed=nuTRUE;
								}
							}
							m_FirstHandData.pStarList[j].b_GPSOK=nuTRUE;
						}
					}
				}				
				if (PackLen==0x29 && Rcv[l_start+6]==0x02)//ID 2的句子,可以用來決定模組使用的衛星
				{
					m_GSAIndex=0;
					nuINT SVCount=Rcv[l_start+34];//channel count
					m_FirstHandData.nActiveStarNum=SVCount;
					for(nuINT j=0;j<SVCount;j++)
					{
						GSAID[m_GSAIndex]=Rcv[l_start+35+j];
						m_GSAIndex++;
					}
				}				
				if (Rcv[l_start+6]==0xAC && Rcv[l_start+7]==0x09)//ID 172 ,Sub 9的句子,可以用來取得車速線
				{
					m_SpecialCaseData.l_DRMode = FINDDR_SIRFBINARY;
					m_FirstHandData.nOdometerSpeed = NumComb(0,0,Rcv[l_start+17],Rcv[l_start+18]);
					m_SpecialCaseData.m_SIRFBINARY.nOBD_Speed = NumComb(0,0,Rcv[l_start+17],Rcv[l_start+18]);
					m_SpecialCaseData.m_SIRFBINARY.nReverseSignal = NumComb(0,0,Rcv[l_start+10],Rcv[l_start+11]);
				}				
				if (Rcv[l_start+6]==0x30 && Rcv[l_start+7]==0x08)//ID 48 ,Sub 8的句子,可以用來取得倒車狀態
				{
				}				
				if (Rcv[l_start+6]==0x30 && Rcv[l_start+7]==0x01)//ID 48 ,Sub 1的句子,可以用來取得倒車狀態
				{
					m_SpecialCaseData.m_SIRFBINARY.bAngleInit = Rcv[l_start+11]%8;//Gyro bias Cal
					m_SpecialCaseData.m_SIRFBINARY.bSpeedInit = Rcv[l_start+11]%8;//Gyro bias Cal
					m_SpecialCaseData.m_SIRFBINARY.bAngleInput = Rcv[l_start+12]%8;//Gyro Scale Factor Cal
					m_SpecialCaseData.m_SIRFBINARY.bSpeedInput = Rcv[l_start+17]%8;//Speed Scale Factor Cal
				}				
				l_start=i-3;
			}
		}

		//剩餘的留後面
		nuBYTE tmp[RCVCOUNT];
		memset(tmp,NULL,sizeof(tmp));
		memcpy(tmp,Rcv+l_start,RcvLen-l_start);
		RcvLen = RcvLen-l_start;
		memcpy(Rcv,tmp,sizeof(tmp));

		return 0;
	}


//標準NMEA
	nuVOID CGPSDataCol::ColRMC(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
	{
		nuCHAR* lpEnd = NULL;
		nuCHAR tempbuffer[3] = {0};
		NUROTIME TmpTime = {0};
		switch(Index)
		{
			case 1://hhmmrr
				if(DataLen<6) break;
				nuMemset(tempbuffer,NULL,3);
				nuMemcpy(tempbuffer,&(ItemData[0]),2);
				m_FirstHandData.nTime.nHour	= nuStrtol(tempbuffer, &lpEnd,10);
				nuMemcpy(tempbuffer,&(ItemData[2]),2);
				m_FirstHandData.nTime.nMinute	= nuStrtol(tempbuffer, &lpEnd,10);
				nuMemcpy(tempbuffer,&(ItemData[4]),2);
				m_FirstHandData.nTime.nSecond	= nuStrtol(tempbuffer, &lpEnd,10);
				m_FirstHandData.nTime.nHour		= m_FirstHandData.nTime.nHour%24;
				m_FirstHandData.nTime.nMinute	= m_FirstHandData.nTime.nMinute%60;
				m_FirstHandData.nTime.nSecond	= m_FirstHandData.nTime.nSecond%60;
				if(nGGASeconds!=m_FirstHandData.nTime.nSecond)	bFindGGA = nuFALSE;
				break;
			case 2:
				if(DataLen<=0) break;
				m_FirstHandData.nStatus_RMC=((ItemData[0]=='A')? DATA_TYPE_GPS : DATA_TYPE_INVALID);
				m_DRData.nStatus_RMC=((ItemData[0]=='A')? DATA_TYPE_GPS : DATA_TYPE_GYRO);
				break;
			case 3:
				if(DataLen<=0) break;
				nuDegreeToMeter((nuFLOAT)(nuStrtod(ItemData,&lpEnd)), m_FirstHandData.nLatitude);
				break;
			case 4:
				if(DataLen<=0) break;
				m_FirstHandData.nLatitude=((ItemData[0]=='S')? -m_FirstHandData.nLatitude : m_FirstHandData.nLatitude);
				break;
			case 5:
				if(DataLen<=0) break;
				nuDegreeToMeter((nuFLOAT)(nuStrtod(ItemData,&lpEnd)), m_FirstHandData.nLongitude);
				break;
			case 6:
				if(DataLen<=0) break;
				m_FirstHandData.nLongitude=((ItemData[0]=='W')? -m_FirstHandData.nLongitude : m_FirstHandData.nLongitude);
				break;
			case 7:
				if(DataLen<=0) break;
				m_FirstHandData.nSpeed	= (nuSHORT)(nuStrtod(ItemData,&lpEnd)*KMTOSNOT);  //誹蛌遙峈km/h
				break;
			case 8:
				if(DataLen<=0) break;
				m_FirstHandData.nAngle	= ( (90 - (nuSHORT)nuStrtod(ItemData, &lpEnd)) + 360 ) % 360;
				break;
			case 9:
				if(DataLen<6) break;
				nuMemset(tempbuffer,NULL,3);
				nuMemcpy(tempbuffer,&(ItemData[0]),2);
				m_FirstHandData.nTime.nDay	= nuStrtol(tempbuffer, &lpEnd,10);
				nuMemcpy(tempbuffer,&(ItemData[2]),2);
				m_FirstHandData.nTime.nMonth	= nuStrtol(tempbuffer, &lpEnd,10);
				nuMemcpy(tempbuffer,&(ItemData[4]),2);
				m_FirstHandData.nTime.nYear		= nuStrtol(tempbuffer, &lpEnd,10);
				bCheckTimeForZone( &(m_FirstHandData.nTime), m_nZoneTime );
				bFindRMC = nuTRUE;
				break;
/*
			case 11:
			case 12:
				if(DataLen<=0) break;
				switch(ItemData[0])
				{
					case 'D'://GPS定位
					case 'A':
						m_FirstHandData.nStatus	=	DATA_TYPE_GPS;//3D+DGPS
						break;
					case 'E'://DR模式
						m_FirstHandData.nStatus	=	DATA_TYPE_GYRO;
						break;
					case 'N'://無資訊狀態
						m_FirstHandData.nStatus	=	DATA_TYPE_GYRO;//DATA_TYPE_INVALID;
						break;
				}
				break;
*/
			default:
				break;
		}
	}
	nuVOID CGPSDataCol::ColDRRMC(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
	{
		nuCHAR* lpEnd = NULL;
		nuCHAR tempbuffer[3] = {0};
		NUROTIME TmpTime = {0};
		m_SpecialCaseData.l_DRMode = FINDDR_CN_DR;
		switch(Index)
		{
			case 7:
				if(DataLen<=0) break;
				m_SpecialCaseData.m_EL.nSpeed	= (nuSHORT)(nuStrtod(ItemData,&lpEnd)*KMTOSNOT);
				break;
			case 8:
				if(DataLen<=0) break;
				m_SpecialCaseData.m_EL.nAngle	= ( (90 - (nuSHORT)nuStrtod(ItemData, &lpEnd)) + 360 ) % 360;
				break;
			default:
				break;
		}
	}

	nuVOID CGPSDataCol::ColGGA(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
	{
		nuCHAR* lpEnd = NULL;
		nuCHAR tempbuffer[3] = {0};
		nuBYTE i = 0;
		switch(Index)
		{
			case 1://hhmmrr
				if(DataLen<6) break;
				nuMemset(tempbuffer,NULL,3);
				nuMemcpy(tempbuffer,&(ItemData[4]),2);
				nGGASeconds	= nuStrtol(tempbuffer, &lpEnd,10);	
				if(nGGASeconds!=m_FirstHandData.nTime.nSecond)	bFindRMC = nuFALSE;
				bFindGGA = nuTRUE;
				break;
			case 7:
				if(DataLen<=0) break;
				m_FirstHandData.nActiveStarNum	= nuStrtol(ItemData, &lpEnd,10);
				if(m_BackData.nActiveStarNum>MAX_STAR_NUMBER)
				{
					m_FirstHandData.nActiveStarNum	= m_BackData.nActiveStarNum%MAX_STAR_NUMBER;
				}
				break;
			case 9:
				if(DataLen<=0) break;
				m_FirstHandData.nAntennaHeight	= (nuLONG)nuStrtod(ItemData, &lpEnd);				
				break;
			default:
				break;
		}
	}

	nuVOID CGPSDataCol::ColGSA(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
	{
		nuCHAR* lpEnd = NULL;
		switch(Index)
		{
			case 2:
				if(DataLen<=0) break;
				switch(ItemData[0])
				{
					case '3':
						m_FirstHandData.nStatus	=	DATA_TYPE_GPS;
						break;
					case '2':
						m_FirstHandData.nStatus	=	DATA_TYPE_GYRO;//有陀螺儀時 可以將這種狀態切為陀螺儀處理
						break;
					default:
						m_FirstHandData.nStatus	=	DATA_TYPE_INVALID;
						break;
				}
				if(m_GSAcleantime!=m_SecondTimeData.nTime.nSecond)
				{
					m_GSAcleantime=m_SecondTimeData.nTime.nSecond;
					m_GSAIndex=0;
				}
				break;
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				if(DataLen<=0) break;
				GSAID[m_GSAIndex]=nuStrtol(ItemData, &lpEnd,10);
				if(m_GSAIndex<MAX_STAR_NUMBER)	m_GSAIndex++;
				break;
			default:
				break;
		}
	}

	nuVOID CGPSDataCol::ColGSV(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
	{
		static nuLONG GSVPageID=0;//紀錄當前解析的是GPGSV第幾頁資料
		nuCHAR* lpEnd = NULL;
		nuLONG PageID = 0;
		nuLONG i;
		switch(Index)
		{
			case 0:
				if(strstr(ItemData,"GPGSV"))//GPS
				{
					l_GPSSateSystem=0;
				}
				if(strstr(ItemData,"BDGSV"))//北斗
				{
					l_GPSSateSystem=1;
				}
				if(strstr(ItemData,"GLGSV"))//GLONASS
				{
					l_GPSSateSystem=2;
				}
				break;
			case 2:
				if(DataLen<=0) break;
				PageID=nuStrtol(ItemData, &lpEnd,10);
				if(GSVPageID>=PageID && l_GPSSateSystem==0)
				{
					m_FirstHandData.nStarNumUsed	= 0;
				}
				GSVPageID=PageID;
				bFindGSV = nuTRUE;
				break;
			case 4:
			case 8:
			case 12:
			case 16:
				bAddNewStar=nuFALSE;
				if(DataLen<=0) break;
				if(MAX_STAR_NUMBER>m_FirstHandData.nStarNumUsed)
				{
					m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nStarID =nuStrtol(ItemData, &lpEnd,10) ;
					m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nSateSystem=l_GPSSateSystem;
					bAddNewStar=nuTRUE;
				}
				break;
			case 5:
			case 9:
			case 13:
			case 17:
				if(bAddNewStar)
				{
					bAddNewStar=nuFALSE;
					if(DataLen<=0) break;
					m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nAngle_elevation =nuStrtol(ItemData, &lpEnd,10) ;
					bAddNewStar=nuTRUE;
				}
				break;
			case 6:
			case 10:
			case 14:
			case 18:
				if(bAddNewStar)
				{
					bAddNewStar=nuFALSE;
					if(DataLen<=0) break;
					m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nAngle_position =nuStrtol(ItemData, &lpEnd,10) ;
					bAddNewStar=nuTRUE;
				}
				break;
			case 7:
			case 11:
			case 15:
			case 19:
				if(bAddNewStar)
				{
					bAddNewStar=nuFALSE;
					m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].b_GPSOK=nuFALSE;
					m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nSignalNoiseRatio	= 0;
					if(DataLen<=0) break;
					m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nSignalNoiseRatio =nuStrtol(ItemData, &lpEnd,10) ;
					for(i=0;i<m_GSAIndex;i++)
					{
						if(m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].nStarID==GSAID[i])
						{
							m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].b_GPSUsed=nuTRUE;
						}
					}
					m_FirstHandData.pStarList[m_FirstHandData.nStarNumUsed].b_GPSOK=nuTRUE;
					m_FirstHandData.nStarNumUsed++;
				}
				break;
			default:
				break;
		}
	}

	
//興聯特製子句
	nuVOID CGPSDataCol::ColGPGDR(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
	{
		nuCHAR* lpEnd = NULL;
		switch(Index)
		{
			case 1:
				if(DataLen<=0) break;
				m_SpecialCaseData.m_SMART.lSystemStatus=((ItemData[0]=='G')? DATA_TYPE_GPS : DATA_TYPE_GYRO );
				break;
			case 4:
				if(DataLen<=0) break;
				nuDegreeToMeter((nuFLOAT)(nuStrtod(ItemData,&lpEnd)), m_SpecialCaseData.m_SMART.lLatitude);
				break;
			case 5:
				if(DataLen<=0) break;
				m_SpecialCaseData.m_SMART.lLatitude=((ItemData[0]=='S')? -m_SpecialCaseData.m_SMART.lLatitude : m_SpecialCaseData.m_SMART.lLatitude);
				break;
			case 6:
				if(DataLen<=0) break;
				nuDegreeToMeter((nuFLOAT)(nuStrtod(ItemData,&lpEnd)), m_SpecialCaseData.m_SMART.lLongitude);
				break;			
			case 7:
				if(DataLen<=0) break;
				m_SpecialCaseData.m_SMART.lLongitude=((ItemData[0]=='W')? -m_SpecialCaseData.m_SMART.lLongitude : m_SpecialCaseData.m_SMART.lLongitude);
				break;
			case 8:
				if(DataLen<=0) break;
				m_SpecialCaseData.m_SMART.nAngle	= ( (90 - (nuSHORT)nuStrtod(ItemData, &lpEnd)) + 360 ) % 360;
				break;
			case 10:
				if(DataLen<=0) break;
				m_SpecialCaseData.m_SMART.nOBD_Speed	= (nuSHORT)(nuStrtod(ItemData,&lpEnd));  //km/h
				m_SpecialCaseData.l_DRMode = FINDDR_SMART;
				break;
			case 13:
				if(DataLen<=0) break;
				switch(ItemData[0])
				{
					case '+':
						m_SpecialCaseData.m_SMART.nSlopeStatus	= GPS_SLOPE_FASTUP;
						break;
					case '-':
						m_SpecialCaseData.m_SMART.nSlopeStatus	= GPS_SLOPE_FASTDOWN;
						break;
					default:
						m_SpecialCaseData.m_SMART.nSlopeStatus	= GPS_SLOPE_NO;
						break;
				}
				break;
			case 16:
				if(DataLen<=0) break;
				m_SpecialCaseData.m_SMART.nSlopeStatus=((nuStrtod(ItemData,&lpEnd)<=2)? GPS_SLOPE_NO : m_SpecialCaseData.m_SMART.nSlopeStatus );
				break;
			default:
				break;
		}
	}


//UBLOS特殊句型
//處理ublos資訊 Kris @2013/2/8
//Louis 2012/3/20 處理ublos資訊
nuBOOL CGPSDataCol::ColUBlosVersion(nuUCHAR *ItemData,nuBYTE DataLen)
{
	nuDWORD mCheckSum[2] = {0};
	nuLONG m_TotalLen = 0;
	nuLONG j = 0;
	//處理u_blos陀螺儀Version輸出
	if(DataLen>=74)
	{
		if( ItemData[0]==0xB5 && ItemData[1]==0x62 && ItemData[2]==0x0A && ItemData[3]==0x04 )
		{
			m_TotalLen=6+ItemData[4]+(ItemData[5]*256)+2;
			if(DataLen==m_TotalLen)
			{
				mCheckSum[0]=0;
				mCheckSum[1]=0;
				for(j=2;j<m_TotalLen-2;j++)
				{
					mCheckSum[0]=mCheckSum[0]+ItemData[j];
					mCheckSum[1]=mCheckSum[1]+mCheckSum[0];
					mCheckSum[0]=mCheckSum[0]%256;
					mCheckSum[1]=mCheckSum[1]%256;
				}
				if(ItemData[m_TotalLen-2]==mCheckSum[0] && ItemData[m_TotalLen-1]==mCheckSum[1])
				{
					nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.EXTVERSION,ItemData+6,30);
					nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.HWVERSION,ItemData+36,10);
					nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.ROMVERSION,ItemData+46,30);
					return nuTRUE;
				}		
			}
		}
	}
	return nuFALSE;
}

nuBOOL CGPSDataCol::ColUBlosNAVSTATUS(nuUCHAR *ItemData,nuBYTE DataLen)
{
	nuDWORD mCheckSum[2] = {0};
	nuLONG j = 0;
	if(DataLen==24)
	{
		if( ItemData[0]==0xB5 && ItemData[1]==0x62 && ItemData[2]==0x01 && ItemData[3]==0x03 && ItemData[4]==0x10 && ItemData[5]==0x00)
		{
			for(j=2;j<22;j++)
			{
				mCheckSum[0]=mCheckSum[0]+ItemData[j];
				mCheckSum[1]=mCheckSum[1]+mCheckSum[0];
				mCheckSum[0]=mCheckSum[0]%256;
				mCheckSum[1]=mCheckSum[1]%256;
			}

			if(ItemData[22]==mCheckSum[0] && ItemData[23]==mCheckSum[1])
			{
				m_SpecialCaseData.m_UBLOS.lSystemStatus=((ItemData[10]>=0x03)?DATA_TYPE_GPS:DATA_TYPE_GYRO);
				return nuTRUE;
			}
		}
	}
	return nuFALSE;
}

nuBOOL CGPSDataCol::ColUBlosESFMEAS(nuUCHAR *ItemData,nuBYTE DataLen)
{
	nuINT TmpNum;
	nuFLOAT TmpFloat;
	MEASINFO Unit_Meas;
	if(DataLen>=16 && (ItemData[0]==0xB5 && ItemData[1]==0x62 && ItemData[2]==0x10 && ItemData[3]==0x02) )
	{
		if(DataLen==(8+ItemData[4]))
		{
			for(nuINT i=0;i<(ItemData[4]-8)/4;i++)
			{
				TmpNum=NumComb(ItemData[14+i*4+3],ItemData[14+i*4+2],ItemData[14+i*4+1],ItemData[14+i*4]);
				nuMemcpy(&Unit_Meas,&TmpNum,sizeof(MEASINFO));
				switch(Unit_Meas.datatype)
				{
					case 5://z-axis gyro
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_ZGyro=(nuFLOAT)(Unit_Meas.datafield/4096.0);
						break;
					case 10://Speed tick
						if(Unit_Meas.datafield&0x00800000)
						{
							m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_ReverseSignal=1;
							m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_SpeedTick=(nuFLOAT)(Unit_Meas.datafield&0x007FFFFF);					
						}
						else
						{
							m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_ReverseSignal=0;
							m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_SpeedTick=(nuFLOAT)(Unit_Meas.datafield&0x007FFFFF);					
						}
						break;
					case 11://Speed
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_Speed=Unit_Meas.datafield/1000.0;
						break;
					case 12://Gyro Temperture
						//Unit_Meas.datafield/100.0;
						break;
					case 13://y-axis gyro
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_YGyro=(nuFLOAT)(Unit_Meas.datafield/4096.0);
						break;
					case 14://x-axis gyro
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_XGyro=(nuFLOAT)(Unit_Meas.datafield/4096.0);
						break;
					case 16://x-axis accelerometer
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_XAcc=(nuFLOAT)(Unit_Meas.datafield/1024.0);
						break;
					case 17://y-axis accelerometer
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_YAcc=(nuFLOAT)(Unit_Meas.datafield/1024.0);
						break;
					case 18://z-axis accelerometer
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Data_ZAcc=(nuFLOAT)(Unit_Meas.datafield/1024.0);
						break;
				}
			}
			return nuTRUE;
		}
	}
	return nuFALSE;
}

nuBOOL CGPSDataCol::ColUBlosESFSTATUS(nuUCHAR *ItemData,nuBYTE DataLen)
{
	nuINT i=0;
	if(DataLen>=24 && (ItemData[0]==0xB5 && ItemData[1]==0x62 && ItemData[2]==0x10 && ItemData[3]==0x10) )
	{
		if(DataLen==(24+(ItemData[21]*4)))
		{
			m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Cal_SFDRStatus=ItemData[18];
			for(i=0;i<ItemData[21];i++)
			{
				switch(ItemData[21+i*4+1]&0x3F)
				{
					case 5://z-axis gyro
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Cal_ZGyro=ItemData[21+i*4+2]&0x03;
						break;
					case 10://Speed tick
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Cal_SpeedTick=ItemData[21+i*4+2]&0x03;
						break;
					case 11://Speed
						break;
					case 12://Gyro Temperture
						break;
					case 13://y-axis gyro
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Cal_YGyro=ItemData[21+i*4+2]&0x03;
						break;
					case 14://x-axis gyro
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Cal_XGyro=ItemData[21+i*4+2]&0x03;
						break;
					case 16://x-axis accelerometer
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Cal_XAcc=ItemData[21+i*4+2]&0x03;
						break;
					case 17://y-axis accelerometer
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Cal_YAcc=ItemData[21+i*4+2]&0x03;
						break;
					case 18://z-axis accelerometer
						m_SpecialCaseData.m_UBLOS.ModInfo.MODEINFO.Cal_ZAcc=ItemData[21+i*4+2]&0x03;
						break;
				}
			}
			return nuTRUE;
		}
	}
	return nuFALSE;
}

nuVOID CGPSDataCol::ColGPTXT(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
{
	switch(Index)
	{
		case 4:
			if(nuStrstr(ItemData,"HW"))//DR HW version
			{
				nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.HWVERSION,ItemData,NURO_MIN(DataLen,GPSVERSIONLEN-1));
			}
			if(nuStrstr(ItemData,"EXT"))//DR FW version
			{
				nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.EXTVERSION,ItemData,NURO_MIN(DataLen,GPSVERSIONLEN-1));
			}
			if(nuStrstr(ItemData,"ROM"))//GPS IC baseband ROM FW version
			{
				nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.ROMVERSION,ItemData,NURO_MIN(DataLen,GPSVERSIONLEN-1));
			}
			break;
		default:
			break;
	}
}

nuBOOL CGPSDataCol::ColUBlosEKFSTATUS(nuUCHAR *ItemData,nuBYTE DataLen)
{
	EKFSTATUS	tmpekf;
	nuDWORD		mCheckSum[2] = {0};
	nuLONG j = 0;
	//處理u_blos陀螺儀EKFSTATUS輸出
	if(DataLen==44 && (ItemData[0]==0xB5 && ItemData[1]==0x62 && ItemData[2]==0x01 && ItemData[3]==0x40 && ItemData[4]==0x24 && ItemData[5]==0x00))
	{
		mCheckSum[0]=0;
		mCheckSum[1]=0;
		for(j=2;j<42;j++)
		{
			mCheckSum[0]=mCheckSum[0]+ItemData[j];
			mCheckSum[1]=mCheckSum[1]+mCheckSum[0];
			mCheckSum[0]=mCheckSum[0]%256;
			mCheckSum[1]=mCheckSum[1]%256;
		}

		if(ItemData[42]==mCheckSum[0] && ItemData[43]==mCheckSum[1])
		{
			nuMemcpy(&tmpekf,ItemData+6,sizeof(EKFSTATUS));
			m_SpecialCaseData.m_UBLOS.bSpeedInit=((ItemData[21]&0x00000002)? nuTRUE : nuFALSE);
			m_SpecialCaseData.m_UBLOS.bSpeedInput=((ItemData[21]&0x00000002)? nuTRUE : nuFALSE);
			m_SpecialCaseData.m_UBLOS.bAngleInit=((ItemData[21]&0x00000008)? nuTRUE : nuFALSE);
			m_SpecialCaseData.m_UBLOS.bAngleInput=((ItemData[21]&0x00000008)? nuTRUE : nuFALSE);
			m_SpecialCaseData.m_UBLOS.lDeltaAngle=(nuLONG)(((tmpekf.difang/100.0-32757.5)*90)*EFFECTBASE/32757.5);
			nuMemset(m_SpecialCaseData.m_UBLOS.ModInfo.EKFSTATUS,NULL,MAXUBXLEN);
			nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.EKFSTATUS,ItemData,DataLen);
		}
		else
		{
			m_SpecialCaseData.m_UBLOS.bAngleInit=nuTRUE;
			m_SpecialCaseData.m_UBLOS.bSpeedInit=nuTRUE;
			m_SpecialCaseData.m_UBLOS.bAngleInput=nuTRUE;
			m_SpecialCaseData.m_UBLOS.bSpeedInput=nuTRUE;
			m_SpecialCaseData.m_UBLOS.lDeltaAngle=0;
		}

		m_SpecialCaseData.m_UBLOS.lcalibstatus=((ItemData[21]&0x00000002 && ItemData[21]&0x00000008 && ItemData[21]&0x00000010)? nuTRUE : nuFALSE);
		m_SpecialCaseData.l_DRMode = FINDDR_UBLOS;
		return nuTRUE;
	}
	return nuFALSE;
}

nuBOOL CGPSDataCol::ColUBlosEKF(nuUCHAR *ItemData,nuBYTE DataLen)
{
	nuDWORD mCheckSum[2] = {0};
	nuLONG j = 0;
	//處理u_blos陀螺儀EKFSTATUS輸出
	if(DataLen==24 && (ItemData[0]==0xB5 && ItemData[1]==0x62 && ItemData[2]==0x06 && ItemData[3]==0x12 && ItemData[4]==0x10 && ItemData[5]==0x00) )
	{
		mCheckSum[0]=0;
		mCheckSum[1]=0;
		for(j=2;j<22;j++)
		{
			mCheckSum[0]=mCheckSum[0]+ItemData[j];
			mCheckSum[1]=mCheckSum[1]+mCheckSum[0];
			mCheckSum[0]=mCheckSum[0]%256;
			mCheckSum[1]=mCheckSum[1]%256;
		}
		if(ItemData[22]==mCheckSum[0] && ItemData[23]==mCheckSum[1])
		{
			nuMemset(m_SpecialCaseData.m_UBLOS.ModInfo.EKF,NULL,MAXUBXLEN);
			nuMemcpy(m_SpecialCaseData.m_UBLOS.ModInfo.EKF,ItemData,DataLen);
		}
		return nuTRUE;
	}
	return nuFALSE;
}

nuBOOL CGPSDataCol::ColUBlosEnable(nuUCHAR *ItemData,nuBYTE DataLen)
{
	nuDWORD mCheckSum[2] = {0};
	nuLONG m_TotalLen = 0;
	nuLONG j = 0;
	//處理u_blos ANT_Enable 判斷
	if(ItemData[2]==0x0A && ItemData[3]==0x09 )
	{
		m_TotalLen=6+ItemData[4]+(ItemData[5]*256)+2;
		if(DataLen!=m_TotalLen)		return nuFALSE;

		mCheckSum[0]=0;
		mCheckSum[1]=0; 
		for(j=2;j<m_TotalLen-2;j++)
		{
			mCheckSum[0]=mCheckSum[0]+ItemData[j];
			mCheckSum[1]=mCheckSum[1]+mCheckSum[0];
			mCheckSum[0]=mCheckSum[0]%256;
			mCheckSum[1]=mCheckSum[1]%256;
		}
		if(ItemData[m_TotalLen-2]==mCheckSum[0] && ItemData[m_TotalLen-1]==mCheckSum[1])
		{
			m_SpecialCaseData.m_UBLOS.bAngleInit=((ItemData[19]==0x76 || ItemData[19]==0x72)? 1 : 0);
			return nuTRUE;
		}		
	}
	return nuFALSE;
}


//鼎天的特殊句型
	nuVOID CGPSDataCol::ColRTOEM(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
	{
		nuCHAR* lpEnd = NULL;
		nuINT slp=0;

		switch(Index)
		{
			case 3://陀螺儀校正完畢 1/Y  0/N
				if(DataLen<=0) break;
				m_SpecialCaseData.m_ROYALTEK.bAngleInit=((ItemData[0]=='1')? nuTRUE : nuFALSE);
				break;
			case 4://車速線校正完畢 1/Y  0/N
				if(DataLen<=0) break;
				m_SpecialCaseData.m_ROYALTEK.bSpeedInit=((ItemData[0]=='1')? nuTRUE : nuFALSE);
				break;
			case 5://陀螺儀輸入狀態 1/Y  0/N
				if(DataLen<=0) break;
				m_SpecialCaseData.m_ROYALTEK.bAngleInput=((ItemData[0]=='1')? nuTRUE : nuFALSE);
				break;
			case 6://車速輸入狀態 1/Y  0/N
				if(DataLen<=0) break;
				m_SpecialCaseData.m_ROYALTEK.bSpeedInput=((ItemData[0]=='1')? nuTRUE : nuFALSE);
				break;
			case 7://第三碼 4:初始狀態 3:GPS 2:DR
				if(DataLen<=0) break;
				switch(ItemData[2])
				{
					case '2':
						m_SpecialCaseData.m_ROYALTEK.lSystemStatus=DATA_TYPE_GYRO;
						break;
					case '3':
						m_SpecialCaseData.m_ROYALTEK.lSystemStatus=DATA_TYPE_GPS;
						break;
					default:
						m_SpecialCaseData.m_ROYALTEK.lSystemStatus=DATA_TYPE_INVALID;
						break;
				}
				m_SpecialCaseData.m_ROYALTEK.lSystemStatus=((ItemData[4]=='0')? DATA_TYPE_INVALID : m_SpecialCaseData.m_ROYALTEK.lSystemStatus);
				m_SpecialCaseData.l_DRMode = FINDDR_ROYALTEK;
				break;
			case 10://瞬時角度變化值
				if(DataLen<=0) break;
				m_SpecialCaseData.m_ROYALTEK.lDeltaAngle=-(nuLONG)(nuStrtod(ItemData,&lpEnd)*EFFECTBASE);
				break;
			case 12://瞬時距離變化值
				if(DataLen<=0) break;
				m_SpecialCaseData.m_ROYALTEK.lDeltaDis=(nuLONG)(nuStrtod(ItemData,&lpEnd)*EFFECTBASE);//-(nuLONG)(nuStrtod(ItemData,&lpEnd)*EFFECTBASE);
				break;
			case 16:
				if(DataLen<=0) break;
				slp=nuStrtod(ItemData,&lpEnd);
				switch(slp)
				{
					case 1:
						m_SpecialCaseData.m_ROYALTEK.nSlopeStatus = GPS_SLOPE_FASTUP;
						break;
					case -1:
						m_SpecialCaseData.m_ROYALTEK.nSlopeStatus = GPS_SLOPE_FASTDOWN;
						break;
					default:
						m_SpecialCaseData.m_ROYALTEK.nSlopeStatus = GPS_SLOPE_NO;
						break;
				}
			default:
				break;
		}
	}


//怡利的特殊句型
	nuVOID CGPSDataCol::ColGYRO(nuLONG Index,nuCHAR *ItemData,nuBYTE DataLen)
	{
		nuCHAR* lpEnd = NULL;
		nuCHAR tempbuffer[3] = {0};

		switch(Index)
		{
			case 1://Up/down hill
				if(DataLen<=0) break;
				switch(ItemData[0])
				{
					case 'U':
						m_SpecialCaseData.m_EL.nSlopeStatus = GPS_SLOPE_FASTUP;
						break;
					case 'D':
						m_SpecialCaseData.m_EL.nSlopeStatus = GPS_SLOPE_FASTDOWN;
						break;
					case 'F':
					default:
						m_SpecialCaseData.m_EL.nSlopeStatus = GPS_SLOPE_NO;
						break;
				}
				break;
			case 2://SN
				break;
			case 3://Status
				if(DataLen<=0) break;
				m_SpecialCaseData.m_EL.sELGyroStatus=ItemData[0];
				break;
			case 4://Speed
				if(DataLen<=0) break;
				m_SpecialCaseData.m_EL.nSpeed	= (nuSHORT)(nuStrtod(ItemData,&lpEnd)*1);  //誹蛌遙峈km/h
				break;
			case 5://Course
				if(DataLen<=0) break;
				m_SpecialCaseData.m_EL.nAngle	= ( (90 - (nuSHORT)nuStrtod(ItemData, &lpEnd)) + 360 ) % 360;
				m_SpecialCaseData.l_DRMode = FINDDR_EL;
				break;
			case 6://Latitude
				if(DataLen<=0) break;
				nuDegreeToMeter((nuFLOAT)(nuStrtod(ItemData,&lpEnd)), m_SpecialCaseData.m_EL.nLatitude);
				break;
			case 7://N/S indicator
				if(DataLen<=0) break;
				if(ItemData[0]=='S')
				{
					m_SpecialCaseData.m_EL.nLatitude = -m_SpecialCaseData.m_EL.nLatitude;
				}
				break;
			case 8://Longitude
				if(DataLen<=0) break;
				nuDegreeToMeter((nuFLOAT)(nuStrtod(ItemData,&lpEnd)), m_SpecialCaseData.m_EL.nLongitude);
				break;
			case 9://E/W indicator
				if(DataLen<=0) break;
				if(ItemData[0]=='W')
				{
					m_SpecialCaseData.m_EL.nLongitude = -m_SpecialCaseData.m_EL.nLongitude;
				}
				break;
			default:
				break;
		}
	}

nuBOOL CGPSDataCol::AddMMF(nuLONG nLat, nuLONG nLng, nuSHORT nAngle, nuLONG nLatFix, nuLONG nLngFix, nuSHORT nAngleFix)
{
	  nuINT i = 0;
	  nuLONG nMMF_Lat  = ( nLatFix - nLat ) * 10 ;
	  nuLONG nMMF_Lng  = ( nLngFix - nLng ) * 10 ;
	  nuSHORT nMMF_Ang = ( nAngleFix - nAngle ) * 10 * (-1) ; 
	  nuCHAR sMMF[100] = "$MMF,";
	  nuCHAR sTmp[10] = {0};
	  nuItoa( nMMF_Lat, sTmp, 10 );
	  nuStrcat( sMMF, sTmp );
	  nuStrcat( sMMF, ",A,");
	  nuItoa( nMMF_Lng, sTmp, 10 );
	  nuStrcat( sMMF, sTmp );
	  nuStrcat( sMMF, ",A,");
	  nuItoa( nMMF_Ang, sTmp, 10 );
	  nuStrcat( sMMF, sTmp );
	  nuStrcat( sMMF, ",A*");
	  nuCHAR CheckSum = 0;
//	 nuMemcpy( sMMF, "$GPGSV,4,3,13,15,08,086,,24,01,169,,32,21,322,,31,17,237,*74",sizeof("$GPGSV,4,3,13,15,08,086,,24,01,169,,32,21,322,,31,17,237,*74")) ;
	  for( i = 1; i < nuStrlen(sMMF); i++)
	  {
		  if( sMMF[i] == '*' )
		  {
			  break;
		  }
		  else
		  {
			CheckSum = CheckSum^sMMF[i];
		  }
	  }
	  nuItoa( CheckSum, sTmp, 16 );
	  if(sTmp[1]==NULL)
	  {
		  sTmp[1]=sTmp[0];
		  sTmp[0]='0';
		  sTmp[2]=NULL;
	  }
	  nuStrcat( sMMF, sTmp );
	  nuINT nLen = nuStrlen(sMMF);
	  sMMF[nLen] = 13;
	  sMMF[nLen + 1] = 10;
	  sMMF[nLen + 2] = NULL;
	  
	  //for(i=0;i<nLen+2;i++){
	  //nuWriteGPSBYRO( (nuBYTE *)&(sMMF[i]) );
	  //}
//	  nuWriteGPSBYRO( (nuBYTE *)sMMF );

/*
	  if(  g_nGPSLogMode != 0 )
	  {
		  m_fLog.AppendGpsData( sMMF, nuStrlen(sMMF) );
		  m_fLog.AppendGpsData( "\r\n", nuStrlen("\r\n") );		
	  }
*/
	  return nuTRUE;
}
