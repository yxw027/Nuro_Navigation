// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:52:00 $
// $Revision: 1.2 $
	#ifndef Def_RtCtrl
		#define Def_RtCtrl

		#include "Ctrl_UONE_RtB.h"
		#include "NuroDefine.h"

		typedef struct Tag_ONECoorStru
		{
				NUROPOINT ORG_Coor;
				nuUSHORT DIF_X;
				nuUSHORT DIF_Y;
		}ONECoorStru,*pUONECoorStru;

		typedef struct Tag_UONERtStru
		{
				nuUSHORT MapID;
				nuUSHORT Rt_Block_Count;
				nuULONG **Rt_Block_ID_List;
				nuLONG **Rt_Block_Addr_List;
				ONECoorStru **RT_Block_Coor;
				UONERtBData **TmpRtB;
		}UONERtStru,*pUONERtStru;

		class UONERtCtrl : public UONERtStru
		{
			public:
				UONERtCtrl();
				virtual ~UONERtCtrl();

				nuBOOL InitClass();
				nuVOID CloseClass();

				nuBOOL	Read_Header();//讀取表頭資料
				nuVOID	Remove_Header();//移除表頭資料
				nuBOOL	ReadRTB(nuLONG RtBIdx,nuBOOL b_MainConnect,nuBOOL b_Idx);//移除Rt檔
				nuVOID	ReSetRTB(nuLONG RtBIdx);//移除Rt檔
				nuVOID	ReSet();
			private:
				nuBOOL	Read_RTB_Main(nuLONG RtBIdx);
				nuVOID	Release_RTB_Main(nuLONG RtBIdx);
				nuBOOL	Read_RTB_Connect(nuLONG RtBIdx);
				nuVOID	Release_RTB_Connect(nuLONG RtBIdx);
				nuBOOL	Read_RTB_Index(nuLONG RtBIdx);
				nuVOID	Release_RTB_Index(nuLONG RtBIdx);
				nuVOID	InitConst();
			public:
				nuFILE	*RtFile;
				//nuTCHAR	**FName;
				nuTCHAR	FName[NURO_MAX_PATH];
			#ifdef _DEBUG
				int  UoneRTAlocCount;
				int  UoneRTFreeCount;
			#endif
			private:
				nuBOOL	ReadHeaderOK;//判斷檔頭讀取與否 只有在已讀取檔頭的情況下 才可以操控資料
		};
	#endif
