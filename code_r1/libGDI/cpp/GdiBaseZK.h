// GdiBaseZK.h: interface for the CGdiBaseZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIBASEZK_H__7DD3684C_B212_4E1B_9BD0_5878BB496589__INCLUDED_)
#define AFX_GDIBASEZK_H__7DD3684C_B212_4E1B_9BD0_5878BB496589__INCLUDED_

#include "NuroDefine.h"
#include "NuroCommonApi.h"

#define _USE_CANVAS_SINGLE//控制是否用重叠窗口

class CGdiBaseZK  
{
public:
	CGdiBaseZK();
	virtual ~CGdiBaseZK();
public:
    static NURO_API_GDI     s_apiGdi;
};

#endif // !defined(AFX_GDIBASEZK_H__7DD3684C_B212_4E1B_9BD0_5878BB496589__INCLUDED_)
