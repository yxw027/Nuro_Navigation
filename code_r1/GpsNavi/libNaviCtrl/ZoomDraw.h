// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: ZoomDraw.h,v 1.3 2010/09/03 07:01:46 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2010/09/03 07:01:46 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libNaviCtrl/ZoomDraw.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZOOMDRAW_H__5CB5E3A6_3DD9_42F4_83E5_3BD898E4AAC2__INCLUDED_)
#define AFX_ZOOMDRAW_H__5CB5E3A6_3DD9_42F4_83E5_3BD898E4AAC2__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"

//--- For nZoomInOrOut ----------------------------//
#define ZOOM_NO									0x00
#define ZOOM_IN									0x01
#define ZOOM_OUT								0x02
class CZoomDraw  
{
public:
	CZoomDraw();
	virtual ~CZoomDraw();

	nuBOOL Initialize(class CNaviThread* pNaviThd);
	nuVOID Free();

	nuBOOL ZoomProcess(PACTIONSTATE pActionState);
	nuBOOL ZoomProcessEx(PACTIONSTATE pActionState, nuDOUBLE fZoomDis);

public:
	nuLONG	m_nOldScaleValue;
	nuLONG	m_nNewScaleValue;
	nuBYTE	m_nZoomType;
	nuLONG	m_lZoomScale;

protected:
	class CNaviThread*	m_pNaviThd;
};

#endif // !defined(AFX_ZOOMDRAW_H__5CB5E3A6_3DD9_42F4_83E5_3BD898E4AAC2__INCLUDED_)
