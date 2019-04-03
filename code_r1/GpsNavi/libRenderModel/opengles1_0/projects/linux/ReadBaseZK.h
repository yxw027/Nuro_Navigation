// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: ReadBaseZK.h,v 1.2 2009/09/18 02:07:30 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/09/18 02:07:30 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libRenderModel/ReadBaseZK.h,v $
/////////////////////////////////////////////////////////////////////////

#if !defined(AFX_READBASEZK_H__3B916571_88A8_47BB_85F3_9BB054F55B87__INCLUDED_)
#define AFX_READBASEZK_H__3B916571_88A8_47BB_85F3_9BB054F55B87__INCLUDED_

#include "nuDefine.h"

#define _ZK_3D_MODEL_FOLDER_PATH_NM				80

class CReadBaseZK  
{
public:
	CReadBaseZK();
	virtual ~CReadBaseZK();

	static void InitStatic();

public:
	static TCHAR	s_tzPath[_ZK_3D_MODEL_FOLDER_PATH_NM];

};

#endif // !defined(AFX_READBASEZK_H__3B916571_88A8_47BB_85F3_9BB054F55B87__INCLUDED_)
