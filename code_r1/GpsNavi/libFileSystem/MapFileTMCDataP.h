
#include "FileMapBaseZ.h"

#define  _TMC_UIROADNAME_LEN_  30
#define  _TMC_UIEVENTNAME_LEN_ 30
#define  _TMC_UIEVENTNAMECHAR_LEN_ 60
#define  _TMC_DATA_LIMIT_	    400
#define  _TMC_VD_MAPTABLE_COUNT 489
#define  _TMC_VD_MAPTABLE_COUNT2 176

#define  FILE_CSC_VD			nuTEXT("UIdata\\國道VD.dat")
#define  FILE_CSC_VD2			nuTEXT("UIdata\\VD.txt")
#define  FILE_CSC_VD_ID			nuTEXT("UIdata\\VDID_MAPPING.txt")
#define  FILE_CSC_VD_NAME		nuTEXT("UIdata\\VDID_MAPPING2.txt")
 
#define  FILE_CSC_XML			nuTEXT("UIdata\\traffic_tmc.txt")
#define  FILE_CSC_XMLR			nuTEXT("UIdata\\traffic_city.txt")
#define  FILE_NAME_SORT			nuTEXT("UIdata\\Sort.bin")
#define  FILE_NAME_TABLE			nuTEXT("UIdata\\tw.ln")


typedef struct tag_VDMapID
{
  nuCHAR VDID[15];
  nuCHAR ID[10];
} VDMapID;
typedef struct tag_VDROADNAME
{
  nuCHAR VDID[15];
  nuCHAR ID[20];
} VDROADNAME;
typedef struct tag_XML_RDST_TMCINFO // XML/FM temp
{
	nuCHAR cLocation[10];
	nuCHAR cEvent[10]; 
	
}XML_RDST_TMCINFO;

typedef struct tag_TMC_INFO_LIST	//UI所需的名稱資訊
{
	nuWORD  wRoadName[_TMC_UIROADNAME_LEN_];
	nuINT	nEventID; 
	nuWORD	wEvenName[_TMC_UIEVENTNAME_LEN_];
	
}TMC_INFO_LIST, *PTMC_INFO_LIST;

typedef struct tag_TMC_INFO_UI
{
	nuINT   nCount;
	PTMC_INFO_LIST pPTMC_INFO_LIST;
	
} TMC_INFO_UI,*pTMC_INFO_UI;

typedef struct tag_TMC_EVENT_DATA	  //.TMC 內部 EVENT參數 
{
	nuWORD   wTraffic_Num;
	nuWORD   wTraffic_NameLen;
	nuSHORT  sTraffic_EventWeighting;
	nuSHORT  sTraffic_EventTime;
	nuWORD	 wEventName[15];

}TMC_EVENT_DATA;

typedef struct tag_TMC_Header		  //.TMC Header_DATA
{

	nuDWORD DataCnt;
	nuDWORD VertexDataAddr;
	nuDWORD TrafficDataAddr;
	nuWORD  TrafficCnt;
	nuWORD  Reserve;

} TMC_Header;

typedef struct tag_TMC_DATA			   //.TMC DATA (nodes資訊)
{
	
	nuWORD  wLocationCode;
	nuWORD  wLocationCodeNum;
	nuDWORD dLocationVertexAddr;
	nuDWORD dLocationUpAddr;
	nuDWORD dLocationDownAddr;
	
}TMC_DATA;

typedef struct tag_LOCATION_DATA		//對應Location_table 的名稱資訊
{
	
	nuINT  cLocID;
    nuWORD cLocName[50];//固定大小        
	
}TMC_LOCATION;

typedef struct tag_TMC_Byte_DATA	//FM 資訊由 BYTE形式傳遞 固為此命名
{
	nuINT	nRoad;
	nuINT	nEvent;
	nuDWORD nTime;//固定大小        

}TMC_Byte_DATA;

typedef struct tag_Code				//Unicode 轉 Big5 所需結構 
{

	nuINT	nBig;
	nuINT	nUniCode;    

}Trans_DATA;

typedef struct tag_TMC_XML_Complex_DATA				//XML_ROAD 資訊 
{													//@@@@@@@@@ 能須想辦法轉換 讓ARM版可以呈現 @@@@@@@@@@@@@@	 			

	nuWORD wRoadName[_TMC_UIROADNAME_LEN_];
	nuWORD wEvenName[_TMC_UIEVENTNAME_LEN_];
	nuDWORD nTime;        

}TMC_XML_C_DATA;


