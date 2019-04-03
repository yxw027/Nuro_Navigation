// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:30:56 $
// $Revision: 1.14 $
//Library
#include "NuroDefine.h"
#ifdef _DEBUG
#include <stdio.h>
#endif
#ifndef DEF_CtrlAVLTree
	#define DEF_CtrlAVLTree


	#define MaxAVLCount			150000
	#define MaxAVLWeightCount	10000
	#define	MAXSELFL			4096

	#define	MAXMappingTreeCount	7

	#define MappingCheckByte_s 4
	#define MappingCheckByte_l 8

	// 2^27=128000km?o???O?i?H¶?????y???@?ӶZ??
	//?ثe?????O?????Ҷq ?ĥ?2^23=8000km ?]???ȯ??ŦX?????j???ݨD
	//???L???a ?i???N?ݭn???O?????e???~??
	//?]��?j?????ëh?Ԩ쫢???ت????D?A?????N???ӰO?????ҦҶq??2^23?令2^24?~??
	typedef struct TagAStarData//?򥻨??q?D??????(Node1->Node2->Node3)
	{
		nuULONG TMapID		: 8;
		nuULONG TRtBID		: 6;
		nuULONG TNodeID		: 18;//?n?��ժ?NodeID(To)
		nuULONG NMapID		: 8;
		nuULONG NRtBID		: 6;
		nuULONG NNodeID		: 18;//?{?b??NodeID?Ҧb??(Now)
		nuULONG FMapID		: 8;
		nuULONG FRtBID		: 6;
		nuULONG FNodeID		: 18;//?O?q?????Ӫ?(From)
		nuULONG FFMapID		: 8;
		nuULONG FFRtBID		: 6;
		nuULONG FFNodeID		: 18;//?O?q?????Ӫ?(From)
		nuULONG RDis			: 24;//?L?h???L???[?v?Z??(Run Dis)
		nuULONG Class			: 4;
		nuULONG FNoTurnFlag	: 1;//???s?Х?
		nuULONG NNoTurnFlag	: 1;//???s?Х?
		nuULONG TNoTurnFlag	: 1;//???s?Х?
		nuULONG WalkingFlag	: 1;//???s?Х?
		nuULONG NowDis		: 24;//?{?b????(Now Dis)
		nuULONG FMapLayer		: 3;
		nuULONG NMapLayer		: 3;
		nuULONG TMapLayer		: 3;
		nuULONG FFMapLayer	: 3;
		nuULONG FTNoUseFlag   : 1;
	}AStarData,*LpAStarData;

	typedef struct TagAVLTree
	{
		nuULONG	p_High;
		nuULONG	p_Self;
		nuULONG	p_Low;
		AStarData		Data;
	} AVLTree, *LpAVLTree;

	/*typedef struct TagAVLTree
	{
		nuULONG	p_High:20;
		nuULONG	p_SelfL:12;
		nuULONG	p_SelfH:12;
		nuULONG	p_Low:20;
		AStarData		Data;
	} AVLTree, *LpAVLTree;*/

	typedef struct tagWEIGHTINGSTRU
	{
		nuULONG TargetID;//?ثe?w?[?J?????Ƽ?
		nuULONG TotalDis;//???{?b???[?v
		nuULONG RealDis;//???{?b?????ڦ樫?Z??
	} WEIGHTINGSTRU, *PWEIGHTINGSTRU, *LPWEIGHTINGSTRU;
	class CtrlAVLTree
	{
		public:
			CtrlAVLTree();
			virtual ~CtrlAVLTree();
			//?W?[?@???s??????
			nuBOOL Insert(AStarData m_AStarData, nuLONG STE_ID, WEIGHTINGSTRU NewWeighting);//?^?ǭȪ??ܷs?W?P?_
			//open to close???B?J
			nuBOOL GetBestOpenToClose(LpAVLTree &m_SAVLTree, WEIGHTINGSTRU &BestChoose);
			//?i?Ω?close???d??
			nuBOOL CheckMapping(AStarData m_AStarData, LpAVLTree &m_ReTurnNode, nuLONG STE_ID);
			nuBOOL CheckMappingNew(AStarData m_AStarData, LpAVLTree &m_ReTurnNode, nuLONG STE_ID);
			//????base?禡
			nuBOOL CreateBaseAVLTreeArray();
			nuVOID RemoveBaseAVLTreeArray();
			nuVOID CleanAVLTreeArray();

		private:
			nuBOOL AddNewBase(AStarData &m_AStarData,LpAVLTree &m_SAVLTree);
			//Mapping Tree ?????禡
			nuVOID AddToMapping(LpAVLTree &m_NewAddData, nuLONG STE_ID);

		public:
			LpAVLTree p_MappingTree[MAXMappingTreeCount];

			nuVOID MoveWeightingArray(nuLONG N_Idx);

		private:
			AVLTree **p_BaseData;
			nuLONG BaseCount;

			WEIGHTINGSTRU **WeightingArray;
			nuLONG WeightingCount;
		
//			nuLONG LEVEL_Max;
//			nuLONG LEVEL_Count;
		public:
			nuINT nucmp(const nuVOID *pDes, const nuVOID *pSrc, nuSIZE nLen);
			nuBOOL m_bSaveSlowClass;

	#ifdef _DEBUG
			nuINT AVLAlocCount;
			nuINT AVLFreeCount;
	#endif
	};

#endif
