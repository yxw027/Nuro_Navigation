#include "FileMapBaseZ.h"
#ifndef DEFINE_KDTMCCTRL
	#define DEFINE_KDTMCCTRL

	typedef struct tagKDTMCFileHeaderStruct
	{
		nuUINT DataCnt;
		nuUINT NodeDataAddr;
		nuBYTE Reserve[92];
	}KDTMCFileHeaderStruct,*pKDTMCFileHeaderStruct;

	typedef struct tagKDTMCFileMainDataStruct
	{
		nuUINT MeshID:24;
		nuUINT RoadClass:8;
		nuUINT LinkID:16;
		nuUINT LocationCodeNum:16;
		nuUINT LocationVertexAddr;  //讀取後要設法改裝成起始Index 不然不易以資料庫的方式操作
	}KDTMCFileMainDataStruct,*pKDTMCFileMainDataStruct;

	typedef struct tagKDTMCFileNodeIDDataStruct
	{
		nuUINT NameAddr;
		nuUINT StartNode;
		nuUINT EndNode;
		nuUINT BlockID;
		nuUINT RoadID;
		nuUINT Dir:8;
		nuUINT TrafficEvent:24;  //Reserve 把這一碼拿來裝填道路交通路況
	}KDTMCFileNodeIDDataStruct,*pKDTMCFileNodeIDDataStruct;

	typedef struct tagKDTMCFileStruct
	{
		KDTMCFileHeaderStruct		KDTMCHeader;
		KDTMCFileMainDataStruct		*pKDTMCMain;
		KDTMCFileNodeIDDataStruct	*pKDTMCNode;
	}KDTMCFileStruct,*pKDTMCFileStruct;

	class CKDTMCFileCtrl : public CFileMapBaseZ 
	{
		public:
			CKDTMCFileCtrl();
			virtual ~CKDTMCFileCtrl();
			nuBOOL ReadTMCFile(nuTCHAR *tsFilePath);
			nuVOID CopyTMCStru();
			KDTMCFileStruct	TMCStru;
			KDTMCFileStruct	TMCStru2;
			nuUINT TotalNodeDataCount;
			nuUINT TotalNodeDataCount2;
			nuBOOL ReleaseTMCStru();
			nuVOID ReleaseTMCStru2();
			nuBOOL m_bUseTMC;
		private:
			nuBOOL bReleaseTMCFileOK;
			
	};

#endif
