// Draw3DModelZK.h: interface for the CDraw3DModelZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAW3DMODELZK_H__B8CC1F5E_1F1B_456C_A278_08B0916F05D4__INCLUDED_)
#define AFX_DRAW3DMODELZK_H__B8CC1F5E_1F1B_456C_A278_08B0916F05D4__INCLUDED_

#include "NuroDefine.h"
#include "LoadRenderModelZK.h"
#include "File3dmZK.h"
#include "NuroModuleApiStruct.h"
#include "Nuro3DModelStruct.h"
#include "GApiOpenResource.h"

#define _3D_MODEL_NAME_MAX_NUM					50		

class CDraw3DModelZK  
{
public:
	typedef struct tagMODEL_3D
	{
		nuLONG		nID;//index + 1
		nuLONG		x;//map coordinate
		nuLONG		y;//map coordinate
		nuroRECT	rtBoundary;//map coordinate
		nuCHAR		szName[51];
	}MODEL_3D, *PMODEL_3D;

public:
	CDraw3DModelZK();
	virtual ~CDraw3DModelZK();
	nuBOOL Inialize(const nuTCHAR* pTzPath, nuPVOID  pRsApi, PMATHTOOLAPI pLibMT = NULL);
	nuVOID Free();

	nuUINT StartOpenGL(SYSDIFF_SCREEN sysDiff, const nuroRECT*	pRtScreen);
	nuUINT Check3DModel(PNURORECT pRtMap);
	nuUINT SetViewOption(nuSHORT nMapAngle, nuFLOAT fScale, nuPVOID lpApiMT, nuLONG w = 0, nuLONG h = 0);
	nuUINT Draw3DLayer(nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, const PNURO_BMP pBitmap);

protected:
	inline nuBOOL AdjustPosition(nuINT& nDesX,
							   	 nuINT& nDesY,
								 nuINT& nWidth,
								 nuINT& nHeight,
								 nuINT& nSrcX,
								 nuINT& nSrcY,
								 nuINT nDesW,
								 nuINT nDesH,
								 nuINT nSrcW,
								 nuINT nSrcH);
	inline nuVOID RemoveModelOut(PNURORECT pRtMap);
	inline nuBOOL SeekForModel(PNURORECT pRtMap);
	inline nuBOOL AddModelToList(const nuLONG& nID, 
								 const nuLONG& x,
								 const nuLONG& y,
								 nuroRECT* prtBoundary,
								 const nuCHAR* pszName);
	inline nuVOID AddModelToOpenGL(PARAMETER_VIEW_OPTION* op);
	nuUINT	StartOpenGLes();

public:
	class CLoadRenderModelZK	m_loadRM;
	API_RENDERMODEL				m_apiRM;
	//
protected:
	MODEL_3D		m_3dModelList[_3D_MODEL_SHOW_MAX];
	nuINT			m_nNowCount;
	CFile3dmZK		m_file3dm;

private:
	nuBOOL			m_bOpenGLStart;
	nuroRECT		m_rtScreen;
	//nuHWND			m_hWnd;
	//nuHDC			m_hDC;
	SYSDIFF_SCREEN  m_sysDiff;
	const nuTCHAR*	m_pTzPath;
	PMATHTOOLAPI    m_pLibMT;
	CTRL_3D_PARAM   m_3d_param;

	POPENRSAPI  m_pRsApi;

#ifdef USING_FILE_SETTING_3D_PARAM_VIEW
	nuFLOAT m_fLookAtX;
	nuFLOAT m_fLookAtY;
	nuFLOAT m_fLookAtZ;
#endif
};

#endif // !defined(AFX_DRAW3DMODELZK_H__B8CC1F5E_1F1B_456C_A278_08B0916F05D4__INCLUDED_)