typedef struct Tag_UNITVD2
{
	nuCHAR	VDID[16];
	nuCHAR	VDName[32];
	nuLONG	VDSpeed;
	nuLONG	EventCount;
	nuCHAR	Event1[32];
	nuCHAR	Event2[32];
	nuCHAR	Event3[32];
	nuCHAR	Event4[32];	
}UNITVD2,*PUNITVD2;


typedef struct Tag_EVENTVD2
{
	nuCHAR	EventID[32];
	nuCHAR	Event[256];
}EVENTVD2,*PEVENTVD2;

typedef struct tag_TMC_VD_EVENT
{
	nuCHAR   nEventId[30];
	
} TMC_VD_EVENT,*pTMC_VD_EVENT;


class CMapFileTMCDataP : public CFileMapBaseZ  
{
public:
	CMapFileTMCDataP();
	virtual ~CMapFileTMCDataP();

	nuBOOL	Get_TMC_FM_XML_SIZE(nuUINT& nselet,nuUINT* nFm_Count,nuUINT& nXML_Count,nuUINT* nXML_Complex_Count);//解析DATA 統計COUNT ， nselet:檔案參數
	
	//UI    取回道路/事件名稱資訊 (BUFFER/頁數/排序模式)
	nuBOOL	Get_TMC_XML_L(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode);			
	nuBOOL	Get_TMC_XML(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode);		
	nuBOOL	Get_TMC_FM(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode);
	nuBOOL	Get_TMC_XML_FM(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode);
	nuBOOL	Get_TMC_VD(nuVOID* pTmcBuf, const nuUINT TmcDataLen,nuUINT nmode);

	//Router取回繞路所需資訊(BUFFER,保留參數)
	nuBOOL	Get_TMC_Router(nuVOID* pTmcBuf, const nuUINT TmcDataLen);		

	nuINT	TransCode(nuINT code); //排序用  unicode 轉 big5

	nuBOOL	OpenXML(nuLONG nMapID);
   	nuBOOL	XML_Parser();					//XML解析資料
	nuBOOL	XML_Update();					//更新 @@
	nuBOOL	XMLFree();

	nuBOOL	OpenRoadXML(nuLONG nMapID);
	nuBOOL	RoadXML_Parser();				//XML_L解析資料
	nuBOOL	RoadXML_Update();				//更新 @@
	nuBOOL	RoadXMLFree();					//@@待補充
	
	nuBOOL	FMFree();
	nuBOOL	Set_TMC_Byte(nuVOID*pTmcBuf);	//FM解析資料
	nuBOOL	Byte_Update();					//更新 待完成
	nuBOOL	ByteFree();						//@@待補充
    
	nuBOOL	ALLFree();
	nuBOOL	TMCInfoFree();

	

	//Set_TMC_Byte () 相關變數 
	nuINT	m_nWords;		//第N個字  (26字為一組)
	nuINT	m_nLeftWords;	//剩餘字數
	nuCHAR  m_sLeftWord[3]; //剩餘字元

	nuBOOL	ShiftWords(nuVOID* pTmcBuf,nuINT* nNums,nuINT);	//比對字元SHIFT	
	nuCHAR	m_sCheckStr[26];							//暫存		DATA
	nuCHAR	m_sTmc_information[13][2];					//轉換		TMC解析的二維陣列
	nuINT	m_nTmc_information2[13][2];					//轉換		ANSI用陣列 
	nuBOOL	m_bCheckHeader;								//判斷		是否符合DATA_HEAD	
	nuBOOL	m_bLeftWord;								//			是否有剩餘的sTmpWord[]沒有 放入....
	nuBOOL	m_bCorrect;									//			是否為正確的DATA(尾端為5A5A)
	nuCHAR	m_sCheckWord[4];							//判斷開頭  進行SHIFT動作(將剩下的字串進行移動並比較)
	nuINT	m_nLeft;									//剩下的未處理字元數
    nuBOOL	m_bLeft;

	//紀錄TMC EVENT_ID(NAME)/LOCATION_ID(NAME)/TIME
	TMC_Byte_DATA	m_nByte[_TMC_DATA_LIMIT_];
	TMC_Byte_DATA	*m_XML_Data;
	TMC_XML_C_DATA	*m_XML_Complex_Data;
	TMC_XML_C_DATA	*m_VD_Data;
	//Trans_DATA		*data;

	//時間間距- Start / End
	nuDWORD	m_dStartTime;
	nuDWORD	m_dDistTime;	
	nuDWORD	m_dnowTime;

