// Ctrl_Thc.h: interface for the CCtrl_Thc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTRL_THC_H__EBA90EB5_99C8_4997_AAB1_F8A7F48F1972__INCLUDED_)
#define AFX_CTRL_THC_H__EBA90EB5_99C8_4997_AAB1_F8A7F48F1972__INCLUDED_

	#include "ConstSet.h"
	#include "NuroDefine.h"

	#ifdef	DEFNEWTHCFORMAT
#ifdef KD
		typedef struct TAGVIACITYNODEDATA
		{
			nuULONG	MAPID:7;
			nuULONG	RTBID:5;
			nuULONG	MRTNODEID:20;
		}VIACITYNODEDATA,*pVIACITYNODEDATA;
		typedef struct Tag_StruConnect
		{
			nuULONG  TNodeID:20;
			nuULONG  TMapLayer:2;
			nuULONG  TGMapID1:10;
			nuULONG  TGMapID2:10;
			nuULONG  Reserve:13;
			nuULONG  bKmFlag:2;
			nuULONG  lViaCityCount:7;
			nuUSHORT Length;
			nuUSHORT WeightingLength;
			nuULONG lViaCityIndex;
		}StruConnect,*pStruConnect;
#else
		typedef struct TAGVIACITYNODEDATA
		{
			nuULONG	MAPID:8;
			nuULONG	RTBID:6;
			nuULONG	MRTNODEID:18;
		}VIACITYNODEDATA,*pVIACITYNODEDATA;

		typedef struct Tag_StruConnect
		{
			nuULONG  TNodeID:20;
			nuULONG  TMapLayer:2;
			nuULONG  TGMapID1:10;
			nuULONG  TGMapID2:10;
			nuULONG  Reserve:15;
			nuULONG  lViaCityCount:7;
			nuUSHORT Length;
			nuUSHORT WeightingLength;
			nuULONG lViaCityIndex;
		}StruConnect,*pStruConnect;

#endif
		typedef struct Tag_StruFNode
		{
			nuULONG	ConnectStruIndex;
			nuULONG	MRTID;
		}StruFNode,*pStruFNode;

		typedef struct Tag_StruThc
		{
			nuULONG	TotalFNodeCount;
			nuULONG	TotalConnectCount;
			nuULONG	ConnectStruStartAddr;
			nuULONG	ViaCityAddrStartAddr;
			StruFNode		**p_StruFNode;
			StruConnect		**p_StruConnect;
			VIACITYNODEDATA **ppstuViaNodeData;
		}StruThc,*pStruThc;
	#else
		typedef struct Tag_StruConnect
		{
			nuULONG TNodeID:20;
			nuULONG TMapLayer:2;
			nuULONG TGMapID:10;
			nuUSHORT WeightingLength;
			nuUSHORT Length;
		}StruConnect,*pStruConnect;

		typedef struct Tag_StruFNode
		{
			nuULONG	ConnectStruIndex;
			nuULONG	ConnectCount;
			nuULONG	MRTID;
		}StruFNode,*pStruFNode;

		typedef struct Tag_StruThc
		{
			nuULONG	TotalFNodeCount;
			nuULONG	TotalConnectCount;
			nuULONG	ConnectStruStartAddr;
			StruFNode		**p_StruFNode;
			StruConnect		**p_StruConnect;
		}StruThc,*pStruThc;
	#endif

	class CCtrl_Thc  : public StruThc
	{
		public:
			CCtrl_Thc();
			virtual ~CCtrl_Thc();

			nuBOOL	InitClass(nuLONG rtrule);
			nuVOID	CloseClass();

			nuBOOL	Read();//讀取表頭資料
			nuBOOL	Remove();//移除表頭資料
			nuBOOL	ReadConnect(nuLONG StartIndex,nuULONG Count);//讀取表頭資料
			#ifdef	DEFNEWTHCFORMAT
				nuBOOL	ReadViaNodeData(nuLONG StartIndex,nuULONG Count);//讀取ViaNodeData
			#endif
			nuFILE	*ThcFile;
			nuTCHAR	FName[NURO_MAX_PATH];
			nuBOOL	b_ThcReady;
		#ifdef _DEBUG
			int  ThcAlocCount;
			int  ThcFreeCount;
		#endif
		private:
			nuVOID	IniConst();
			nuLONG	l_RoutingRule;
	};


#endif // !defined(AFX_CTRL_THC_H__EBA90EB5_99C8_4997_AAB1_F8A7F48F1972__INCLUDED_)
