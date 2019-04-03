#include "MapFileTMCDataP.h"
#include "MapFileTMCP.h"
#include "MapFileRnY.h"

#include "NuroDefine.h"

#ifdef WINCE
	#define FILE_Test
#else
	#define	  TMC_WINDOWS //for Windows
#endif

#ifndef DeBug2005				//2005 BUG ?i?J?ץ?
#define	  DeBug 1 
#endif

#define TIMELIMIT       1000*60
#define Tmc_Count_Limit 20

//#define  _TMC_DATA_LIMIT_	   400
#define _TMC_CHECK_DATA_COUNT  200

#ifdef FILE_Test											//???]?w?T?wŪ?ɦ??}
#define FILE_XMLR  _T("\\My Documents\\traffic_city.txt")	
#define FILE_XML   _T("\\My Documents\\traffic_tmc.txt")
#endif

//Nuro_Louis
#define MAXVDNUM	NURO_MAX_FILE_PATH*2
#define MAXVDEVENTNUM	NURO_MAX_FILE_PATH*2
#define FILE_XMLW  _T("\\My Documents\\traffic_way.txt")


CMapFileTMCDataP::CMapFileTMCDataP()
{
	nTempRoadIdx=-1;
	nTempHighIdx=-1;
	m_XML_Data=NULL;
	m_XML_Complex_Data=NULL;
	m_VD_Data=NULL;
	EventComparData=NULL;;			
	TMC_dataA=NULL;;					
	LocData=NULL;
	data=NULL;
	pFile=NULL;

	m_nWords=0;
	m_nByte_Record=0;
	m_bCheckHeader=nuFALSE;				//?P?_		?O?_?ŦXDATA_HEAD	
	m_bLeftWord=nuFALSE;					//			?O?_???Ѿl??sTmpWord[]?S?? ???J....
	m_bCorrect=nuFALSE;	
	m_nLeft=0;
	m_dStartTime=nuGetTickCount();
	m_dDistTime=TIMELIMIT;
	m_nArry_idx=0;
	m_nXml_Record=0;
	m_nXml_Complex_Record=0;
	m_nXml_fm_Record=0;
	m_bLeft=nuFALSE;

	bVDInit=nuFALSE;

	nVDCount=0;
	pVDData = NULL;
	pVDData = new UNITVD2[MAXVDNUM];

	nVDEventCount=0;
	pVDevent = NULL;
	pVDevent = new EVENTVD2[MAXVDEVENTNUM];

	nuMemset(m_nByte,0,sizeof(m_nByte));
	nuMemset(m_sLeftWord,0,sizeof(m_sLeftWord));
	nuMemset(m_sCheckStr,0,sizeof(m_sCheckStr));
	nuMemset(m_sCheckWord,0,sizeof(m_sCheckWord));
	

}

CMapFileTMCDataP::~CMapFileTMCDataP()
{
	nVDCount=0;
	if(NULL != pVDData)
	{
		delete [] pVDData;
		pVDData=NULL;
	}
	nVDEventCount=0;
	if(NULL !=pVDevent)
	{
		delete [] pVDevent;
		pVDevent=NULL;
	}
	if(NULL !=m_XML_Complex_Data)
	{
          delete[] m_XML_Complex_Data;
		  m_XML_Complex_Data=NULL;
	}
	if(NULL !=m_XML_Data)
	{
		delete[] m_XML_Data;
		m_XML_Data=NULL;
	}
	if(NULL !=m_VD_Data)
	{
		delete[] m_VD_Data;
		m_VD_Data=NULL;
	}
	if(NULL !=EventComparData)
	{
		delete[] EventComparData;
		EventComparData=NULL;
	}
	if(NULL !=TMC_dataA)
	{
		delete[] TMC_dataA;
		TMC_dataA=NULL;
	}
	if(NULL !=LocData)
	{
		delete[] LocData;
		LocData=NULL;
	}
	if(NULL !=pVDevent)
	{
		delete[] pVDevent;
		pVDevent=NULL;
	}
	if(NULL !=data)
	{
		delete[] data;
		data=NULL;
	}	
	
		


	ALLFree();

	Close();
}
nuBOOL	CMapFileTMCDataP::XML_Update()
{
	return nuTRUE;
}

nuBOOL CMapFileTMCDataP::Get_TMC_FM_XML_SIZE(nuUINT& nSelet,nuUINT* nFm_Count,nuUINT& nXML_Count,nuUINT* nXML_Complex_Count)
{	 
#ifndef DeBug				//2005 BUG ?i?J?ץ?

#endif
 
	
   //0:XML_L
   //1:FM
   //2:XML
	m_dStartTime=nuGetTickCount();
   
	ALLFree();
	
   // nSelet=3;
	if(nSelet==1)
	{


		//CleanVDData();
	
		if(NULL!=m_VD_Data)
		{
			delete[] m_VD_Data;
			m_VD_Data=NULL;
		}
		
			if(NULL!=m_XML_Complex_Data)
			{
				delete[] m_XML_Complex_Data;
				m_XML_Complex_Data=NULL;
			
			}
			m_nXml_Complex_Record=0;
			m_XML_Complex_Data=new TMC_XML_C_DATA[_TMC_DATA_LIMIT_];
			nuMemset(nCityCounts,-1,sizeof(nCityCounts));
			//?i??ROAD XML???ƸѪR
			if(!RoadXML_Parser())
			{
				//return 0;
			}
			nuMemcpy(nFm_Count,nCityCounts,sizeof(nCityCounts));
			//Get_City_SIZE(n);
			//nFm_Count=m_Record;//m_nXml_Complex_Record;
		
	}
	
	if(nSelet==0)
	{
	
		if(NULL!=m_XML_Data)
		{
			m_nXml_Record=0;
			delete[] m_XML_Data;
			m_XML_Data=NULL;
			RoadXMLFree();
		}


		nVD_Event_Count=0;
		nuMemset(nSpeedCounts,-1,sizeof(nSpeedCounts));
	if(pVDData && pVDevent)
	{
		bVDInit=nuTRUE;
		nuMemset(pVDData,NULL,sizeof(UNITVD2)*MAXVDNUM);
		nuMemset(pVDevent,NULL,sizeof(EVENTVD2)*MAXVDEVENTNUM);
		char sTime[30];
		ReadVD3(sTime);
	}
		//nXML_Complex_Count=nVD_Event_Count;//nVDCount;
		nuMemcpy(nXML_Complex_Count,nSpeedCounts,sizeof(nSpeedCounts));
	}

	if(nSelet==2)
	{
		if(NULL!=m_XML_Complex_Data)
		{
			m_nXml_Complex_Record=0;
			delete[] m_XML_Complex_Data;
			m_XML_Complex_Data=NULL;
			RoadXMLFree();
		}
		if(NULL!=m_XML_Data)
		{
			delete[] m_XML_Data;
			m_XML_Data=NULL;

		}
		m_nXml_Record=0;
		
		m_XML_Data=new TMC_Byte_DATA[_TMC_DATA_LIMIT_];

		m_nXml_Record=0;
		m_nXml_fm_Record=0;
		//?i??XML?ѪR
		if(!XML_Parser())
		{
			//return 0;
		}
		//?i??XML ?Ѽ?FREE
		if(!XMLFree())
		{
			//return 0;
		}
	
		
		m_nXml_fm_Record+=m_nByte_Record;

		//m_nXml_fm_Record=m_nXml_Record+m_nByte_Record;
		nXML_Count=m_nXml_fm_Record;
	}

	//?i??EVENT ?]?w?ʧ@
	//nuINT a[]={229,500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//SetEvent(a);

	return nuTRUE;
}

