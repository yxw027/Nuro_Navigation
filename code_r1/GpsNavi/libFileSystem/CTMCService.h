#include "FileMapBaseZ.h"
#include "CTMCCollation.h"
#ifndef DEFINE_CTMCSERVICE
	#define DEFINE_CTMCSERVICE

extern PFILESYSAPI	g_pTMCFSApi;

	class CTMCService : public CFileMapBaseZ
	{
	public:
		CTMCService();
		virtual ~CTMCService();
		nuBOOL Copy_TMC_Event_Data(nuBYTE nDWIdx);
		nuBOOL Initial_TMC_Event_Data(nuBYTE nDWIdx);//掛載TMCMapping和TMCServer資料
		nuVOID Release_TMC_Event_Data(nuBYTE nDWIdx);//釋放TMCMapping和TMCServer資料
		nuBOOL Initial_TMC_Data(nuINT iCount);
		nuVOID Release_TMC_Data();
		nuUINT GetTMCTrafficInfo(nuBYTE nDWIdx, nuUINT BlockID,nuUINT RoadID);           //取得道路交通狀況
		nuUINT GetTMCTrafficInfoByNodeID(nuBYTE nDWIdx, nuUINT NodeID1,nuUINT NodeID2);  //取得道路交通狀況
		nuUINT GetTMCDirInfo(nuBYTE nDWIdx, nuUINT BlockID,nuUINT RoadID);				 //取得道路方向性
		nuUINT GetTMCDirInfoByNodeID(nuBYTE nDWIdx, nuUINT NodeID1,nuUINT NodeID2);		 //取得道路方向性
		TMCRoadInfoStru* GetTMCDataInfo(nuBYTE nDWIdx, nuUINT BlockID,nuUINT RoadID);
		CTMCCollation *tmpTMCCollation;
		nuUINT *p_index;
		nuINT nCount;
	};
#endif