	//FM/XML/FM+XML/XML_R 數量
	nuINT	m_nByte_Record;
	nuINT	m_nXml_Record;
	nuINT   m_nXml_fm_Record;
	nuINT	m_nXml_Complex_Record;

	
	//CHECK - TIME 保留有效時間內的資訊 @@
	nuBOOL	CheckDataTime();
	nuBOOL	CheckDataTimeXML();
	nuBOOL	CheckDataTimeComplexXML();
	
	//比對重複資訊 以及更新時間資訊
	nuBOOL	CheckData(TMC_Byte_DATA temp);
	nuBOOL	CheckDataX(TMC_Byte_DATA temp);
	nuBOOL	CheckDataXMLComplex(TMC_XML_C_DATA temp);

	//排序  0:NULL 1:ROAD 2:Event 
	nuBOOL  FM_Order(nuINT);
	nuBOOL  XML_Order(nuINT);			
	nuBOOL	XML_COMPLEX_Order(nuINT n);
	nuBOOL	XML_FM_Order(nuINT n);
	nuBOOL	VD_Order(nuINT n);
	
	//FILE  基本函式
	nuBOOL	IsOpen();
	nuVOID	Close();
	nuUINT	GetLength();
	nuBOOL	ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len);

	//EVENT/LOACTION SORT
	nuVOID	ShellSort();							//改良的Bubble
	nuINT	BinarySearch_TMC_DATA(nuINT find);		//二分排序-Location_code(.TMC)
	nuINT	BinarySearch_TMC_LocName(nuINT find);	//二分排序-Location_Id(Location_table)
	nuBOOL	ShowEvent(nuINT);						//轉存事件資訊(FM+XML)
	nuBOOL	ShowLocation(nuINT);					//轉存路名資訊(FM+XML)

	TMC_EVENT_DATA		*EventComparData;			//TMC內部的EVENT資料(供比對用)
	TMC_DATA			*TMC_dataA;					//TMC內部的DATA_A資料 利用loc_code 進行排序 以利之後的比對
	TMC_LOCATION		*LocData;					//TMC Table名稱資料   利用loc_id   進行排序 以利之後的比對

	nuBOOL	SetEvent(nuINT nEventSet[20]);			//UI設定事件 改變bTMCPass 提供Router使用 (***要改成動態的大小***)
	nuINT	n_Fm_Xml_total;							//FM+XML total records ()

	nuINT   nTmcLocCount;		//TMC的資料(DataA)總數
	nuINT	TMC_Event_Count;	//計算事件名稱筆數
	nuINT	nLocSize;			//location_code 筆數
	nuINT	nLocTableSize;		//location_id   筆數
	
	nuBOOL	bLocationFrist;		//判斷是否進行TMC DATA_A的資料排序
	nuBOOL	bEventFrist;		//判斷是否將 EventData 載入


	//UI&Router 使用TMC內部資料參數 
	TMC_Info_Router		m_TMC_Info_Router;  //Router 所需的DATA
	TMC_INFO_UI			tmc_ui;				//FM&RDS_XML 所取得到資訊(路名,Event_ID,事件名稱)
	nuINT				m_nArry_idx;		//名稱陣列(tmc_ui)的index

	nuBOOL ReadVD();
	nuINT  FindMapID(char *temp);
	nuBOOL ReadVD2(nuCHAR *sTime);
	nuBOOL ReadVD3(nuCHAR *sTime);

	nuINT CityID;
	nuINT nCityCounts[22];
	nuINT nSpeedCounts[24];
	nuINT nStartIdx[24];
	
	nuLONG GetVDCount(nuCHAR *sTime);
	nuBOOL GetVDUnit(nuLONG UnitIndex,UNITVD2 &VD);
	nuBOOL CleanVDData();
	nuBOOL GetVD(nuVOID*pTmcBuf,const nuUINT size);

	nuLONG nVDCount;
	UNITVD2 *pVDData;//裝填VD資料的結構指標

	nuBOOL GetVDEVENT(nuCHAR *pEvent,EVENTVD2 &VDevent);

	nuLONG nVDEventCount;
	EVENTVD2 *pVDevent;
	nuBOOL bVDInit;
	nuINT nVD_Event_Count;
	nuINT nVD_Road_Count;
	Trans_DATA *data;
	nuINT nSortLimt;
	VDMapID sMapID[_TMC_VD_MAPTABLE_COUNT];
	VDROADNAME sMapID2[_TMC_VD_MAPTABLE_COUNT2];
	nuBOOL bNoLoad;
	nuINT nTempRoadIdx;
	nuINT nTempHighIdx;
	
private:
	nuCFile file;			
	nuCFile LocNamefile;
	nuWORD	totalcount;
	nuFILE  *pFile;
};