nuBOOL	CMapFileTMCDataP::Get_TMC_XML_L(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode)
{
 if(nmode>=22)
 {
	return nuFALSE;
 }	
 CityID=nmode;
#ifndef DeBug				//2005 BUG ?i?J?ץ?

#endif
 
	//ALLFree();

	if(tmc_ui.nCount<=0||nTempRoadIdx!=CityID) //?P?_?O?_?n?ѪR????
	{
		nTempRoadIdx=CityID;
		ALLFree();
		tmc_ui.nCount=m_nXml_Complex_Record;
		tmc_ui.pPTMC_INFO_LIST=new TMC_INFO_LIST[tmc_ui.nCount];
		if(tmc_ui.pPTMC_INFO_LIST  == NULL)
		{
			return nuFALSE;
		}
		nuMemset(tmc_ui.pPTMC_INFO_LIST, NULL, tmc_ui.nCount*sizeof(TMC_INFO_LIST));

		bLocationFrist=nuTRUE;
		bEventFrist=nuTRUE;

		m_nArry_idx=0; 
     
		//XML_COMPLEX_Order(nmode);
      
		nuWCHAR City[22][4];/*={L"?򶩥?",L"?O?_??",L"?s?_??",L"???鿤",L"?s?˥?",L"?s?˿?",L"?]?߿?",
					   L"?O????",L"???ƿ?",L"?n?뿤",L"???L??",L"?Ÿq??",L"?Ÿq??",L"?O?n??",
					   L"??????",L"?̪F??",L"?O?F??",L"?Ὤ??",L"?y????",L"??????",L"??????",L"?s????"};*/



		nuINT nIdx=0;
		for(;nIdx<m_nXml_Complex_Record;nIdx++)//m_nXml_Complex_Record
		{
			if(CityID<22)
			{
				if(nuWcsncmp(m_XML_Complex_Data[nIdx].wRoadName,City[CityID],nuWcslen(City[CityID]))==0)
				{
	
				nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName,m_XML_Complex_Data[nIdx].wRoadName,_TMC_UIROADNAME_LEN_*2);
				nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName,m_XML_Complex_Data[nIdx].wEvenName,_TMC_UIEVENTNAME_LEN_*2);
				m_nArry_idx++;
				}
			}
			
		}
		//RoadXMLFree();

	}

	nuINT idx=0,nArrayIdx=TmcDataLen;
	nArrayIdx*=5;
	TMC_INFO_LIST*  pWord = (TMC_INFO_LIST*)pTmcBuf;

	for(;idx<5;idx++,nArrayIdx++)
	{
		if(nArrayIdx>tmc_ui.nCount-1)//???i?W?L?}?C?j?p 
		{
			break;
		}
		pWord[idx]=tmc_ui.pPTMC_INFO_LIST[nArrayIdx];
	}
	return nuTRUE;

}
nuBOOL	CMapFileTMCDataP::Get_TMC_VD(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode)
{
 if(nmode>=24)
{
  return nuFALSE;
}
#ifndef DeBug				//2005 BUG ?i?J?ץ?

#endif
 
 nuWCHAR wChangWord[30] = {0};
	
	if(tmc_ui.nCount<=0||nmode!=nTempHighIdx) //?P?_?O?_?n?ѪR????
	{
		nTempHighIdx=nmode;
		nuTCHAR tsFileMapping2[NURO_MAX_PATH] = {0};
		nuGetModulePath(NULL, tsFileMapping2, NURO_MAX_PATH);
		nuTcscat(tsFileMapping2, FILE_CSC_VD_NAME);
		
		nuFILE*	pFile3 = nuTfopen(tsFileMapping2, NURO_FS_RB);
	
		if( pFile3 == NULL )
	{
		CleanVDData();
		return nuFALSE;
	}

	if(nuFread(sMapID2,sizeof(sMapID2),1,pFile3)!=1)
	{
		CleanVDData();
		return nuFALSE;
	}
	nuFclose(pFile3);
	pFile3=NULL;
		ALLFree();
		tmc_ui.nCount=nVD_Event_Count;
		tmc_ui.pPTMC_INFO_LIST=new TMC_INFO_LIST[tmc_ui.nCount];
		if(tmc_ui.pPTMC_INFO_LIST  == NULL)
		{
			return nuFALSE;
		}
		nuMemset(tmc_ui.pPTMC_INFO_LIST, NULL, tmc_ui.nCount*sizeof(TMC_INFO_LIST));

		bLocationFrist=nuTRUE;
		bEventFrist=nuTRUE;

		m_nArry_idx=0; 

		//VD_Order(nmode);

		char sIDLimit[][8]={"nfb2012","nfb2032","nfb2062","nfb2076","nfb2100","nfb2124",
"nfb2140","nfb2156","nfb2176","nfb2200","nfb2218","nfb2234"};



		nuINT z=nmode;		
		nuINT nIdx=nStartIdx[z];
		for(nuINT k=0;k<nSpeedCounts[z];k++,nIdx++)//nSpeedCounts[z]
		{
			for(nuINT n=0;n<_TMC_VD_MAPTABLE_COUNT2;n++)
			{
				if(nuStrcmp(pVDData[nIdx].VDName,sMapID2[n].VDID)==0)
				{
					nuMemset(wChangWord,0,sizeof(wChangWord));
					//nuLONG lLen = MultiByteToWideChar(CP_ACP,0,sMapID2[n].ID,nuStrlen(sMapID2[n].ID),wChangWord,sizeof(wChangWord));
					nuLONG lLen = nuMbsToWcs(sMapID2[n].ID, wChangWord, sizeof(wChangWord));
					wChangWord[lLen]    =    '\0';  
					if(lLen>=29)
					{
						wChangWord[29]    =    '\0';  
					}			
			
					nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName,wChangWord,sizeof(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName));

					//lLen = MultiByteToWideChar(CP_UTF8,0,pVDData[nIdx].Event1,nuStrlen(pVDData[nIdx].Event1),wChangWord,sizeof(wChangWord));   
					lLen = nuMbsToWcs(pVDData[nIdx].Event1, wChangWord, sizeof(wChangWord)); 
					wChangWord[lLen]    =    '\0';  
					if(lLen>=29)
					{
						wChangWord[29]    =    '\0';  
					}			
			nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName,wChangWord,sizeof(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName));
			m_nArry_idx++;
			nuINT l=0;		
			}
			}
			//m_nArry_idx++;
			/*
			for(nuINT j=0;j<16;j++)
			if(nuStrcmp(pVDData[nIdx].VDName,cRoadID[j])==0)
			{
				nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName,m_VD_Data[nIdx].wRoadName,sizeof(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName));
				nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName,m_VD_Data[nIdx].wEvenName,sizeof(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName));
				m_nArry_idx++;
			}*/
		}

		}

		nuINT zzz=0;
	

	nuINT idx=0,nArrayIdx=TmcDataLen;
	nArrayIdx*=5;
	TMC_INFO_LIST*  pWord = (TMC_INFO_LIST*)pTmcBuf;

	for(;idx<5;idx++,nArrayIdx++)
	{
		if(nArrayIdx>tmc_ui.nCount-1)//???i?W?L?}?C?j?p 
		{
			break;
		}
		pWord[idx]=tmc_ui.pPTMC_INFO_LIST[nArrayIdx];
	}
	return nuTRUE;

}
nuBOOL	CMapFileTMCDataP::Get_TMC_XML(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode)
{ 
  
#ifndef DeBug				//2005 BUG ?i?J?ץ?

#endif
 
   
	if(tmc_ui.nCount<=0) //?P?_?O?_?n?ѪR????
	{
		tmc_ui.nCount=m_nXml_Record;
		tmc_ui.pPTMC_INFO_LIST=new TMC_INFO_LIST[tmc_ui.nCount];
		nuMemset(tmc_ui.pPTMC_INFO_LIST, NULL, tmc_ui.nCount*sizeof(TMC_INFO_LIST));

		bLocationFrist=nuTRUE;
		bEventFrist=nuTRUE;

		m_nArry_idx=0; //
		m_TMC_Info_Router.nCount=m_nXml_Record;
		m_TMC_Info_Router.pPTMC_INFORouter = new TMC_ROUTER_DATA[m_TMC_Info_Router.nCount];
		
		nuINT	nIdx=0;
		for(;nIdx<m_nXml_Record;nIdx++)
		{
			ShowEvent(m_XML_Data[nIdx].nEvent);
			ShowLocation(m_XML_Data[nIdx].nRoad);
			m_nArry_idx++;  
		}

		XML_Order(nmode);

	}

	nuINT idx=0,nArrayIdx=TmcDataLen;
	nArrayIdx*=5;
	TMC_INFO_LIST*  pWord = (TMC_INFO_LIST*)pTmcBuf;

	for(;idx<5;idx++,nArrayIdx++)
	{
		if(nArrayIdx>tmc_ui.nCount-1)//???i?W?L?}?C?j?p 
		{
			break;
		}
		pWord[idx]=tmc_ui.pPTMC_INFO_LIST[nArrayIdx];
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::Get_TMC_FM(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode)
{ 
 
#ifndef DeBug				//2005 BUG ?i?J?ץ?

#endif


	if(tmc_ui.nCount<=0) //?P?_?O?_?n?ѪR????
	{

		tmc_ui.nCount=m_nByte_Record;
		tmc_ui.pPTMC_INFO_LIST=new TMC_INFO_LIST[tmc_ui.nCount];
		if(tmc_ui.pPTMC_INFO_LIST  == NULL)
		{
			return nuFALSE;
		}
		nuMemset(tmc_ui.pPTMC_INFO_LIST, NULL, tmc_ui.nCount*sizeof(TMC_INFO_LIST));

		if(tmc_ui.pPTMC_INFO_LIST  == NULL)
		{
			return nuFALSE;
		}

		bLocationFrist=nuTRUE;
		bEventFrist=nuTRUE;

		m_nArry_idx=0; //
		m_TMC_Info_Router.nCount=m_nByte_Record;
		m_TMC_Info_Router.pPTMC_INFORouter = new TMC_ROUTER_DATA[m_TMC_Info_Router.nCount];

		nuINT	nIdx=0;
		for(;nIdx<m_nByte_Record;nIdx++)
		{
			ShowEvent(m_nByte[nIdx].nEvent);
			ShowLocation(m_nByte[nIdx].nRoad);
			m_nArry_idx++;  
		}

		FM_Order(nmode);

	}

	

	nuINT idx=0,nArrayIdx=TmcDataLen;
	nArrayIdx*=5;
	TMC_INFO_LIST*  pWord = (TMC_INFO_LIST*)pTmcBuf;

	for(;idx<5;idx++,nArrayIdx++)
	{
		if(nArrayIdx>tmc_ui.nCount-1)//???i?W?L?}?C?j?p 
		{
			break;
		}
		pWord[idx]=tmc_ui.pPTMC_INFO_LIST[nArrayIdx];
		
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::Get_TMC_XML_FM(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode)
{

	if(tmc_ui.nCount<=0) //?P?_?O?_?n?ѪR????
	{

		tmc_ui.nCount=m_nByte_Record+m_nXml_Record;
		tmc_ui.pPTMC_INFO_LIST=new TMC_INFO_LIST[tmc_ui.nCount];
		if(tmc_ui.pPTMC_INFO_LIST == NULL)
		{
			return nuFALSE;
		}
		nuMemset(tmc_ui.pPTMC_INFO_LIST, NULL, tmc_ui.nCount*sizeof(TMC_INFO_LIST));

		bLocationFrist=nuTRUE;
		bEventFrist=nuTRUE;

		//tmc_ui.pPTMC_INFO_LIST=0; //

		
		m_nXml_fm_Record=m_nByte_Record+m_nXml_Record;


		m_TMC_Info_Router.nCount=m_nXml_fm_Record;
		m_TMC_Info_Router.pPTMC_INFORouter = new TMC_ROUTER_DATA[m_TMC_Info_Router.nCount];

		
		m_nArry_idx=0;

		//?i?? XML ?ѪR

		//XML_Order(nmode);
		nuINT nIdx=0;
	
		CMapFileTMCP tmcfile;
		if (!tmcfile.Open(0))
		{
			return 0;
		}
		TMC_Header header;
		tmcfile.ReadData(0,&header,sizeof(header));
		EventComparData=new TMC_EVENT_DATA[header.TrafficCnt];
		if(EventComparData==NULL)
		{
			return nuFALSE;
		}
		TMC_Event_Count=header.TrafficCnt;//?p???ƥ??W?ٵ???
		nuMemset(EventComparData,0,sizeof(TMC_EVENT_DATA)*header.TrafficCnt);
		nuINT nCount=-1;
		nuINT pos=header.TrafficDataAddr;
		while(nCount++<header.TrafficCnt)//?N????Ū?J ???s EventComparData[]  ?H?Q?????ƥ???????					
		{	 
			tmcfile.ReadData(pos,&EventComparData[nCount].wTraffic_Num,sizeof(nuWORD));
			pos+=sizeof(nuWORD);
			tmcfile.ReadData(pos,&EventComparData[nCount].wTraffic_NameLen,sizeof(nuWORD));
			pos+=sizeof(nuWORD);
			tmcfile.ReadData(pos,&EventComparData[nCount].sTraffic_EventWeighting,sizeof(short));
			pos+=sizeof(short);
			tmcfile.ReadData(pos,&EventComparData[nCount].sTraffic_EventTime,sizeof(short));
			pos+=sizeof(short);	
			tmcfile.ReadData(pos,&EventComparData[nCount].wEventName,EventComparData[nCount].wTraffic_NameLen);
			pos+=EventComparData[nCount].wTraffic_NameLen;	
		}
		bEventFrist=nuFALSE;
		tmcfile.Close();

		for(;nIdx<m_nXml_Record;nIdx++)
		{
			ShowEvent(m_XML_Data[nIdx].nEvent);
			ShowLocation(m_XML_Data[nIdx].nRoad);
			m_nArry_idx++;  
		}

		//?i?? Byte FM ?ѪR
		//FM_Order(nmode);
		nIdx=0;
		for(;nIdx<m_nByte_Record;nIdx++)
		{
			ShowEvent(m_nByte[nIdx].nEvent);
			ShowLocation(m_nByte[nIdx].nRoad);
			m_nArry_idx++;  
		}
		
		if(!TMCInfoFree())
		{
			//return 0;
		}
		XML_FM_Order(nmode);
	
	}

	nuINT idx=0,nArrayIdx=TmcDataLen;
	nArrayIdx*=5;
	TMC_INFO_LIST*  pWord = (TMC_INFO_LIST*)pTmcBuf;

	for(;idx<5;idx++,nArrayIdx++)
	{
		if(nArrayIdx>tmc_ui.nCount-1)//???i?W?L?}?C?j?p 
		{
			break;
		}
		pWord[idx]=tmc_ui.pPTMC_INFO_LIST[nArrayIdx];
		//nuMemcpy(pTmcBuf, tmc_ui.pPTMC_INFO_LIST, tmc_ui.nCount*sizeof(TMC_INFO_LIST));
	}


	return nuTRUE;

}
nuBOOL	CMapFileTMCDataP::Get_TMC_Router(nuVOID* pTmcBuf, const nuUINT TmcDataLen)
{
	TMC_ROUTER_DATA*  pWord = (TMC_ROUTER_DATA*)pTmcBuf;
	nuMemcpy(pTmcBuf, m_TMC_Info_Router.pPTMC_INFORouter, m_TMC_Info_Router.nCount*sizeof(TMC_ROUTER_DATA));
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::OpenXML(nuLONG nMapID)
{
	Close();

	nuTCHAR tsFile[NURO_MAX_PATH] = {0};

#ifdef WINCE
	nuTcscpy(tsFile,FILE_XML);
	//nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	//nuTcscat(tsFile, FILE_CSC_XML);
#else
	nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	nuTcscat(tsFile, FILE_CSC_XML);
#endif

	//Open XML File
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		//CleanVDData(); //@@@@@@@@@@@@@@@@@@@@@
		return nuFALSE;
	}

	return nuTRUE;
}

nuBOOL  CMapFileTMCDataP::XML_Parser ()
{
	//?ѪR²????XML?һݥΨ쪺TAGS
	nuCHAR	cXMLstart[]="LocationInfos";		
	nuCHAR	cLocationInfo[]="LocationInfo Country";
	nuCHAR	cLocationInfo2[]="LocationInfo Channel";
	nuCHAR	cGroup[]="Group";
	nuCHAR	cDirection[]="Direction";
	nuCHAR	cExtent[]="Extent";
	nuCHAR	cLocation[]="Location=";
	nuCHAR	cEvent[]="Event";
	nuCHAR	cLatitude[]="Latitude";
	nuCHAR	cLongitude[]="Longitude";
	nuCHAR	cIOTid[]="IOTid";
	nuCHAR	cStationid[]="Stationid";
	nuCHAR	cCountry[]="Country";

	nuCHAR	cEnd[]="\0";
	nuCHAR	cXmlEnd[]="/LocationInfos";
	nuCHAR	cParserStart[]="\"";
  
	//?????s??DATA??INDEX	
	//m_nArry_idx=0;

	//?X??DATA(?D?????T)
	XML_RDST_TMCINFO m_XmlRDSTMCInfo;

	if (!OpenXML(0))//ŪXML??
	{
		return 0;
	}

    nuCHAR	TmpWord = 0;
	nuCHAR	TempStr[NURO_MAX_PATH*2] = {0};
	
	nuCHAR	*pChr1 = NULL;
	nuCHAR	*pChr2 = NULL;
	nuLONG	lLen=0;
	nuBOOL	bStart=nuTRUE;
	nuINT	nCount=0;
    nuINT	nPos=-1;

	nuINT	nuWordLen = 0;
	nuINT	nRecord=0;
	nuINT	nIdx=0;

	TMC_Byte_DATA	XmlTemp;

	/*
	
	?ǥѤW?z??TAG?ѪRDATA?A????TAG???m?H??TAG?????????m(\")    	
	
	?ǥ?nuStrstr???XTAG???_?l???m?A???s?һݪ?LOCATION?BEVENT???T

	?A?ǥ?cLocation??�??TMC?ɤ??????W?BcEvent??�???ƥ????T

	*/

	while(nuFread(&TmpWord, 1, 1, pFile))
	{
		
		switch(TmpWord)
		{
			case '<':
					bStart=nuTRUE;
					lLen=0;
					nuMemset(TempStr,NULL,NURO_MAX_PATH*2*sizeof(nuCHAR));
					break;
			case '>':
					bStart=nuFALSE;
					break;
			default:
					if(bStart)
					{
						if(lLen<((NURO_MAX_PATH*2)-1))
						{
							TempStr[lLen]=TmpWord;
							lLen++;
						}
						else
						{
							bStart=nuFALSE;
							lLen=0;
						}
					}
					break;
		}

		if(bStart==nuFALSE && lLen!=0)
		{

			if(nuStrstr(TempStr,cLocationInfo)||nuStrstr(TempStr,cLocationInfo2))//????LocationInfo?y?z?l?y
			{

                nuMemset(&m_XmlRDSTMCInfo,0,sizeof(XML_RDST_TMCINFO));

				//pChr1 ?s??TAG???_?l???m
				//pChr2 ?s??TAG?????�??m
				//nuMemcpy ?NDATA(?r??)?s????m_XmlRDSTMCInfo.cLocation
				pChr1=nuStrstr(TempStr,cLocation);
				nuWordLen=nuStrlen(cLocation)+1;//
				pChr2=nuStrstr(pChr1+nuWordLen,cParserStart);
				nuMemcpy(m_XmlRDSTMCInfo.cLocation,pChr1+nuWordLen,pChr2-(pChr1+nuWordLen));

				//pChr1 ?s??TAG???_?l???m
				//pChr2 ?s??TAG?????�??m
				//nuMemcpy ?NDATA(?r??)?s????m_XmlRDSTMCInfo.cEvent
				pChr1=nuStrstr(TempStr,cEvent);
				nuWordLen=nuStrlen(cEvent)+2;
				pChr2=nuStrstr(pChr1+nuWordLen,cParserStart);
				nuMemcpy(m_XmlRDSTMCInfo.cEvent,pChr1+nuWordLen,pChr2-(pChr1+nuWordLen));
				
				nuLONG lLocation = 0;
				nuLONG lEvent    = 0;
				nuCHAR* pEdn;
				lLocation = nuStrtol(m_XmlRDSTMCInfo.cLocation, &pEdn, 10);
				lEvent    = nuStrtol(m_XmlRDSTMCInfo.cEvent, &pEdn, 10);		
		
				if(lEvent>=1772&&lEvent<=1790) //Kick Weather Info
				{
					bStart=nuFALSE;
					lLen=0;
					continue;
				}
				XmlTemp.nEvent=lEvent;
				XmlTemp.nRoad=lLocation;
				XmlTemp.nTime=nuGetTickCount();
	    
				if(CheckDataX(XmlTemp)||m_nXml_Record==0)
				{			
					m_XML_Data[nIdx]=XmlTemp;
					nIdx++;       //????+1
					m_nXml_Record++;
					m_nXml_fm_Record++;
				
				}
				
			}
			bStart=nuFALSE;
			lLen=0;
		}
	}
	//file.Close();
	nuFclose(pFile);
	pFile=NULL;
	//CheckDataTimeXML();//@@@@@@@
	nIdx=0;
	return nuTRUE;
}


nuBOOL	CMapFileTMCDataP::XMLFree()
{

   return nuTRUE;
}

nuBOOL	CMapFileTMCDataP::OpenRoadXML(nuLONG nMapID)
{
	Close();

	nuTCHAR tsFile[NURO_MAX_PATH] = {0};

#ifdef WINCE
	nuTcscpy(tsFile,FILE_XMLR);
	//nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	//nuTcscat(tsFile, FILE_CSC_XMLR);
#else
	nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	nuTcscat(tsFile, FILE_CSC_XMLR);
#endif

	//Open XML_ROAD File
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		//CleanVDData(); //@@@@@@@@@@@@@@@@@@@@@
		return nuFALSE;
	}

	

	return nuTRUE;
}

nuBOOL  CMapFileTMCDataP::RoadXML_Parser ()
{
	//?ѪR²????XML?һݥΨ쪺TAGS
	nuCHAR	cComment[]="Comment";		
	nuCHAR	cCommentE[]="Comment /";
	nuCHAR	cCdata[]="[CDATA[";
	nuCHAR	cArea[]="area";
	nuCHAR	cAreaEND[]="/area";
	nuCHAR	cAreaSn[]="area_sn";
	nuCHAR	cRecord[]="Record";
	nuCHAR	cRecordEnd[]="/Record";
	nuCHAR	cEnd[]="\0";
	nuCHAR	cParserStart[]="\"";

	
	nuWCHAR City[22][4];/*={L"?򶩥?",L"?O?_??",L"?s?_??",L"???鿤",L"?s?˥?",L"?s?˿?",L"?]?߿?",
					   L"?O????",L"???ƿ?",L"?n?뿤",L"???L??",L"?Ÿq??",L"?Ÿq??",L"?O?n??",
					   L"??????",L"?̪F??",L"?O?F??",L"?Ὤ??",L"?y????",L"??????",L"??????",L"?s????"};*/

	if (!OpenRoadXML(0))//ŪRoadXML??
	{
		return 0;
	}

    nuCHAR	TmpWord = 0;
	nuCHAR	TmpData[128] = {0};
	nuCHAR	TempStr[NURO_MAX_PATH*2] = {0};
	
	nuCHAR	*pChr1 = NULL;
	nuCHAR	*pChr2 = NULL;
	nuLONG	lLen=0;
	nuBOOL	bStart=nuTRUE;
	nuINT	nCount=0;
    nuINT	nPos=-1;

	nuINT	nuWordLen = 0;
	nuINT	nRecord=0;
	nuINT	nIndex=0;

	nuBOOL	bCheck=nuTRUE;
	nuBOOL	bParserA=nuFALSE;
	
	nuWCHAR wChangWord[_TMC_UIEVENTNAME_LEN_*2];

	nuMemset(nCityCounts,0,sizeof(nCityCounts));
	/*
	
	?ǥѤW?z??TAG?ѪRDATA?A????TAG???m?H??TAG?????????m(\")    	
	
	?ǥ?nuStrstr???XTAG???_?l???m?A???s?һݪ?LOCATION?BEVENT???T

	?A?ǥ?cLocation??�??TMC?ɤ??????W?BcEvent??�???ƥ????T

	*/

	TMC_XML_C_DATA XmlComplexTemp;
	
	nuINT nIdx=0;

	while(nuFread(&TmpWord, 1, 1, pFile))
	{
		switch(TmpWord)
		{
			case '<':
					bStart=nuTRUE;
					lLen=0;
					nuMemset(TempStr,NULL,NURO_MAX_PATH*2*sizeof(nuCHAR));
					break;
			case '>':
					bStart=nuFALSE;
					break;
			default:
					if(bStart)
					{
						if(lLen<((NURO_MAX_PATH*2)-1))
						{
							TempStr[lLen]=TmpWord;
							lLen++;
						}
						else
						{
							bStart=nuFALSE;
							lLen=0;
						}
					}
					break;
		}

		if(bStart==nuFALSE && lLen!=0)
		{
			nuMemset(&TmpData, 0,sizeof(TmpData));
			if(nuStrstr(TempStr,cRecord)&&lLen==nuStrlen(cRecord))//????LocationInfo?y?z?l?y
			{
				bCheck=nuTRUE;
			}
			if(nuStrstr(TempStr,cComment)&&bCheck&&lLen==nuStrlen(cComment))//????LocationInfo?y?z?l?y
			{
				while(nuFread(&TmpWord, 1, 1, pFile)&&nIndex<_TMC_UIROADNAME_LEN_-1)
				{
					
					TmpData[nIndex]=TmpWord;
					nIndex++;
					if(nuStrstr(TmpData,cCdata))
					{
						break;
					}
				}
				nIndex=0;
				nuMemset(TmpData,0,sizeof(TmpData));
				nuMemset(wChangWord,0,sizeof(wChangWord));
				
                while(nuFread(&TmpWord, 1, 1, pFile)&&nIndex<_TMC_UIEVENTNAMECHAR_LEN_-1)
				{
					if(TmpWord=='>')
						break;
					if(nIndex==0) //?????? ?Y?Ĥ@?Ӧr?쬰?ť?/[ ?N???[???r?ꤤ
						if(TmpWord==' '||TmpWord=='[')
							continue;
					if(TmpWord=='*'||TmpWord==']') //?ư? * ] ???r??
						continue;
					TmpData[nIndex]=TmpWord;
					nIndex++;
				}
				
				//nuMemset(&tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName, 0,sizeof(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName));	
				//nuLONG lLen = MultiByteToWideChar(CP_UTF8,0,TmpData,nuStrlen(TmpData),wChangWord,sizeof(wChangWord));
				nuLONG lLen = nuMbsToWcs(TmpData, wChangWord, sizeof(wChangWord));
				wChangWord[lLen]    =    '\0';  
				//nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName,wChangWord, nIndex);   
				if(lLen>=29)
				{
					wChangWord[29]    =    '\0';  
				}			
				nuMemcpy(XmlComplexTemp.wEvenName,wChangWord, sizeof(XmlComplexTemp.wEvenName)); 
				
				nIndex=0;
				bCheck=nuFALSE;
				bParserA=nuTRUE;
			}
			
			if(nuStrstr(TempStr,cArea)&&lLen==nuStrlen(cArea)&&bParserA)//????Area?y?z?l?y &&!nuStrstr(TempStr,cAreaSn)&&!nuStrstr(TempStr,cAreaEND)
			{
				nuMemset(TmpData, 0,sizeof(TmpData));
				nuMemset(wChangWord,0,sizeof(wChangWord));
				while(nuFread(&TmpWord, 1, 1, pFile)&&nIndex<_TMC_UIROADNAME_LEN_-1)
				{
					if(TmpWord=='<')
						break;
					
					TmpData[nIndex]=TmpWord;
					nIndex++;
				}
				//nuMemset(&tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName, 0,sizeof(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName));			
				//nuLONG lLen = MultiByteToWideChar(CP_UTF8,0,TmpData,nuStrlen(TmpData),wChangWord,sizeof(wChangWord));
				nuLONG lLen = nuMbsToWcs(TmpData, wChangWord, sizeof(wChangWord));
				wChangWord[lLen]    =    '\0';
				//MultiByteToWideChar( CP_UTF8, MB_PRECOMPOSED, TmpData, -1, wChangWord, _TMC_UIROADNAME_LEN_ );
				nuMbsToWcs(TmpData, wChangWord, _TMC_UIROADNAME_LEN_);

				nuINT idx=0;
				for(idx=0;idx<_TMC_UIROADNAME_LEN_;idx++) //?ư? <
				{
					if(wChangWord[idx]=='<')
					{
						wChangWord[idx]=0;
					}
				}
				//nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName,wChangWord, sizeof(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName)-1);   
				nuMemcpy(XmlComplexTemp.wRoadName,wChangWord, sizeof(XmlComplexTemp.wRoadName)); 
				nIndex=0;
				bCheck=nuTRUE;
				bParserA=nuFALSE;
				XmlComplexTemp.nTime=nuGetTickCount();
				if(CheckDataXMLComplex(XmlComplexTemp)||nIdx==0)
				{
					//nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName,XmlComplexTemp.wRoadName, sizeof(XmlComplexTemp.wRoadName)); 
					//nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName,XmlComplexTemp.wEvenName, sizeof(XmlComplexTemp.wEvenName)); 
					
					m_XML_Complex_Data[nIdx]=XmlComplexTemp;
					nuBOOL bCityCheck=nuFALSE;
					for(nuINT i=0;i<22;i++)
					{
						if(nuWcsncmp(m_XML_Complex_Data[nIdx].wRoadName,City[i],nuWcslen(City[i]))==0)
						{
							nCityCounts[i]++;
							bCityCheck=nuTRUE;
						}
					}
					if(!bCityCheck)
						nCityCounts[22]++;
					nIdx++;
					m_nXml_Complex_Record++;
					//m_nArry_idx++;
				}
				
			}

			bStart=nuFALSE;
			lLen=0;
		}
	}
	//file.Close();
	nuFclose(pFile);
	pFile=NULL;
	return nuTRUE;
}


nuBOOL	CMapFileTMCDataP::RoadXMLFree()
{
    if(NULL!=m_XML_Complex_Data)
	{
		delete []m_XML_Complex_Data;
		m_XML_Complex_Data=NULL;
	}
	return nuTRUE;
}

nuBOOL	CMapFileTMCDataP::ShowEvent(nuINT code)
{
	/*

       ?ǥѨƥ?ID ?????ƥ??W??

	   TMC?ɮפ????s?????�r???W??

	*/



	nuINT nCount=0;
	nuBOOL flag=nuFALSE; 
	nCount=-1;

	while(nCount++<TMC_Event_Count)						
	{
		//?????ƥ???ID
		if((nuINT)EventComparData[nCount].wTraffic_Num==(nuINT)code)
		{
			//?N?һݭn??DATA?g?J???c?餤 ?v?ȡB?ƥ??v?T?ɶ??BTMCPASS?B?ƥ??W?١B?ƥ?ID
			nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName,EventComparData[nCount].wEventName,_TMC_UIEVENTNAME_LEN_);
			//nuWcsncpy((nuWCHAR*)tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wEvenName,(nuWCHAR*)EventComparData[nCount].wEventName,_TMC_UIEVENTNAME_LEN_/2);
			m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCRouteWeighting  = EventComparData[nCount].sTraffic_EventWeighting;
			m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCRouteTime       = EventComparData[nCount].sTraffic_EventTime;
			
			m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCEventID            = code;
			tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].nEventID=code;
			flag=nuTRUE;
		}		
		if(flag) //?w???????? ?N?i?H???}???j??
		{
			break;	
		}
	}
	if(!flag)
	{
			m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCRouteWeighting=0;
			m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCRouteTime=0;
			m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCEventID=0;
	}
	return nuTRUE;	
} 

nuBOOL	CMapFileTMCDataP::ShowLocation(nuINT code)
{
	/*
        ?ǥ?Location_ID(code) ??????�??Location???T(?`?I???T?B???W....)

		?ǥ?Location_CODE ???oNODE???????T (?ۮaTMC)

		?ǥ?Location_id ???oLocation???W?? (LocationTable)
	*/

	//?P?_?O?_?????Llocation_id?Ƨ?
	if(bLocationFrist)
	{
		CMapFileTMCP tmcfile;
		if (!tmcfile.Open(0))
		{
			return 0;
		} 
		//Ū?J?Ҧ???location_DATA
		TMC_Header header;
		tmcfile.ReadData(0,&header,sizeof(header)); 
		nTmcLocCount=header.DataCnt;//TMC???????`??
		TMC_dataA=new TMC_DATA[nTmcLocCount]; //?N????Ū?J ???s TMC_dataA[]  ?H?Q???᪺?G?�j?M
		if(TMC_dataA  == NULL)
		{
			return nuFALSE;
		}
		tmcfile.ReadData(header.VertexDataAddr,TMC_dataA,sizeof(TMC_DATA)*(nuINT)header.DataCnt);

		//?i??shell?Ƨ?
		ShellSort();	
		tmcfile.Close();
		
	}

	nLocSize=nTmcLocCount;//TMC???????`??
	nuINT nLocArrayIdx=BinarySearch_TMC_DATA(code);//?i??location_code???m???j?M
	if(nLocArrayIdx==-1)//?j?M???? 
	{
		m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].bTMCPass        = nuFALSE;
		m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].lTMCLocationAddr= 0;
		m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCNodeCount	= 0;
	}
	else//?j?M???\ ?]?w?Ѽ?
	{
		m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].bTMCPass         = nuTRUE;
		m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].lTMCLocationAddr = TMC_dataA[nLocArrayIdx].dLocationVertexAddr;
		m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCNodeCount    = TMC_dataA[nLocArrayIdx].wLocationCodeNum;
	}

	if(bLocationFrist)
	{
		nuTCHAR tsFile[NURO_MAX_PATH] ={0};
		nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
		nuTcscat(tsFile,FILE_NAME_TABLE);

		if (!LocNamefile.Open(tsFile, NURO_FS_RB))
		{
			return nuFALSE;
		}

		nLocTableSize=LocNamefile.GetLength()/sizeof(TMC_LOCATION);//TMC Table???????`??(?ɮפj?p/?C?????ƪ?????)
		LocData=new TMC_LOCATION[nLocTableSize];
		if(LocData  == NULL)
		{
			return nuFALSE;
		}
		LocNamefile.ReadData(0,LocData,sizeof(TMC_LOCATION)*nLocTableSize);//?N????Ū?J ???s LocData[]  ?H?Q???᪺?G?�j?M  
		LocNamefile.Close();
		bLocationFrist=nuFALSE;
	}

	nuINT	nArrayIdx=BinarySearch_TMC_LocName(code);//?i??location_id???m???j?M

	if(nArrayIdx==-1)//?j?M???? 
	{
		m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].lTMCLocationAddr = 0;
		m_TMC_Info_Router.pPTMC_INFORouter[m_nArry_idx].sTMCNodeCount    = 0;
	}
	else//?j?M???\ ?]?w?D???W??
	{
	//	nuWCHAR wChangWord[_TMC_UIROADNAME_LEN_];

	//	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, LocData[nArrayIdx].cLocName, -1, wChangWord, _TMC_UIROADNAME_LEN_ );
		nuMemcpy(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName, LocData[nArrayIdx].cLocName,sizeof(tmc_ui.pPTMC_INFO_LIST[m_nArry_idx].wRoadName)-1);
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::FMFree()
{
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::TMCInfoFree()   //@@@@@@
{
//	m_nFileCount=-1;  

	if(EventComparData)
	{
		delete[] EventComparData;
		EventComparData=NULL;
	}
	
	if(TMC_dataA)
	{
		delete[] TMC_dataA;
		TMC_dataA=NULL;
	}

	if(LocData)
	{
		delete[] LocData;
		LocData=NULL;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::CheckDataTime() //@@@@@@@
{
	if(m_nByte_Record<1)//Temp ?S???Ƥ??i???ɶ??P?_
		return nuTRUE;
	TMC_Byte_DATA *temp_nByte=new TMC_Byte_DATA[_TMC_DATA_LIMIT_];
	if(temp_nByte  == NULL)
	{
		return nuFALSE;
	}
	nuINT n=-1;
	nuINT nTempIdx=0;
	

	if(m_nByte_Record%20==0) //FOR TEST!!!!
	{
		nuINT k=10;

	}

	while(n++<m_nByte_Record) 
	{
		if(m_nByte[n].nTime>m_dnowTime-m_dDistTime) //???ץ????ɶ??@?I?I?I?I?I?I?I?I?I
		{
			temp_nByte[nTempIdx]=m_nByte[n];
			nTempIdx++;
		}
	}

	nuMemset(m_nByte,0,sizeof(m_nByte));
	nuMemcpy(m_nByte, temp_nByte,nTempIdx*sizeof(temp_nByte[0]));
	nuMemset(temp_nByte,0,sizeof(temp_nByte));
	
	m_nByte_Record = nTempIdx;

	if(temp_nByte)
	{
		delete temp_nByte;
		temp_nByte=NULL;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::CheckDataTimeXML()
{
if(m_nXml_Record<1)//Temp ?S???Ƥ??i???ɶ??P?_
		return nuTRUE;
	TMC_Byte_DATA *temp_Xml=new TMC_Byte_DATA[_TMC_DATA_LIMIT_];//
	if(temp_Xml  == NULL)
	{
		return nuFALSE;
	}
	nuINT n=0;
	nuINT nTempIdx=0;
	nuDWORD DnowTime=nuGetTickCount()-m_dDistTime;
	while(n<m_nXml_Record) 
	{
		if(m_XML_Data[n].nTime>DnowTime)
		{
			temp_Xml[nTempIdx]=m_XML_Data[n];
			nTempIdx++;
		
		}
		n++;
		
	}

	nuMemset(m_XML_Data,0,sizeof(m_XML_Data));
	nuMemcpy(m_XML_Data, temp_Xml,(nTempIdx)*sizeof(TMC_Byte_DATA));
	nuMemset(temp_Xml,0,sizeof(temp_Xml));

	m_nXml_Record = nTempIdx;

	if(temp_Xml)
	{
		delete temp_Xml;
		temp_Xml=NULL;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::CheckDataTimeComplexXML()
{
	if(m_nXml_Record<1)//Temp ?S???Ƥ??i???ɶ??P?_
		return nuTRUE;
	TMC_XML_C_DATA *temp_XML=new TMC_XML_C_DATA[_TMC_DATA_LIMIT_];//
	if(temp_XML  == NULL)
	{
		return nuFALSE;
	}
	nuINT n=-1;
	nuINT nTempIdx=0;
	nuDWORD DnowTime=nuGetTickCount()-m_dDistTime;
	while(n++<m_nXml_Record) 
	{
		if(m_XML_Complex_Data[n].nTime>DnowTime)
		{
			temp_XML[nTempIdx]=m_XML_Complex_Data[n];
			nTempIdx++;
		}
		else
		{
			break;
		}
	}

	nuMemset(m_XML_Complex_Data,0,sizeof(m_XML_Complex_Data));
	nuMemcpy(m_XML_Complex_Data, temp_XML,nTempIdx*sizeof(temp_XML[0]));

	nuMemset(temp_XML,0,sizeof(temp_XML));

	m_nXml_Complex_Record = nTempIdx;
	if(temp_XML)
	{
		delete temp_XML;
		temp_XML=NULL;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::CheckData(TMC_Byte_DATA temp)
{
	/*
		?P?_?O?_???ۦP???W ?ۦP?ƥ?

		?Y????DATA ?h?u???s?ɶ?

	*/

	nuINT nIdx=0;
	for(;nIdx<m_nByte_Record;nIdx++)
	{
		if((nuINT)m_nByte[nIdx].nRoad==(nuINT)temp.nRoad)
		{
			if((nuINT)m_nByte[nIdx].nEvent==(nuINT)temp.nEvent)
			{
				m_nByte[nIdx].nTime=temp.nTime;	//*** ?]?ɶ????? ?n???ˤ~?ݭn???s ***
				return nuFALSE;						//???W?[COUNT??
			}
		}	
	
	}	
	
	//m_nByte[m_nByte_Record].nTime=nuGetTickCount();
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::XML_FM_Order(nuINT n)
{
	/*
	XML?Ƨ? ?G?@NULL/ ROAD / EVENT
	*/

	if (n==0)   //???Ƨ?
	{
		return nuFALSE;
	}
	else
	{
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
		nuTcscat(tsFile,FILE_NAME_SORT);
		nuFILE *TempFile;

		TempFile = nuTfopen(tsFile, NURO_FS_RB);
		if( TempFile == NULL )
		{
			return nuFALSE;
		}

		nuFseek(TempFile,0,NURO_SEEK_END);
		nuINT f_size=nuFtell(TempFile)/sizeof(Trans_DATA);
		nSortLimt=f_size;
		data=new Trans_DATA[f_size];
		nuFseek(TempFile,0,NURO_SEEK_SET);
		nuFread(data,1,sizeof(Trans_DATA)*(nuINT)f_size,TempFile);


		nuFclose(TempFile);
		TempFile=NULL;
	}

	TMC_INFO_LIST  temp;
	nuINT nIdx=0;
	nuINT nIdx2=0;

	nuINT k=5;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser = 0;
	nuINT parser2 = 0;

	while(k-->0)
	{
		//CHECK NWORDS:5
		//CONDITION1: 
		//CHECK WORDS_CODES (BIG5)

		//CONDITION2: 
		//K==0&&parser==parser2 
		//CHECK WORDS_LEN

		for(nIdx2=0;nIdx2<m_nXml_Record+m_nByte_Record-1;nIdx2++)
			for(nIdx=0;nIdx<m_nXml_Record+m_nByte_Record-1-nIdx2;nIdx++)
			{
				if(n==1)
				{
					parser=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx].wRoadName[k]);
					parser2=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wRoadName[k]);
					if(parser>parser2)
					{ 
						temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
						tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
						tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

					}
					if(k==0)
					{
						while(tmc_ui.pPTMC_INFO_LIST[nIdx].wRoadName[++nTemp1]!=0)
						{
							if(nTemp1==30)
								break;
						}
						while(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wRoadName[++nTemp2]!=0)
						{
							if(nTemp2==30)
								break;
						}
						if(parser==parser2&&nTemp1>nTemp2)
						{
							temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
							tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
							tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

						}
						nTemp1=-1;
						nTemp2=-1;
					}
				}	
				else
				{
					parser=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx].wEvenName[k]);
					parser2=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wEvenName[k]);
					if(parser>parser2)
					{
						temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
						tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
						tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

					}
					if(k==0)
					{
						while(tmc_ui.pPTMC_INFO_LIST[nIdx].wEvenName[++nTemp1]!=0)
						{
							if(nTemp1==30)
								break;
						}
						while(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wEvenName[++nTemp2]!=0)
						{
							if(nTemp2==30)
								break;
						}
						if(parser==parser2&&nTemp1>nTemp2)
						{
							temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
							tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
							tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

						}
						nTemp1=-1;
						nTemp2=-1;
					}
				}

			}	
	}
	if(data)
	{
       delete[] data;
	   data=NULL;
	}
	return nuTRUE;

}

nuBOOL	CMapFileTMCDataP::XML_Order(nuINT n)
{
	/*
	    XML?Ƨ? ?G?@NULL/ ROAD / EVENT
	*/

	if (n==0)   //???Ƨ?
	{
		return nuFALSE;
	}
	else
	{
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
		nuTcscat(tsFile,FILE_NAME_SORT);
		nuFILE *TempFile;

		TempFile = nuTfopen(tsFile, NURO_FS_RB);
		if( TempFile == NULL )
		{
			return nuFALSE;
		}

		nuFseek(TempFile,0,NURO_SEEK_END);
		nuINT f_size=nuFtell(TempFile)/sizeof(Trans_DATA);
		nSortLimt=f_size;
		data=new Trans_DATA[f_size];
		nuFseek(TempFile,0,NURO_SEEK_SET);
		nuFread(data,1,sizeof(Trans_DATA)*(nuINT)f_size,TempFile);

	

		nuFclose(TempFile);
		TempFile=NULL;
	}

	TMC_INFO_LIST  temp;
	nuINT nIdx=0;
	nuINT nIdx2=0;

	nuINT k=5;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser = 0;
	nuINT parser2 = 0;

	while(k-->0)
	{
		//CHECK NWORDS:5
		//CONDITION1: 
		//CHECK WORDS_CODES (BIG5)

		//CONDITION2: 
		//K==0&&parser==parser2 
		//CHECK WORDS_LEN

		for(nIdx2=0;nIdx2<m_nXml_Record-1;nIdx2++)
			for(nIdx=0;nIdx<m_nXml_Record-1-nIdx2;nIdx++)
			{
				if(n==1)
				{
					parser=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx].wRoadName[k]);
					parser2=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wRoadName[k]);
					if(parser>parser2)
					{ 
						temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
						tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
						tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

					}
					if(k==0)
					{
						while(tmc_ui.pPTMC_INFO_LIST[nIdx].wRoadName[++nTemp1]!=0)
						{
							if(nTemp1==30)
								break;
						}
						while(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wRoadName[++nTemp2]!=0)
						{
							if(nTemp2==30)
								break;
						}
						if(parser==parser2&&nTemp1>nTemp2)
						{
							temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
							tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
							tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

						}
						nTemp1=-1;
						nTemp2=-1;
					}
				}	
				else
				{
					parser=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx].wEvenName[k]);
					parser2=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wEvenName[k]);
					if(parser>parser2)
					{
						temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
						tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
						tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

					}
					if(k==0)
					{
						while(tmc_ui.pPTMC_INFO_LIST[nIdx].wEvenName[++nTemp1]!=0)
						{
							if(nTemp1==30)
								break;
						}
						while(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wEvenName[++nTemp2]!=0)
						{
							if(nTemp2==30)
								break;
						}
						if(parser==parser2&&nTemp1>nTemp2)
						{
							temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
							tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
							tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

						}
						nTemp1=-1;
						nTemp2=-1;
					}
				}
				
			}	
	}
	if(data)
	{
		delete[] data;
		data=NULL;
	}
	return nuTRUE;

}
nuBOOL	CMapFileTMCDataP::FM_Order(nuINT n)
{
	/*
		FM ?Ƨ? ?G?@NULL/ ROAD / EVENT
	*/

	if (n==0) //?????Ƨ?
	{
		return nuFALSE;
	}
	else
	{
		nuTCHAR tsFile[NURO_MAX_PATH];
		nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
		nuTcscat(tsFile,FILE_NAME_SORT);
		nuFILE *TempFile;

		TempFile = nuTfopen(tsFile, NURO_FS_RB);
		if( TempFile == NULL )
		{
			return nuFALSE;
		}

		nuFseek(TempFile,0,NURO_SEEK_END);
		nuINT f_size=nuFtell(TempFile)/sizeof(Trans_DATA);
		nSortLimt=f_size;
		data=new Trans_DATA[f_size];
		nuFseek(TempFile,0,NURO_SEEK_SET);
		nuFread(data,1,sizeof(Trans_DATA)*(nuINT)f_size,TempFile);

	

		nuFclose(TempFile);
		TempFile=NULL;
	}

	TMC_INFO_LIST  temp;
	nuINT nIdx=0;
	nuINT nIdx2=0;

	nuINT k=5;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser = 0;
	nuINT parser2 = 0;

	while(k-->0)
	{
		//CHECK NWORDS:5
		//CONDITION1: 
		//CHECK WORDS_CODES (BIG5)

		//CONDITION2: 
		//K==0&&parser==parser2 
		//CHECK WORDS_LEN

		for(nIdx2=0;nIdx2<m_nByte_Record-1;nIdx2++)
			for(nIdx=0;nIdx<m_nByte_Record-1-nIdx2;nIdx++)
			{
				if(n==1)
				{
					parser=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx].wRoadName[k]);
					parser2=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wRoadName[k]);
					if(parser>parser2)
					{
						temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
						tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
						tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

					}
					if(k==0)
					{
						while(tmc_ui.pPTMC_INFO_LIST[nIdx].wRoadName[++nTemp1]!=0)
						{
							if(nTemp1==30)
								break;
						}
						while(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wRoadName[++nTemp2]!=0)
						{
							if(nTemp2==30)
								break;
						}
						if(parser==parser2&&nTemp1>nTemp2)
						{
							temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
							tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
							tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

						}
						nTemp1=-1;
						nTemp2=-1;
					}
				}	
				else
				{
					parser=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx].wEvenName[k]);
					parser2=TransCode(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wEvenName[k]);
					if(parser>parser2)
					{
						temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
						tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
						tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

					}
					if(k==0)
					{
						while(tmc_ui.pPTMC_INFO_LIST[nIdx].wEvenName[++nTemp1]!=0)
						{
							if(nTemp1==30)
								break;
						}
						while(tmc_ui.pPTMC_INFO_LIST[nIdx+1].wEvenName[++nTemp2]!=0)
						{
							if(nTemp2==30)
								break;
						}
						if(parser==parser2&&nTemp1>nTemp2)
						{
							temp=tmc_ui.pPTMC_INFO_LIST[nIdx];
							tmc_ui.pPTMC_INFO_LIST[nIdx]=tmc_ui.pPTMC_INFO_LIST[nIdx+1];
							tmc_ui.pPTMC_INFO_LIST[nIdx+1]=temp;

						}
						nTemp1=-1;
						nTemp2=-1;
					}
				}

			}	
	}
	if(data)
	{
		delete[] data;
		data=NULL;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::XML_COMPLEX_Order(nuINT n)
{
	/*
	?P?_?O?_???ۦP???W ?ۦP?ƥ?

	?Y????DATA ?h?u???s?ɶ?

	*/

	if (n==0)
	{
		return nuFALSE;
	}
	else
	{
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
			nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
			nuTcscat(tsFile,FILE_NAME_SORT);
			nuFILE *TempFile;

			TempFile = nuTfopen(tsFile, NURO_FS_RB);
			if( TempFile == NULL )
			{
				return nuFALSE;
			}

			nuFseek(TempFile,0,NURO_SEEK_END);
			nuINT f_size=nuFtell(TempFile)/sizeof(Trans_DATA);
			nSortLimt=f_size;
			data=new Trans_DATA[f_size];
			nuFseek(TempFile,0,NURO_SEEK_SET);
			nuFread(data,1,sizeof(Trans_DATA)*(nuINT)f_size,TempFile);

		

		nuFclose(TempFile);
		TempFile=NULL;
	}

	TMC_XML_C_DATA  temp;
	nuINT nIdx=0;
	nuINT nIdx2=0;
	
	nuINT k=5;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser=0;
	nuINT parser2=0;

	while(k-->0)
	{
		//CHECK NWORDS:5
		//CONDITION1: 
		//CHECK WORDS_CODES (BIG5)

		//CONDITION2: 
		//K==0&&parser==parser2 
		//CHECK WORDS_LEN


	for(nIdx2=0;nIdx2<m_nXml_Complex_Record-1;nIdx2++)
		for(nIdx=0;nIdx<m_nXml_Complex_Record-1-nIdx2;nIdx++)
		{
			if(n==1)
			{
				
				parser=TransCode(m_XML_Complex_Data[nIdx].wEvenName[k]);
				parser2=TransCode(m_XML_Complex_Data[nIdx+1].wEvenName[k]);

				if(parser>parser2)
				{
					temp=m_XML_Complex_Data[nIdx];
					m_XML_Complex_Data[nIdx]=m_XML_Complex_Data[nIdx+1];
					m_XML_Complex_Data[nIdx+1]=temp;

				}
				if(k==0)
				{
					while(m_XML_Complex_Data[nIdx].wEvenName[++nTemp1]!=0)
					{
						if(nTemp1==30)
							break;
					}
					while(m_XML_Complex_Data[nIdx+1].wEvenName[++nTemp2]!=0)
					{
						if(nTemp2==30)
							break;
					}

					if(parser==parser2&&nTemp1>nTemp2)
					{
						temp=m_XML_Complex_Data[nIdx];
						m_XML_Complex_Data[nIdx]=m_XML_Complex_Data[nIdx+1];
						m_XML_Complex_Data[nIdx+1]=temp;
					}
					nTemp1=-1;
					nTemp2=-1;

				}
			}	
			else
			{
				parser=TransCode(m_XML_Complex_Data[nIdx].wRoadName[k]);
				parser2=TransCode(m_XML_Complex_Data[nIdx+1].wRoadName[k]);
				if(parser>parser2)
				{
					temp=m_XML_Complex_Data[nIdx];
					m_XML_Complex_Data[nIdx]=m_XML_Complex_Data[nIdx+1];
					m_XML_Complex_Data[nIdx+1]=temp;

				}
				if(k==0)
				{
					while(m_XML_Complex_Data[nIdx].wRoadName[++nTemp1]!=0)
					{
						if(nTemp1==30)
							break;
					}
					while(m_XML_Complex_Data[nIdx+1].wRoadName[++nTemp2]!=0)
					{
						if(nTemp2==30)
							break;
					}

					if(parser==parser2&&nTemp1>nTemp2)
					{
						temp=m_XML_Complex_Data[nIdx];
						m_XML_Complex_Data[nIdx]=m_XML_Complex_Data[nIdx+1];
						m_XML_Complex_Data[nIdx+1]=temp;
					}
					nTemp1=-1;
					nTemp2=-1;
				}
			}
			
		}	
	}

	if(data)
	{
		delete[] data;
		data=NULL;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::VD_Order(nuINT n)
{
	/*
	?P?_?O?_???ۦP???W ?ۦP?ƥ?

	?Y????DATA ?h?u???s?ɶ?

	*/

	if (n==0)
	{
		return nuFALSE;
	}
	else
	{
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
		nuTcscat(tsFile,FILE_NAME_SORT);
		nuFILE *TempFile;

		TempFile = nuTfopen(tsFile, NURO_FS_RB);
		if( TempFile == NULL )
		{
			return nuFALSE;
		}

		nuFseek(TempFile,0,NURO_SEEK_END);
		nuINT f_size=nuFtell(TempFile)/sizeof(Trans_DATA);
		nSortLimt=f_size;
		data=new Trans_DATA[f_size];
		nuFseek(TempFile,0,NURO_SEEK_SET);
		nuFread(data,1,sizeof(Trans_DATA)*(nuINT)f_size,TempFile);

	
		nuFclose(TempFile);
		TempFile=NULL;
	}


	TMC_XML_C_DATA  temp;
	nuINT nIdx=0;
	nuINT nIdx2=0;
	
	nuINT k=5;
	nuINT nTemp1=-1;
	nuINT nTemp2=-1;
	nuINT parser=0;
	nuINT parser2=0;

	while(k-->0)
	{
		//CHECK NWORDS:5
		//CONDITION1: 
		//CHECK WORDS_CODES (BIG5)

		//CONDITION2: 
		//K==0&&parser==parser2 
		//CHECK WORDS_LEN


	for(nIdx2=0;nIdx2<nVD_Event_Count-1;nIdx2++)
		for(nIdx=0;nIdx<nVD_Event_Count-1-nIdx2;nIdx++)
		{
			if(n==1)
			{
				parser=TransCode(m_VD_Data[nIdx].wEvenName[k]);
				parser2=TransCode(m_VD_Data[nIdx+1].wEvenName[k]);

				if(parser>parser2)
				{
					temp=m_VD_Data[nIdx];
					m_VD_Data[nIdx]=m_VD_Data[nIdx+1];
					m_VD_Data[nIdx+1]=temp;

				}
				if(k==0)
				{
					while(m_VD_Data[nIdx].wEvenName[++nTemp1]!=0)
					{
						if(nTemp1==30)
							break;
					}
					while(m_VD_Data[nIdx+1].wEvenName[++nTemp2]!=0)
					{
						if(nTemp2==30)
							break;
					}

					if(parser==parser2&&nTemp1>nTemp2)
					{
							temp=m_VD_Data[nIdx];
							m_VD_Data[nIdx]=m_VD_Data[nIdx+1];
							m_VD_Data[nIdx+1]=temp;
					}
					nTemp1=-1;
					nTemp2=-1;

				}
			}	
			else
			{
				parser=TransCode(m_VD_Data[nIdx].wRoadName[k]);
				parser2=TransCode(m_VD_Data[nIdx+1].wRoadName[k]);
				if(parser>parser2)
				{
					temp=m_VD_Data[nIdx];
					m_VD_Data[nIdx]=m_VD_Data[nIdx+1];
					m_VD_Data[nIdx+1]=temp;

				}
				if(k==0)
				{
					while(m_VD_Data[nIdx].wRoadName[++nTemp1]!=0)
					{
						if(nTemp1==30)
							break;
					}
					while(m_VD_Data[nIdx+1].wRoadName[++nTemp2]!=0)
					{
						if(nTemp2==30)
							break;
					}

					if(parser==parser2&&nTemp1>nTemp2)
					{
						temp=m_VD_Data[nIdx];
						m_VD_Data[nIdx]=m_VD_Data[nIdx+1];
						m_VD_Data[nIdx+1]=temp;
					}
					nTemp1=-1;
					nTemp2=-1;
				}
			}
			
		}	
	}
	if(data)
	{
		delete[] data;
		data=NULL;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::CheckDataX(TMC_Byte_DATA temp)
{
	/*
	?P?_?O?_???ۦP???W ?ۦP?ƥ?

	?Y????DATA ?h?u???s?ɶ?

	*/

	nuINT nIdx=0;
	for(;nIdx<m_nXml_Record;nIdx++)
	{
		if(m_XML_Data[nIdx].nRoad==temp.nRoad)
		{
			if(m_XML_Data[nIdx].nEvent==temp.nEvent)
			{
				m_XML_Data[nIdx].nTime=temp.nTime;	//*** ?]?ɶ????? ?n???ˤ~?ݭn???s ***
				return nuFALSE;
			}
		}	
	}	

	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::CheckDataXMLComplex(TMC_XML_C_DATA temp)
{
	/*
	?P?_?O?_???ۦP???W ?ۦP?ƥ?

	?Y????DATA ?h?u???s?ɶ?

	*/

	nuINT nIdx=0;
	for(;nIdx<m_nXml_Complex_Record;nIdx++)
	{
		if(nuWcscmp(m_XML_Complex_Data[nIdx].wRoadName,temp.wRoadName)==0)
			if(nuWcscmp(m_XML_Complex_Data[nIdx].wEvenName,temp.wEvenName)==0)
			{
				m_XML_Complex_Data[nIdx].nTime=temp.nTime;
				return nuFALSE;
			}
	}	

	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::Set_TMC_Byte(nuVOID*pTmcBuf) //@@@@@@@@@@@@@@@@@@@@
{ 
	//NO USE FROM FM
   return nuFALSE;

	nuINT z=0;
	//?ѪRFM?? TMCDAT
	nuINT	nReadCount=-1;
	nuINT	n_word=0;						//Ū?J??N?r
	nuCHAR	*sTmpWord=(nuCHAR*)pTmcBuf;					//?Ȧs?r??	buffer
	nuCHAR	sHeaderTag[]="A5A5";			//????		Header 
	nuCHAR	sTailTag[]	="5A5A";			//????		Tail
	
	nuINT	nDataPos=0;						//Ū?J?@?? cTmc_information
	nuINT	nPos=0;							//?r?դ????ĴX?Ӧr(?@?ը??Ӧr)
	nuINT	nNums=0;						//?PsHeaderTag[]?i?????? 4?N???r???@?P
	nuINT	n_idx=-1;						//sTmpWord[]-index

	TMC_Byte_DATA	ByteTemp;
		
	if(m_nByte_Record==_TMC_CHECK_DATA_COUNT) //?W?L?W?? ?簣?W?ɸ??T ?ɶ??n?D???Y??
	{
		//CheckDataTime(); //?W?[?ɶ??????Ѽ? @@@@@
		return nuTRUE;
	}

	if(!m_bLeftWord)		
	{	
		m_nLeftWords=4;
	}	

		while(n_idx++<3)	//?N?????쪺sTmpWord[] ???JsCheckStr[]
		{
			nuINT i=0;
			nuINT j=0;
			if(m_nWords<4)
			{			
				if(!m_bLeftWord)				//????1.
				{
					m_nLeft=0;
					if(sTmpWord[n_idx]>32)						//?ťեH?~???~?n?Ѵ?
					{
						m_sCheckStr[m_nWords]=sTmpWord[n_idx];
						m_nWords++;		//sCheckStr[] ?r??++
					
					}
					m_nLeftWords--;	//sLeftWord[] ?r??--
					if(m_nLeftWords<1)	//sLeftWord[] ?S??DATA ?A?}?lŪ?? sTmpWord[] /*****?g?k?ǩǪ??ݭץ? @@@@@@
					{
						m_bLeftWord=nuFALSE;
						
					}
				}
				else									//????2.
				{
					if(m_nLeftWords<1)
						break;
					m_sCheckStr[m_nWords]=m_sLeftWord[m_nLeft];
					m_nLeft++;
					m_nWords++;		//sCheckStr[] ?r??++
					if(m_nLeft==3)
					{
						m_bLeftWord=nuFALSE;
						n_idx=0;
						nuMemset(m_sLeftWord,0,3);
					}
				}
			
			}
			/*
				????Head_Data A5A5
			*/
			if(m_nWords==4&&!m_bCheckHeader)	//?P?_?}?Y ?O?_???n?Ѵ?????
			{
				nuStrncpy(m_sCheckWord,m_sCheckStr,4);
				nNums=m_nWords;
				ShiftWords(m_sCheckWord,&nNums,0);		//????WORD??SHIFT
				if(nNums==4)
				{
					m_bCheckHeader=nuTRUE;
					//???Ѿl???r???? sLeftWord[]
					nuMemset(m_sLeftWord,0,3);
					
					nuINT j=4-m_nLeftWords;
					nuINT i=j-1;
					nuINT n=1;
					m_bLeftWord=nuFALSE;
					m_nLeftWords=0;
					m_nLeft=0;
					for(;i<j&&j<4;i++,j++)
					{
						while(sTmpWord[j]<33)
						{
							
							if(j>=3)
								break;
							j++;
						}
						if(sTmpWord[j]>33)
						{
							m_sLeftWord[i]=sTmpWord[j];
							m_nLeftWords=n++;
							m_nLeft=m_nLeftWords;
							m_bLeftWord=nuTRUE;
						}
					}
				}
				else
				{
					if(nNums==0)
					{
						nuMemset(m_sCheckStr,0,sizeof(m_sCheckStr));
						m_nWords=0;

					}
					if(nNums>0)
					{
						//nuMemcpy(sCheckStr,sCheckWord,nNums);
						m_bCheckHeader=nuFALSE;
						nuMemcpy(m_sCheckStr,m_sCheckWord,sizeof(m_sCheckStr));

						//idx=0;
						m_nWords=nNums;
						
					}
					if(m_nWords==0)
					{
						nuMemset(m_sTmc_information,0,sizeof(m_sTmc_information));
						nuMemset(m_sCheckStr,0,sizeof(m_sCheckStr));

						m_nWords=0;
						break;
					}
				}
			}
		}
		if(m_bCheckHeader&&m_nWords<26)
		{
			if(m_bLeftWord)
			{	
				nuINT i=0;
				nuINT j=0;
				nuINT nCount=m_nLeftWords;
				for(;j<nCount;j++)
				{
					m_sCheckStr[m_nWords]=m_sLeftWord[j];
					m_nWords++;
					m_nLeftWords--;
				}
				nuMemset(m_sLeftWord,0,3);
				m_bLeftWord=nuFALSE;
			}
			else
			{
				nuINT i=0;
				nuINT j=0;
				j=-1;
				for(;j<4;j++)
				{
					if(m_nLeftWords<0)
						break;
					m_nLeftWords--;
					

					if(sTmpWord[j]>32)//&&nLeftWords<1)//?ťեH?~???~?n?Ѵ?
					{
						m_sCheckStr[m_nWords]=sTmpWord[j];
						m_nWords++;
					}
					if(m_nWords==26)
					{
						m_nLeftWords=3-j;
						
						break;
					}
				}
				if(m_nWords==26)
				{
					//bCheck=nuTRUE;
					//???Ѿl???r???? sLeftWord[]
					nuMemset(m_sLeftWord,0,3);
					nuINT i=0;
					nuINT j=4-m_nLeftWords;
					m_bLeftWord=nuFALSE;
					for(;i<m_nLeftWords&&j<4;i++)
					{
						while(sTmpWord[j]<33)
						{
							j++;
							if(j==3)
								break;
						}
						if(j<4&&sTmpWord[j]>33)
						{
							m_sLeftWord[i]=sTmpWord[j];
							m_bLeftWord=nuTRUE;
						}
						j++;
					}
				}
			}
		}
		if(m_nWords==26)
		{
			
			if(m_sCheckStr[22]=='5'&&m_sCheckStr[23]=='A'&&m_sCheckStr[24]=='5'&&m_sCheckStr[25]=='A')
			{
				m_bCorrect=nuTRUE;
				m_bLeft=nuFALSE;
			}
			else
			{
				ShiftWords(m_sCheckStr,&m_nWords,1);
				nuINT idx=0;
				if(m_bLeftWord)
				{
				for(;idx<m_nLeftWords;idx++)
				{
					if(m_sLeftWord[idx]>33)
					{
					  m_sCheckStr[m_nWords]=m_sLeftWord[idx];
					  m_nWords++;
					}
				}
				m_nLeftWords=0;
				m_bLeftWord=nuFALSE;
				}
			
				if(m_nWords>0)
				{
					m_bLeft=nuTRUE;
				    m_bCorrect=nuFALSE;
				}
				else
				{
					m_bLeft=nuFALSE;
				}
				
				
			}
			if(m_bCorrect)
			{
				nuINT nrows=0;
				nuINT ncols=0;
				nuINT nWords=0;

				for(;nrows<13;nrows++)
					for(ncols=0;ncols<2;ncols++,nWords++)
						m_sTmc_information[nrows][ncols]=m_sCheckStr[nWords];
				
				nDataPos=-1;
				while(nDataPos++<12)      //?ഫANSI (read txt)
				{                         
					for(nuINT nCount=0;nCount<2;nCount++)
					{
						if(m_sTmc_information[nDataPos][nCount]<58)
						{
							m_nTmc_information2[nDataPos][nCount]=(m_sTmc_information[nDataPos][nCount]-48); 
						}
						else
						{
							m_nTmc_information2[nDataPos][nCount]=(m_sTmc_information[nDataPos][nCount]-48-7);								
						}
					}
				}

				//?P?_?O?_?????T???T check_sum ???쪺???T???T ?~?i???U?C???ѪR
				nuINT nError=0;
				nDataPos=1; //2~9 check
				while(nDataPos++<9)
				{
					nError^=((m_nTmc_information2[nDataPos][0])*16+m_nTmc_information2[nDataPos][1]);
				}
				if(nError==m_nTmc_information2[10][0]*16+m_nTmc_information2[10][1])
				{
					m_bCorrect=nuTRUE;
				}
				else
				{	
					m_bCorrect=nuFALSE;
				}
				
			}
			
			if(m_bCorrect)
			{
				
				if(1)
				{	
					ByteTemp.nTime=nuGetTickCount();
				//	m_dDistTime=ByteTemp.nTime;       //???ץ??@?I?I?I?I?I?I?I?I?I?I?I
				
					// TMC?ѪR  B BLOCK ???W???Ш?TMC???? ?ѼƦP???󤺮e
					nuINT	nTmcData[16]={0};
					nuINT	n_InformationString[8]={0};
					nuINT	nTh=0;
					nuINT	nPos = 0;
					nDataPos=5;

					//?ഫ???G?i?? ???ܼƨϥ?
					/*for (nPos = 7; nPos>-1; nPos--)
					{
						n_InformationString[nPos] = nTmc_information2[nDataPos][1]%2;
						nTmc_information2[nDataPos][1] = nTmc_information2[nDataPos][1]/2;   
					}
					nuINT	n_T =n_InformationString[3];
					nuINT	n_F =n_InformationString[4];
					nuINT	n_DP=n_InformationString[7]+n_InformationString[6]*2+n_InformationString[5]*4;
					*/
					//tmc_router[m_nArry_idx].sTMCTime=n_DP;  //?ثe?wTMC?????ѼƳ]?w ???H?????T

					// TMC?ѪR  C BLOCK ???W???Ш?TMC???? ?ѼƦP???󤺮e
					nDataPos=5;
					while(nDataPos++<7)
					{
						nuINT nPos;
						//?ഫ???G?i?? ???ܼƨϥ?
						for (nPos = 3; nPos>-1; nPos--)
						{
							nTmcData[4*nTh+nPos] = m_nTmc_information2[nDataPos][0]%2;
							m_nTmc_information2[nDataPos][0] = m_nTmc_information2[nDataPos][0]/2;        
						} 
							nTh++;
						for (nPos = 3; nPos>-1; nPos--)
						{
							nTmcData[4*nTh+nPos] = m_nTmc_information2[nDataPos][1]%2;
							m_nTmc_information2[nDataPos][1] = m_nTmc_information2[nDataPos][1]/2;   
						} 
							nTh++;
					}

					//?]?wC?ϰѼ? ???W???Ш?TMC???? ?ѼƦP???󤺮e
					nuINT	n_Note=nTmcData[0];
					nuINT	n_Direction=nTmcData[1];
					nuINT	n_Eextent[3];
					
					nuINT n_Pos;
					/*	for(n_Pos=0;n_Pos<3;n_Pos++)
					{
						n_Eextent[n_Pos]=nTmcData[n_Pos+2];
					}*/
					nuINT	nEvent[11];
					for(n_Pos=0;n_Pos<11;n_Pos++)
					{
						nEvent[n_Pos]=nTmcData[n_Pos+5];
					}
					nuINT	nSum=0;
					nuINT	n_Level;

					//?p???G?i?nEvent[] ???T ?ഫ???Q?i??
					for (n_Pos=10,n_Level=1; n_Pos>-1; n_Pos--)
					{
						nSum+=nEvent[n_Pos]*n_Level;
						n_Level = n_Level * 2;       
					} 
						
					ByteTemp.nEvent=nSum;//*** ???o?ƥ????T ***

					// TMC?ѪR  D BLOCK ???W???Ш?TMC???? ?ѼƦP???󤺮e
					nuINT	n_LocationData[16]={0};
					nDataPos=7;
					nTh=0;

					while(nDataPos++<9)
					{
						nuINT ii;
						//?ഫ???G?i?? ???ܼƨϥ?
						for (ii = 3; ii>-1; ii--)
						{
							n_LocationData[4*nTh+ii] = m_nTmc_information2[nDataPos][0]%2;
							m_nTmc_information2[nDataPos][0] = m_nTmc_information2[nDataPos][0]/2;        
						} 
						nTh++;
						for (ii = 3; ii>-1; ii--)
						{
							n_LocationData[4*nTh+ii] = m_nTmc_information2[nDataPos][1]%2;
							m_nTmc_information2[nDataPos][1] = m_nTmc_information2[nDataPos][1]/2;       
						} 
						nTh++;
					}
					nSum=0;

					//?p???G?i?n_LocationData[] ???T ?ഫ???Q?i??
					for (n_Pos=15,n_Level=1; n_Pos>-1; n_Pos--)
					{
						nSum+=n_LocationData[n_Pos]*n_Level;
						n_Level = n_Level * 2;    
					} 
					ByteTemp.nRoad=nSum; //*** ???o???m???T	***
					

					if(CheckData(ByteTemp)||m_nByte_Record==0)
					{
						m_nByte[m_nByte_Record]=ByteTemp;
						m_nByte_Record++;      //????+1
						m_nXml_fm_Record++;
						
						m_dnowTime=ByteTemp.nTime;
						//CheckDataTime();
						
					}

					m_bCorrect=nuFALSE;
					}
				}
				if(!m_bCorrect&&!m_bLeft) //?????T???T ?M?? ???s?[?J m_sTmc_information[]
				{
					/*	init parser() */	

					m_bCheckHeader=nuFALSE;
					m_bCorrect=nuFALSE;
					nuMemset(m_sCheckStr,0,sizeof(m_sCheckStr));
					nuMemset(m_sTmc_information,0,sizeof(m_sTmc_information));
					m_nWords=0;
				}
			}
			n_idx=-1;
	//file.Close();
	return nuTRUE;
}
nuBOOL CMapFileTMCDataP::ShiftWords(nuVOID* pTmcBuf,nuINT* nNums,nuINT nmode)
{

	char	sComparedWords[]="A5A5";
	char	sChangeWords[4] = {0};
	char	*sChangeWords2 = NULL;
	
	char*	pWord = (char*)pTmcBuf;
	nuINT	nIdx=*nNums;
	nuINT	nWordIdx=0;
	nuBOOL	bSame=nuFALSE;

	if(nmode==0)
	{
		while(!bSame&&nIdx>0&&nmode==0)
		{
			if(nuStrncmp(sComparedWords,pWord,nIdx)!=0) //????0~3?r ?O?_???ŦX sComparedWords[]???l?r??
			{
				nIdx--;
				nuMemset(sChangeWords,0,4);

				for(nWordIdx=0;nWordIdx<nIdx;nWordIdx++)
				{
					sChangeWords[nWordIdx]=pWord[nWordIdx+1];
				}
			
				nuMemcpy(pWord,sChangeWords,4);
			}
			else
			{
				bSame=nuTRUE;	
			}		
		}
		*nNums=nIdx;
	}

	if(nmode==1)
	{
		nuCHAR	*pChr1 = NULL;
		nuCHAR	*pChr2 = NULL;
		sChangeWords2 =new char[26];
		if(sChangeWords2  == NULL)
		{
			return nuFALSE;
		}
		nuMemcpy(sChangeWords2,pWord,26);
		
		pChr1=nuStrstr(sChangeWords2,sComparedWords);
		pChr2=nuStrstr(sChangeWords2+4,sComparedWords);

		if((pChr2-pChr1)>0)								//?????r?? ?O?_???ŦX sComparedWords[]???l?r??
		{
			*nNums=26-(nuStrlen(pChr1)-nuStrlen(pChr2));
			nuMemset(&m_sCheckStr,0,26);
			nuMemcpy(m_sCheckStr,pChr2,nuStrlen(pChr2));
		}
		else
		{
			*nNums=0;
		}
	}	
	
	
	if(sChangeWords2)
	{

		delete[] sChangeWords2;
		sChangeWords2=NULL;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::IsOpen()
{
	return file.IsOpen();
}

nuVOID	CMapFileTMCDataP::Close()
{
	file.Close();
	totalcount = 0;
}
nuUINT	CMapFileTMCDataP::GetLength()
{
	return file.GetLength();
}
nuBOOL	CMapFileTMCDataP::ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len)
{
	return file.ReadData(addr, pBuffer, len);
}
nuVOID	CMapFileTMCDataP::ShellSort(){

	//Shell Sort ?i?? location_ID ?Ƨ?

	TMC_DATA tempTmcDataA = {0};		  // ?ȦsDATA ?H?i?????ƥ洫??
	nuINT gap = nTmcLocCount / 2; // ?????Z?? ?H/2 ?v???Y??
	nuINT k = 0;
	nuINT i = 0;
	nuINT j = 0;

	while(gap > 0) 
	{ 	
		for(k = 0; k < gap; k++) 
		{ 
			for(i = k+gap; i < nTmcLocCount; i+=gap) 
			{ 
				for(j = i - gap; j >= k; j-=gap) 
				{ 
					if(TMC_dataA[j].wLocationCode > TMC_dataA[j+gap].wLocationCode) //SWAP
					{ 
						tempTmcDataA=TMC_dataA[j];
						TMC_dataA[j]=TMC_dataA[j+gap];
						TMC_dataA[j+gap]=tempTmcDataA; 
					} 
					else 
					{
						break; 
					}
				} 
			} 
		} 
		gap /= 2; 
	} 
	
}
nuINT	CMapFileTMCDataP::BinarySearch_TMC_LocName(nuINT find) { 
    
	//?G?�j?M?k ?ΨӤ???Location_Code??
	nuINT	low = 0; 
	nuINT	upper = 0; 
	nuINT	lLocation = 0;//?????μƭ?
	nuCHAR	*pEdn = NULL;	  //?ഫ???ƥ?	
	
	low	  = 0;
	upper = nLocTableSize-1;

	//?Y???諸???Ƥ??blow~upper?d?? ?N???Τ??? ?????^??-1

	if(find<LocData[low].cLocID) 
	{
		return -1;
	}
	if(find>LocData[upper].cLocID)
	{
		return -1;
	}

	while(low <= upper) 
	{ 
		nuINT mid = (low+upper) / 2;
		lLocation = LocData[mid].cLocID;
		
		if(lLocation < find) 
		{
			low = mid+1; 
		}
		else if(lLocation > find) 
		{	
			upper = mid - 1; 
		}
		else 
		{    
			return mid;
		}
	} 
    return -1; 
} 
nuINT	CMapFileTMCDataP::BinarySearch_TMC_DATA(nuINT find) { 
    
	//?G?�j?M?k ?ΨӤ???Location_ID??
	nuINT	low = 0; 
	nuINT	upper = 0; 
	nuINT	lLocation = 0;//?????μƭ?
	nuCHAR	*pEdn = NULL;	  //?ഫ???ƥ?	
		
	low	  = 0;
	upper = nLocSize - 1;	

	//?Y???諸???Ƥ??blow~upper?d?? ?N???Τ??? ?????^??-1
	if(find>TMC_dataA[upper].wLocationCode) 
	{
		return -1;
	}
	if(find<TMC_dataA[low].wLocationCode)
	{
		return -1;
	}

	while(low <= upper) 
	{ 
		nuINT mid = (low+upper) / 2;
		lLocation = TMC_dataA[mid].wLocationCode;

		if(lLocation < find) 
		{
			low = mid+1; 
		}
		else if(lLocation  > find) 
		{	
			upper = mid - 1; 
		}
		else 
		{    
			return mid;
		}
	} 
    return -1; 
} 
nuBOOL	CMapFileTMCDataP::ALLFree(){

	if(tmc_ui.nCount>0)
	{
		if(NULL!=tmc_ui.pPTMC_INFO_LIST)
		{
			delete[] tmc_ui.pPTMC_INFO_LIST;
			tmc_ui.pPTMC_INFO_LIST=NULL;
		}
	tmc_ui.nCount=-1;
	}

	if(m_TMC_Info_Router.nCount>0)
	{
		if(NULL!=m_TMC_Info_Router.pPTMC_INFORouter)
		{
			delete[] m_TMC_Info_Router.pPTMC_INFORouter;
			m_TMC_Info_Router.pPTMC_INFORouter=NULL;
		}
		m_TMC_Info_Router.nCount=-1;
	}
	return nuTRUE;
}
nuBOOL	CMapFileTMCDataP::SetEvent(nuINT nEventSet[20])
{
	/*
		??UI?]?w?ư?Event?? ?ǤJ?ƥ??}?C 

		?????ۦP??EVENT_ID ?i???ư?

	*/
	nuINT	nIdx=-1; 
	nuINT	nCheck_idx=0;

	while(nIdx++<n_Fm_Xml_total) 
	{
	  for(nCheck_idx=0;nCheck_idx<20;nCheck_idx++) 
	  {
		  if(nEventSet[nCheck_idx]==0) //?? Event SETs ?I??0 ?N???פ????? 
		  {
			  break;
		  }
	  }	  
	}
	return nuTRUE;
}


nuINT	CMapFileTMCDataP::TransCode(nuINT code)
{	
	//??�??ARRAY unicode ??BIG5
	//Trans_DATA *test=new Trans_DATA[2]({123,456},{156,456});
	//test[0]=(0x456,0x123);

	//nuINT *try1=new nuINT[0](10);

	nuINT idx=-1;
	nuINT nLimit=nSortLimt;
	nuINT	low = 0; 
	nuINT	upper = 0; 
	nuINT	lLocation = 0;//?????μƭ?
	nuCHAR	*pEdn = NULL;	  //?ഫ???ƥ?	

	low	  = 0;
	upper = nLimit-1;

	//?Y???諸???Ƥ??blow~upper?d?? ?N???Τ??? ?????^??-1
	if(code<data[low].nUniCode) 
	{
		return code;
	}

	if(code>data[upper].nUniCode)
	{
		return -1;
	}

	while(low <= upper) 
	{ 
		nuINT mid = (low+upper) / 2;
		lLocation = data[mid].nUniCode;

		if(lLocation < code) 
		{
			low = mid+1; 
		}
		else if(lLocation > code) 
		{	
			upper = mid - 1; 
		}
		else 
		{    
			return data[mid].nBig;
		}
	} 
	return -1; 
}
nuBOOL CMapFileTMCDataP::ReadVD()
{
	nuFILE * pFile = NULL;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuCHAR TmpWord = 0;
	nuCHAR TempStr[NURO_MAX_PATH*2] = {0};
	nuCHAR *pChr1 = NULL;
	nuCHAR *pChr2 = NULL;

	nuLONG lLen = 0;
	nuBOOL bStart = nuFALSE;

	if(!bVDInit)
	{
		return nuFALSE;
	}

	nVDCount	=	0;//?`?ƭ??O
	nuMemset(pVDData,NULL,sizeof(UNITVD2)*MAXVDNUM);

	nVDEventCount=0;
	nuMemset(pVDevent,NULL,sizeof(EVENTVD2)*MAXVDEVENTNUM);

	lLen=0;
	bStart=nuFALSE;
	nVD_Event_Count=0;
	nVD_Road_Count=0;

	nuWCHAR wChangWord[_TMC_UIEVENTNAME_LEN_*2]; //buffer


	//Get Data File Name


#ifdef WINCE
	nuTcscpy(tsFile,FILE_XMLW);
	//nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	//nuTcscat(tsFile, FILE_CSC_VD);
#else
	nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	nuTcscat(tsFile, FILE_CSC_VD);
#endif

		if(nVDCount>0)
		{
			delete[] m_VD_Data;
			m_VD_Data=NULL;
		}
		m_VD_Data=new TMC_XML_C_DATA[MAXVDNUM];
		if(m_VD_Data  == NULL)
		{
			return nuFALSE;
		}
		TMC_VD_EVENT nVD_Event[MAXVDNUM];
		nuMemset(nVD_Event,0,sizeof(nVD_Event));
		nuINT nEeventCount=0;


  TMC_XML_C_DATA XmlComplexTemp;

	//Open VD File
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		CleanVDData();
		return nuFALSE;
	}

	while(nuFread(&TmpWord, 1, 1, pFile))
	{
		switch(TmpWord)
		{
		case '<':
			bStart=nuTRUE;
			lLen=0;
			break;
		case '>':
			bStart=nuFALSE;
			break;
		default:
			if(bStart)
			{
				if(lLen<((NURO_MAX_PATH*2)-1))
				{
					TempStr[lLen]=TmpWord;
					lLen++;
				}
				else
				{
					bStart=nuFALSE;
					lLen=0;
				}
			}
			break;
		}
		if(nVD_Event_Count>=MAXVDNUM)
		{
            break;
		}

		if(bStart==nuFALSE && lLen!=0)
		{
			if(nuStrstr(TempStr,"node enName"))//????node?y?z?l?y
			{
				nVDCount++;
				if(nVDCount>MAXVDNUM)
				{
					nVDCount--;
					continue;
				}
				//???oID
				pChr1=nuStrstr(TempStr,"id=");
				pChr2=nuStrstr(pChr1+4," ");
				if(pChr1==NULL || pChr2==NULL || pChr2-1-(pChr1+4)>16)
				{
					nVDCount--;
				}
				nuMemcpy(pVDData[nVDCount-1].VDID,pChr1+4,pChr2-1-(pChr1+4));
				//???o?t??
				pChr1=nuStrstr(TempStr,"speed=");
				pChr2=nuStrstr(pChr1+7," ");
				if(pChr1==NULL || pChr2==NULL)
				{
					nVDCount--;
				}

				pVDData[nVDCount-1].VDSpeed=nuStrtol(pChr1+7,NULL,10);
				//???o?W??
				pChr1=nuStrstr(TempStr,"name=");
				pChr2=nuStrstr(pChr1+6," ");
				if(pChr1==NULL || pChr2==NULL  || pChr2-1-(pChr1+6)>32)
				{
					nVDCount--;
				}
				
		
				//nuMemcpy(XmlComplexTemp.wEvenName,wChangWord, nIndex); 
				
				nuMemcpy(pVDData[nVDCount-1].VDName,pChr1+6,pChr2-1-(pChr1+6));
				//nuLONG lLen = MultiByteToWideChar(CP_UTF8,0,pVDData[nVDCount-1].VDName,nuStrlen(pVDData[nVDCount-1].VDName),wChangWord,sizeof(wChangWord)); 
				nuLONG lLen = nuMbsToWcs(pVDData[nVDCount-1].VDName, wChangWord, sizeof(wChangWord));
				wChangWord[lLen]    =    '\0';  
				//nuMemcpy(m_VD_Data[nVDCount-1].wRoadName,wChangWord, sizeof(wChangWord)); 
			

			}


			if(nuStrstr(TempStr,"rEvent happenTime"))//????event?y?z?l?y
			{
				nVDEventCount++;
				if(nVDEventCount>MAXVDEVENTNUM)
				{
					nVDEventCount--;
					continue;
				}
				//???o?ƥ??y?z
				pChr1=nuStrstr(TempStr,"status=");
				pChr2=nuStrstr(pChr1+8,"id");
				if(pChr1==NULL || pChr2==NULL ) //PROSPER 2/22
				{
					nVDEventCount--;
				}
				//LONG EVENT HANDLE  (PROSPER 2/22)
				if(pChr2-2-(pChr1+8)>=256)
				{
					nuMemcpy(pVDevent[nVDEventCount-1].Event,pChr1+8,255);
					pVDevent[nVDEventCount-1].Event[255]=0;
				}
				else
				nuMemcpy(pVDevent[nVDEventCount-1].Event,pChr1+8,pChr2-2-(pChr1+8));
	
				//???o?ƥ?ID
				pChr1=nuStrstr(TempStr,"id=");
				pChr2=nuStrstr(pChr1+6,"/");
				if(pChr1==NULL || pChr2==NULL  || pChr2-1-(pChr1+4)>32)
				{
					nVDEventCount--;
				}
				nuMemcpy(pVDevent[nVDEventCount-1].EventID,pChr1+4,pChr2-1-(pChr1+4));
			}


			if(nVDCount<=MAXVDNUM)
			{
				//				if(nuStrstr(TempStr,"nEvent eventType=") && NULL==nuStrstr(TempStr,"PREWORK"))//?????ƥ??y?z?l?y ?ư??g?`?ʤu?@
				if(nuStrstr(TempStr,"nEvent eventType=") )//?????ƥ??y?z?l?y ?ư??g?`?ʤu?@
				{
					pVDData[nVDCount-1].EventCount++;
					
					if(nVD_Event_Count>MAXVDNUM)
					{
                         break;
					}
				    

					
					switch(pVDData[nVDCount-1].EventCount)
					{
					case 1:
						pChr1=nuStrstr(TempStr,"id=");
						pChr2=nuStrstr(pChr1+4,"/");
						nuMemcpy(pVDData[nVDCount-1].Event1,pChr1+4,pChr2-1-(pChr1+4));
						nuMemcpy(m_VD_Data[nVD_Event_Count].wRoadName,wChangWord, sizeof(wChangWord)); 
						nuMemcpy(nVD_Event[nVD_Event_Count++].nEventId,pChr1+4,pChr2-1-(pChr1+4));	
						break;
					case 2:
						pChr1=nuStrstr(TempStr,"id=");
						pChr2=nuStrstr(pChr1+4,"/");
						nuMemcpy(pVDData[nVDCount-1].Event2,pChr1+4,pChr2-1-(pChr1+4));
						nuMemcpy(m_VD_Data[nVD_Event_Count].wRoadName,wChangWord, sizeof(wChangWord)); 
						nuMemcpy(nVD_Event[nVD_Event_Count++].nEventId,pChr1+4,pChr2-1-(pChr1+4));	
						break;
					case 3:
						pChr1=nuStrstr(TempStr,"id=");
						pChr2=nuStrstr(pChr1+4,"/");
						nuMemcpy(pVDData[nVDCount-1].Event3,pChr1+4,pChr2-1-(pChr1+4));
						nuMemcpy(m_VD_Data[nVD_Event_Count].wRoadName,wChangWord, sizeof(wChangWord)); 
						nuMemcpy(nVD_Event[nVD_Event_Count++].nEventId,pChr1+4,pChr2-1-(pChr1+4));	
						break;
					case 4:
						pChr1=nuStrstr(TempStr,"id=");
						pChr2=nuStrstr(pChr1+4,"/");
						nuMemcpy(pVDData[nVDCount-1].Event4,pChr1+4,pChr2-1-(pChr1+4));
						nuMemcpy(m_VD_Data[nVD_Event_Count].wRoadName,wChangWord, sizeof(wChangWord)); 
						nuMemcpy(nVD_Event[nVD_Event_Count++].nEventId,pChr1+4,pChr2-1-(pChr1+4));	
						break;
						
					}
					if(pVDData[nVDCount-1].EventCount>4)
					{
						pVDData[nVDCount-1].EventCount=4;
					}
				}
			}
			lLen=0;
		}
	}

	
  nuINT nEvents=-1;
  nuINT nRoads=-1;
  nuINT nARRY=0;
  
  while(nRoads++<nVD_Event_Count)
  {

	  while(nEvents++<nVDEventCount)
		{
			
			if(nuStrcmp(nVD_Event[nRoads].nEventId,pVDevent[nEvents].EventID)==0)
			{
				char temp[128];
				 
		        nuMemcpy(temp,pVDevent[nEvents].Event, sizeof(temp)); 

				//nuLONG lLen = MultiByteToWideChar(CP_UTF8,0,temp,nuStrlen(temp),wChangWord,60);
				nuLONG lLen = nuMbsToWcs(temp, wChangWord, 60);
				wChangWord[lLen]    =    '\0';  
				if(lLen>=29)
				{
					wChangWord[29]    =    '\0';  
				}
				 nuMemset(m_VD_Data[nARRY].wEvenName,0, sizeof(m_VD_Data[nARRY].wEvenName)); 
				nuMemcpy(m_VD_Data[nARRY].wEvenName,wChangWord, sizeof(m_VD_Data[nARRY].wEvenName)); 
				nARRY++;
				nEvents=nVDEventCount;
			}
			
		}
		nEvents=-1;
  }

	nuFclose(pFile);
	pFile=NULL;

	UNITVD2 VD;
	EVENTVD2 VDevent;
	nuLONG k,l;
	for(k=0;k<nVDCount;k++)
	{
		GetVDUnit(k,VD);
		for(l=0;l<VD.EventCount;l++)
		{
			switch(l)
			{
			case 0:
				GetVDEVENT(VD.Event1,VDevent);
				break;
			case 1:
				GetVDEVENT(VD.Event2,VDevent);
				break;
			case 2:
				GetVDEVENT(VD.Event3,VDevent);
				break;
			case 3:
				GetVDEVENT(VD.Event4,VDevent);
				break;			
			}
		}
	}

	return nuTRUE;
}

nuINT	CMapFileTMCDataP::FindMapID(nuCHAR temp[10])
{	
	//??�??ARRAY ?s??TMC ??�?줧?e??????	
	//memset(tt,0,sizeof(STR2)*471);

	nuINT idx=-1;
	nuINT nLimit=_TMC_VD_MAPTABLE_COUNT;
	nuINT	low = 0; 
	nuINT	upper = 0; 
	nuINT	lLocation = 0;//?????μƭ?
	nuCHAR	*pEdn = NULL;	  //?ഫ???ƥ?	

	low	  = 0;
	upper = nLimit-1;

	//?Y???諸???Ƥ??blow~upper?d?? ?N???Τ??? ?????^??-1
	

	while(low <= upper) 
	{
		nuINT mid = (low+upper) / 2;
		nuINT l=nuStrncmp(temp,sMapID[mid].ID,7);
		if(l>0) 
		{
			low = mid+1; 
		}
		else if(l<0) 
		{	
			upper = mid - 1; 
		}
		else 
		{    
			return mid;
		}
	} 
	return -1; 
}


nuBOOL CMapFileTMCDataP::ReadVD2(nuCHAR *sTime)
{

	nuFILE * pFile = NULL,*pFile2 = NULL,*pFile3 = NULL;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTCHAR tsFileMapping[NURO_MAX_PATH] = {0};

	nuCHAR TmpWord = 0;
	nuCHAR TempStr[NURO_MAX_PATH*2] = {0};
	nuCHAR *pChr1 = NULL;
	nuCHAR *pChr2 = NULL;
	
	nuLONG lLen = 0;
	nuBOOL bStart = nuFALSE;

	if(!bVDInit)
	{
		return nuFALSE;
	}

	nVDCount	=	0;//?`?ƭ??O
	nuMemset(pVDData,NULL,sizeof(UNITVD2)*MAXVDNUM);

	lLen=0;
	bStart=nuFALSE;
	nVD_Event_Count=0;
	nVD_Road_Count=0;

	nuWCHAR wChangWord[_TMC_UIEVENTNAME_LEN_*2] = {0}; //buffer


	//Get Data File Name


#ifdef WINCE
	nuTcscpy(tsFile,FILE_XMLW);
	//nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	//nuTcscat(tsFile, FILE_CSC_VD);

	nuGetModulePath(NULL, tsFileMapping, NURO_MAX_PATH);
	nuTcscat(tsFileMapping, FILE_CSC_VD_ID);
	
	//nuTcscat(tsFile, FILE_CSC_VD2);
	
#else
	nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	nuGetModulePath(NULL, tsFileMapping, NURO_MAX_PATH);
	
	nuTcscat(tsFile, FILE_CSC_VD2);
	nuTcscat(tsFileMapping, FILE_CSC_VD_ID);
#endif

	if(nVDCount>0)
	{
		delete[] m_VD_Data;
		m_VD_Data=NULL;
	}
	/*m_VD_Data=new TMC_XML_C_DATA[MAXVDNUM];
	if(m_VD_Data  == NULL)
	{
		return nuFALSE;
	}*/
	
	pFile2 = nuTfopen(tsFileMapping, NURO_FS_RB);
	if( pFile2 == NULL )
	{
		CleanVDData();
		return nuFALSE;
	}

	if(nuFread(sMapID,sizeof(sMapID),1,pFile2)!=1)
	{
		CleanVDData();
		return nuFALSE;
	}
	
	TMC_XML_C_DATA XmlComplexTemp;

	//Open VD File
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		CleanVDData();
		return nuFALSE;
	}

	while(nuFread(&TmpWord, 1, 1, pFile))
	{
		switch(TmpWord)
		{
		case '<':
			bStart=nuTRUE;
			lLen=0;
			break;
		case '>':
			bStart=nuFALSE;
			break;
		default:
			if(bStart)
			{
				if(lLen<((NURO_MAX_PATH*2)-1))
				{
					TempStr[lLen]=TmpWord;
					lLen++;
				}
				else
				{
					bStart=nuFALSE;
					lLen=0;
				}
			}
			break;
		}

		if(bStart==nuFALSE && lLen!=0)
		{

			if(nuStrstr(TempStr,"XML_Head version"))
			{
				nuMemset(sTime,0,sizeof(sTime));
				pChr1=nuStrstr(TempStr,"updatetime=");
				pChr2=nuStrstr(pChr1+12,"i");
				nuMemcpy(sTime,pChr1+12,pChr2-2-(pChr1+12));
				

			}
			if(nuStrstr(TempStr,"Info routeid"))//????node?y?z?l?y
			{
				nVDCount++;
				if(nVDCount>MAXVDNUM)
				{
					nVDCount--;
					continue;
				}
				pChr1=nuStrstr(TempStr,"value=");
				pChr2=nuStrstr(pChr1+7," ");
				if(pChr1==NULL || pChr2==NULL)
				{
					nVDCount--;
				}

				pVDData[nVDCount-1].VDSpeed=nuStrtol(pChr1+7,NULL,10);
				//???o?W??
				pChr1=nuStrstr(TempStr,"Info routeid=");
				pChr2=nuStrstr(pChr1+14," ");
				if(pChr1==NULL || pChr2==NULL  || pChr2-1-(pChr1+14)>32)
				{
					nVDCount--;
				}
				nuMemcpy(pVDData[nVDCount-1].VDName,pChr1+14,pChr2-1-(pChr1+14));
				
				//?P?_?O?_????�??VDID
				nuINT nIdx=FindMapID(pVDData[nVDCount-1].VDName);
				if(nIdx>-1)
				{
					nuStrcpy(pVDData[nVDCount-1].VDID,sMapID[nIdx].VDID);				
				}
			}

		
			lLen=0;
		}
	}
	nuFclose(pFile);
	pFile=NULL;

	
	return nuTRUE;
}
nuBOOL CMapFileTMCDataP::ReadVD3(nuCHAR *sTime)
{

	nuFILE * pFile = NULL;
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};	

	nuCHAR TmpWord = 0;
	nuCHAR TempStr[NURO_MAX_PATH*2] = {0};
	nuCHAR *pChr1 = NULL;
	nuCHAR *pChr2 = NULL;
	
	nuLONG lLen;
	nuBOOL bStart;

	nuMemset(nSpeedCounts,0,sizeof(nSpeedCounts));
	nuMemset(nStartIdx,0,sizeof(nStartIdx));
	
	if(!bVDInit)
	{
		return nuFALSE;
	}

	nVDCount	=	0;//?`?ƭ??O
	nuMemset(pVDData,NULL,sizeof(UNITVD2)*MAXVDNUM);

	lLen=0;
	bStart=nuFALSE;
	nVD_Event_Count=0;
	nVD_Road_Count=0;

	nuWCHAR wChangWord[_TMC_UIEVENTNAME_LEN_*2] = {0}; //buffer


	//Get Data File Name


#ifdef WINCE
	nuTcscpy(tsFile,FILE_XMLW);
	//nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	//nuTcscat(tsFile, FILE_CSC_VD);

	//nuGetModulePath(NULL, tsFileMapping, NURO_MAX_PATH);
	//nuTcscat(tsFileMapping, FILE_CSC_VD_ID);
	
	//nuTcscat(tsFile, FILE_CSC_VD2);
	
#else
	nuGetModulePath(NULL, tsFile, NURO_MAX_PATH);
	nuTcscat(tsFile, FILE_CSC_VD2);
	
#endif

	nuMemset(nSpeedCounts,0,sizeof(nSpeedCounts));
	nuMemset(nStartIdx,0,sizeof(nStartIdx));
	
	nuINT nIDIdx=0;
	nuCHAR sIDLimit[][8]={"nfb2012","nfb2032","nfb2062","nfb2076","nfb2100","nfb2124",
						  "nfb2140","nfb2156","nfb2176","nfb2200","nfb2218","nfb2234"};
	nuBOOL bAdd=nuFALSE;
	
	if(nVDCount>0)
	{
		delete[] m_VD_Data;
		m_VD_Data=NULL;
	}
	/*m_VD_Data=new TMC_XML_C_DATA[MAXVDNUM];
	if(m_VD_Data  == NULL)
	{
		return nuFALSE;
	}*/

	TMC_XML_C_DATA XmlComplexTemp;

	//Open VD File
	pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		CleanVDData();
		return nuFALSE;
	}

	while(nuFread(&TmpWord, 1, 1, pFile))
	{
		switch(TmpWord)
		{
		case '<':
			bStart=nuTRUE;
			lLen=0;
			break;
		case '>':
			bStart=nuFALSE;
			break;
		default:
			if(bStart)
			{
				if(lLen<((NURO_MAX_PATH*2)-1))
				{
					TempStr[lLen]=TmpWord;
					lLen++;
				}
				else
				{
					bStart=nuFALSE;
					lLen=0;
				}
			}
			break;
		}

		if(bStart==nuFALSE && lLen!=0)
		{

			if(nuStrstr(TempStr,"XML_Head version"))
			{
				nuMemset(sTime,0,sizeof(sTime));
				pChr1=nuStrstr(TempStr,"updatetime=");
				pChr2=nuStrstr(pChr1+12,"i");
				nuMemcpy(sTime,pChr1+12,pChr2-2-(pChr1+12));
				

			}
			if(nuStrstr(TempStr,"Info routeid"))//????node?y?z?l?y
			{
				nVDCount++;
				if(nVDCount>MAXVDNUM)
				{
					nVDCount--;
					continue;
				}
				pChr1=nuStrstr(TempStr,"value=");
				pChr2=nuStrstr(pChr1+7," ");
				if(pChr1==NULL || pChr2==NULL)
				{
					nVDCount--;
				}
				char speed[32];
				nuMemset(speed,0,sizeof(speed));
				nuMemcpy(speed,pChr1+7,pChr2-1-(pChr1+7));
				nuStrcat(speed,"km/h");
				nuMemcpy(pVDData[nVDCount-1].Event1,speed,sizeof(speed));
				
				pVDData[nVDCount-1].VDSpeed=nuStrtol(pChr1+7,NULL,10);
				//???o?W??
				pChr1=nuStrstr(TempStr,"Info routeid=");
				pChr2=nuStrstr(pChr1+14," ");
				if(pChr1==NULL || pChr2==NULL  || pChr2-1-(pChr1+14)>32)
				{
					nVDCount--;
				}
				
				char temp[32]={0};
				char *c;
				nuMemcpy(temp,pChr1+14,pChr2-1-(pChr1+14));
				
				if((nuStrcmp(temp,"nfb2000")<0)||nuStrcmp(temp,"nfb2234")>0||nuStrcmp(temp,"nfb2188")==0||nuStrcmp(temp,"nfb2187")==0)
				{
					nVDCount--;
					
				}	
				else
				{
					char sNum;
					sNum=temp[6];

					if(nuStrcmp(temp,sIDLimit[nIDIdx/2])<=0)
					{
						if(sNum%2==1)
						{
							if(!bAdd)
							{	
								bAdd=nuTRUE;
							}
							nSpeedCounts[nIDIdx]++;
							
						}
						else
						{
							if(bAdd)
							{	
								nIDIdx++;
								bAdd=nuFALSE;
							}
							nSpeedCounts[nIDIdx]++;
						}
					}
					else
					{

						nIDIdx++;
						nSpeedCounts[nIDIdx]++;
					}

				nuMemcpy(pVDData[nVDCount-1].VDName,pChr1+14,pChr2-1-(pChr1+14));
				
				}
			}

		
			lLen=0;
		}
	}
	for(nuINT i=0;i<24;i++)
	{
		nStartIdx[i]=nVD_Event_Count;
		nVD_Event_Count+=nSpeedCounts[i];
	}
	nuFclose(pFile);
	pFile=NULL;

	
	return nuTRUE;
}
nuBOOL CMapFileTMCDataP::CleanVDData()
{
	if(!bVDInit)
	{
		return nuFALSE;
	}
	nVDCount=0;
	if(pVDData)
	{
		delete [] pVDData;
		pVDData=NULL;
	}
	nVDEventCount=0;
	if(pVDevent)
	{
		delete [] pVDevent;
		pVDevent=NULL;
	}
	return nuTRUE;
}

nuLONG CMapFileTMCDataP::GetVDCount(nuCHAR *sTime)
{
		if(pVDData && pVDevent)
	{
		bVDInit=nuTRUE;
		nuMemset(pVDData,NULL,sizeof(UNITVD2)*MAXVDNUM);
		nuMemset(pVDevent,NULL,sizeof(EVENTVD2)*MAXVDEVENTNUM);
		ReadVD2(sTime);
	}
	return nVDCount;
}

nuBOOL CMapFileTMCDataP::GetVDUnit(nuLONG UnitIndex,UNITVD2 &VD)
{
	if(!bVDInit)
	{
		return nuFALSE;
	}
	if(0<=UnitIndex && UnitIndex<nVDCount)
	{
		nuMemcpy(&VD,&(pVDData[UnitIndex]),sizeof(UNITVD2));
		return nuTRUE;
	}
	return nuFALSE;
}

nuBOOL CMapFileTMCDataP::GetVDEVENT(nuCHAR *pEvent,EVENTVD2 &VDevent)
{
	nuLONG i;
	if(!bVDInit)
	{
		return nuFALSE;
	}
	if(pEvent==NULL)
	{
		return nuFALSE;
	}

	for(i=0;i<nVDEventCount;i++)
	{
		if(0==nuMemcmp(pVDevent[i].EventID,pEvent,nuStrlen(pVDevent[i].EventID)))
		{
			nuMemcpy(&VDevent,&(pVDevent[i]),sizeof(UNITVD2));
			return nuTRUE;
		}
	}
	return nuFALSE;
}

nuBOOL CMapFileTMCDataP::GetVD(nuVOID*pTmcBuf,const nuUINT size)
{
	nuMemcpy(pTmcBuf,pVDData,size);
	return nuTRUE;
}
