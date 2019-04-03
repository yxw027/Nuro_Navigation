	#ifndef DEF_MDWCTRL
		#define DEF_MDWCTRL

		#include "ConstSet.h"
		#include "NuroDefine.h"

		typedef struct Tag_MDWHeader//固定的表頭
		{
			nuULONG	m_TotalBlockCount;
			nuLONG**			p_GBlockID;//道路起始位置串
			nuLONG**			p_BlockRoadDataAddr;//道路起始位置串
			nuLONG**			p_BlockRoadAddCount;//道路起始位置串
			nuLONG**			p_BlockDataSize;//道路起始位置串
			nuULONG	m_ClassCnt;
			nuLONG**			p_BlockClassStartAddr;//道路起始位置串
		}MDWHeader,*pMDWHeader;

		typedef struct TagMDWRoadData
		{		
			nuULONG	VertexCount:16;//節點總數
			nuULONG	StartVertexIndex_high:16;//起始節點索引值
			nuULONG	StartVertexIndex_Low:8;//起始節點索引值
			nuULONG	RoadClass:3;//道路共同資料索引值
			nuULONG	MapID:9;
			nuULONG	RTBID:12;
		}MDWRoadData;
		typedef struct Tag_MDWBlockRoadData
		{
			nuULONG	m_RoadCount;//道路總數
			nuULONG	m_VertexCount;//節點總數
			MDWRoadData		**p_RdData;//道路資料
			NUROPOINT		**p_VertexCoor;//節點座標串
		}MDWBlockRoadData,*pMDWBlockRoadData;


		class CMDWCtrl
		{
			public:
				CMDWCtrl();
				virtual ~CMDWCtrl();

				nuBOOL InitClass();
				nuVOID CloseClass();

				nuBOOL	Reset();
				nuBOOL	Read_Header();//讀取表頭資料
				nuVOID	Remove_Header();//移除表頭資料
				nuBOOL	Read_BlockData(nuULONG Index);
				nuVOID	Remove_BlockData(nuULONG Index);			
			private:
				nuVOID	InitConst();
				nuBOOL	CreateMainStru(nuULONG Count);
				nuVOID	ReleaseMainStru();
				nuBOOL	Read_RoadData(nuULONG Index);//讀取道路資料
				nuVOID	Remove_RoadData(nuULONG Index);//移除道路資料
			public:
				nuFILE				*DWFile;
				MDWHeader			Header;
				MDWBlockRoadData	**Rd;
				nuTCHAR				FName[NURO_MAX_PATH];
			#ifdef _DEBUG
				int  MDWAlocCount;
				int  MDWFreeCount;
			#endif
			private:
				nuBOOL	ReadHeaderOK;//判斷檔頭讀取與否 只有在已讀取檔頭的情況下 才可以操控資料
				nuBOOL	**ReadBlockOK;//判斷是否有讀取Block資料
		};
	#endif
