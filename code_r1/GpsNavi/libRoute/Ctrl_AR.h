
#include "NuroDefine.h"
//#include "Ctrl_UONE_RtB.h"
//extern UONERtBCtrl UoneRTB;

typedef struct Tag_structAR_Nearby
{
	nuULONG FMapID;
	nuULONG FRTBID;
	nuULONG TMapID;
	nuULONG TRTBID;
	nuULONG Count;
}structAR_Nearby;

typedef struct Tag_structARtrans_C
{
	nuULONG NodeID:18;
	nuULONG RTBID:6;
}structARtrans_C;

typedef struct Tag_structAR_C
{
	nuULONG FRTBID:12;
	nuULONG FNodeID:20;
	nuULONG TRTBID:12;
	nuULONG TNodeID:20;
}structAR_C;

typedef struct Tag_structAR_B
{
	nuULONG TMapID:20;
	nuULONG TAreaID:12;
	nuULONG NodeCount:16;
	nuULONG Reserve:16;
	nuULONG C_address:32;
}structAR_B;

typedef struct Tag_structAR_A
{
	nuULONG FMapID:8;
	nuULONG FAreaID:12;
	nuULONG NearbyCount:12;	
}structAR_A;

typedef struct Tag_structAR
{
	nuUSHORT RTIDCount;
	structAR_A **A_ARdata;
	structAR_B **B_ARdata;
	structAR_C **C_ARdata;
}structAR;

class CCtrl_AR : public structAR//, UONERtBCtrl
{
		public:
			CCtrl_AR();
			virtual ~CCtrl_AR();

			nuBOOL InitClass();
			nuVOID CloseClass();
			
			nuBOOL ReadAR_Head();
			nuBOOL ReadAR_Node(nuULONG C_add, nuULONG B_add);
			nuBOOL AR_NearbyCity_Compare( nuULONG FMap, nuULONG TMap, nuULONG FRtb, nuULONG TRtb, 
				                          nuULONG *C_add, nuULONG *B_add );

			nuBOOL AR_Node_Compare(nuULONG FNodeID, nuULONG FRTBID);
			//nuBOOL AR_Enter_RT(UONERtBCtrl *g_pRtB_F);
			nuVOID ReleaseStruct();
			nuVOID InitARStruct();
			
		public:
			structAR_Nearby  AR_Nearby;
			structARtrans_C  ARtrans_C;
			nuLONG addr;
			nuFILE *ARFile;
			nuTCHAR FName[512];
	#ifdef _DEBUG
		int  ARAlocCount;
		int  ARFreeCount;
	#endif
};