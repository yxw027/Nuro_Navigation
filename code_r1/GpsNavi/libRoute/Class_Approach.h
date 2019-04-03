// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:47:23 $
// $Revision: 1.2 $
#ifndef Def_Class_Approach
	#define Def_Class_Approach

	typedef struct Tag_ApproachStru
	{
		nuULONG FNodeID : 20; //要嘗試的NodeID(To)
		nuULONG FRtBID : 12;

		nuULONG FMapID : 16;
		nuULONG TMapID : 16;

		nuULONG TNodeID : 20; //現在的NodeID所在的(Now)
		nuULONG TRtBID : 12;

		nuULONG Reserval : 21;
		nuULONG FMapLayer : 5; //From to Now
		nuULONG TMapLayer : 6; //Now to To

	}ApproachStru,*pApproachStru;

	class Class_Approach
	{
		public:
			Class_Approach();
			virtual ~Class_Approach();
			nuBOOL CreateApproach(nuLONG Count);
			nuBOOL ReleaseApproach();
			nuLONG ApproachCount;
			ApproachStru **p_Approach;
#ifdef _DEBUG
		nuINT ApproachFreeCount;		
		nuINT ApproachAlocCount;
#endif
		private:
	};
#endif