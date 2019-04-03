// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/12/08 04:14:01 $
// $Revision: 1.4 $
// MyMathTool.h: interface for the CMyMathTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYMATHTOOL_H__5D8B0ADB_E5C1_4C3E_8E20_D7A78E3E3867__INCLUDED_)
#define AFX_MYMATHTOOL_H__5D8B0ADB_E5C1_4C3E_8E20_D7A78E3E3867__INCLUDED_

#include "NuroDefine.h"

class CMyMathTool  
{
	public:
		CMyMathTool();
		virtual ~CMyMathTool();
		nuVOID  Normalize(NURORECT &pRect);
		nuBOOL  PointInRect(NUROPOINT pt, NURORECT &rect);
		nuVOID  FixinLine(NUROPOINT NowPt, NUROPOINT LinePt1, NUROPOINT LinePt2, NUROPOINT &FixedPt, nuLONG &Dis);
};
extern CMyMathTool		**g_MyMathTool;

#endif // !defined(AFX_MYMATHTOOL_H__5D8B0ADB_E5C1_4C3E_8E20_D7A78E3E3867__INCLUDED_)
