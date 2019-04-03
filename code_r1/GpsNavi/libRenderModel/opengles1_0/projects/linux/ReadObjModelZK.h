// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: ReadObjModelZK.h,v 1.3 2009/09/18 02:07:30 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/09/18 02:07:30 $
// $Locker:  $
// $Revision: 1.3 $
// $Source: /home/nuCode/libRenderModel/ReadObjModelZK.h,v $
///////////////////////////////////////////////////////////////////////////
#if !defined(AFX_READOBJMODELZK_H__69B2F618_1E0F_41AA_B73B_F45F8F9D63A4__INCLUDED_)
#define AFX_READOBJMODELZK_H__69B2F618_1E0F_41AA_B73B_F45F8F9D63A4__INCLUDED_

#include "nuDefine.h"
#ifdef NURO_OS_LINUX

#else 
#include <windows.h>
#include <tchar.h>
#endif
#include "ReadBaseZK.h"

#define _ZK_MAX_TRANGEL_COUNT					(256*1024)

#define _ZK_OBJ_MAX_COUNT						5
#define	_ZK_OBJ_FILE_NAME_NUM					80



typedef struct tagOBJ_3D_MODEL
{
//	TCHAR	tzObjName[_ZK_OBJ_FILE_NAME_NUM];
	long	nX;
	long	nY;
	long	nTrangelAddr;
	long	nTrangelCount;
	UINT	nTexture;//only one or more than one?
	BYTE	bUsefull;
	BYTE	bHasNormal;
}OBJ_3D_MODEL, *POBJ_3D_MODEL;

class CReadObjModelZK : public CReadBaseZK
{
public:
	CReadObjModelZK();
	virtual ~CReadObjModelZK();

	bool Initialize(class COpenGLProcessZK*	pOpenGL);
	void Free();

	UINT	AddModel(long x, long y, char *pszObjName);

protected:
	bool ReadObjFile(const TCHAR* tzFieName);

public:
	BYTE			m_nNowObjCount;
	OBJ_3D_MODEL	m_stuObj3DList[_ZK_OBJ_MAX_COUNT];
	long			m_nNowTrangelCount;
	float*			m_pfTrangel;
protected:
	class COpenGLProcessZK*		m_pOpenGL;

};

#endif // !defined(AFX_READOBJMODELZK_H__69B2F618_1E0F_41AA_B73B_F45F8F9D63A4__INCLUDED_)
