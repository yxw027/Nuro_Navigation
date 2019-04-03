#ifndef Def_RoutingDataCtrl
	#define Def_RoutingDataCtrl
	//這是個管理類 其目的是用來進行規劃結果的管理
	//目前的規劃是 規劃資料從原本的class轉為struct
	//但是原本類所支援的建構,解構功能 則轉由本類來管理
	//藉以將 資料主體 以及 資料管理 這兩個部分分離
	#include "NuroEngineStructs.h"
	class CRoutingDataCtrl
	{
		public:
			CRoutingDataCtrl();
			virtual ~CRoutingDataCtrl();

			nuVOID Release_RoutingData(PROUTINGDATA &NaviData);


			nuBOOL Create_NaviFromToClass(PNAVIFROMTOCLASS &NaviData);
			nuVOID Release_NaviFromToClass(PNAVIFROMTOCLASS &NaviData);

			nuBOOL Create_NaviRtBlockClass(PNAVIRTBLOCKCLASS &NaviData);
			nuVOID Release_NaviRtBlockClass(PNAVIRTBLOCKCLASS &NaviData);


			nuBOOL Create_NaviSubClass(PNAVIRTBLOCKCLASS &NRBC,nuULONG Count);
			nuVOID Release_NaviSubClass(PNAVIRTBLOCKCLASS &NRBC);
			nuBOOL new_NaviSubClass(PNAVISUBCLASS &NaviData,nuLONG Count);
			nuVOID delete_NaviSubClass(PNAVISUBCLASS &NaviData);
			nuBOOL Create_NodeIDList(PNAVIRTBLOCKCLASS &NRBC,nuULONG Count);
			nuVOID Release_NodeIDList(PNAVIRTBLOCKCLASS &NRBC);

			nuBOOL Create_CrossData(NAVISUBCLASS &NSS,nuLONG Count);
			nuVOID Release_CrossData(NAVISUBCLASS &NSS);
			nuBOOL Create_ArcPt(NAVISUBCLASS &NSS,nuLONG Count);
			nuVOID Release_ArcPt(NAVISUBCLASS &NSS);
	};

#endif