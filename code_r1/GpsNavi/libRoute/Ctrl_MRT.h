	#ifndef Def_MRTCtrl
		#define Def_MRTCtrl

		#include	"Stru_StateStru.h"
		#include	"ConstSet.h"
		#include	"NuroDefine.h"

		typedef struct Tag_MRTData_Header_Stru
		{
			nuULONG	TotalNodeCount;//總Node數
			nuULONG	TotalRoadCount;//總Road數
			nuULONG	RtConnectStruCount;//總連結數
			nuLONG			RtIndexStruStartAddr;//所以資料起始位置
			nuLONG			RtConnectStruStartAddr;//連結結構起始位置
		}MRTData_Header_Stru,*pMRTData_Header_Stru;
 
		typedef struct Tag_MRTData_MainData_Stru
		{
			nuULONG	ConnectStruIndex : 24;//連結資料索引值
			nuULONG	ConnectCount : 6;//連結總數
			nuULONG	NoTurnItem : 2;//轉彎標示
			NUROPOINT		NodeCoor;
		}MRTData_MainData_Stru,*pMRTData_MainData_Stru;

		typedef struct Tag_MRTData_Connect_Stru
		{
			nuULONG	TNodeID;//起始NodeID
			nuULONG	RoadLength:24;//道路長度
			nuULONG	RoadClass:8;//道路等級
		}MRTData_Connect_Stru,*pMRTData_Connect_Stru;

#ifdef KD
		typedef struct Tag_MRTData_Index_Stru
		{
			nuULONG		BlockIdx;//道路代碼
			nuULONG		RoadIdx;//道路代碼
			nuULONG 	FNodeID;//起始NodeID
			nuULONG 	TNodeID;//起始NodeID
		}MRTData_Index_Stru,*pMRTData_Index_Stru;
#else
		typedef struct Tag_MRTData_Index_Stru
		{
			nuUSHORT	BlockIdx;//道路代碼
			nuUSHORT	RoadIdx;//道路代碼
			nuULONG 	FNodeID;//起始NodeID
			nuULONG 	TNodeID;//起始NodeID
		}MRTData_Index_Stru,*pMRTData_Index_Stru;
#endif

		typedef struct Tag_MRTData
		{
			MRTData_Header_Stru		RtBHeader;//Rt的表頭
			pMRTData_MainData_Stru	*RtBMain;//Rt中的主資料
			pMRTData_Connect_Stru	*RtBConnect;//Rt中的連結資料
			pMRTData_Index_Stru		*RtBIdx;//Rt中RoadID與NodeID的對應資料
		}MRTData,*pMRTData;


		class CMRTCtrl : public MRTData
		{
			public:
				CMRTCtrl();
				virtual ~CMRTCtrl();
				nuBOOL	InitClass();
				nuVOID	CloseClass();
				nuBOOL	Read_Header(nuLONG StartAddr);//,FILE *RtFile);//讀取表頭資料
				nuBOOL	Read_MainData();//產生主資料儲存陣列
				nuBOOL	Read_Connect();//產生聯通資料儲存陣列
				nuBOOL	Read_Index();//讀取地標點資料
				nuBOOL	ReSet();//移除Rt檔

				nuVOID	Remove_Index();//讀取背景線資料


				nuVOID	Remove_Header();//移除表頭資料
				nuVOID	Remove_MainData();//移除主資料儲存陣列
				nuVOID	Remove_Connect();//移除聯通資料儲存陣列
			private:
				nuVOID	IniConst();

			public:
				nuFILE	*RtBFile;
				nuTCHAR	FName[NURO_MAX_PATH];
			#ifdef _DEBUG
				int  MRTAlocCount;
				int  MRTFreeCount;
			#endif
			private:
				nuBOOL	ReadHeaderOK;//判斷檔頭讀取與否 只有在已讀取檔頭的情況下 才可以操控資料
				nuLONG	m_StartAddr;
		};
	#endif
