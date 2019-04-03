#ifndef Def_DontToHere
	#define Def_DontToHere
	//這是個管理類 其目的是用來進行規劃結果的管理
	//目前的規劃是 規劃資料從原本的class轉為struct
	//但是原本類所支援的建構,解構功能 則轉由本類來管理
	//藉以將 資料主體 以及 資料管理 這兩個部分分離

	#include "NuroDefine.h"
	#define	MaxDontToHereCount		20
	#define	EachTimeJoinCount		10
	#define DontToWorkingCount		1000
	#define MaxCongestionIDCount	128

	typedef struct Tag_DontToNodeID
	{
		nuULONG NodeID : 20;
		nuULONG RtBID : 12;
		nuULONG MapID : 16;
		nuULONG UseCount : 16;
		nuLONG  Index;
	}DontToNodeID,*LpDontToNodeID;

	class CDontToHere
	{
		public:
			CDontToHere();
			virtual ~CDontToHere();
			nuVOID InitConst();
			nuLONG CheckDontTo(nuULONG MapID, nuULONG RtBID, nuULONG NodeID);
			nuVOID JoinNewDontTo(nuULONG MapID, nuULONG RtBID, nuULONG NodeID, nuLONG Order);
			nuVOID SystemReRouting();

			//塞車規劃機制
			nuBOOL CheckCongestionID(nuDWORD NodeID);
			nuVOID JoinNewCongestionID(nuDWORD NodeID);
			nuVOID CleanCongestionID();

			nuBOOL b_CongestionRoute;

		private:
			nuLONG OrderFixWeighting(nuLONG Order);
			nuLONG OrderFixCount(nuLONG Order);
			DontToNodeID DontToPt[MaxDontToHereCount];
			nuLONG CheckCount;
			nuLONG Indexvalue;

			//塞車規劃機制
			nuLONG CongestionIDCount;
			nuDWORD CongestionID[MaxCongestionIDCount];
	};

#endif