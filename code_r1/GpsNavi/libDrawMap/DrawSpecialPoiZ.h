// DrawSpecialPoiZ.h: interface for the CDrawSpecialPoiZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWSPECIALPOIZ_H__5960CD28_4580_486B_9FF7_BFF42876562E__INCLUDED_)
#define AFX_DRAWSPECIALPOIZ_H__5960CD28_4580_486B_9FF7_BFF42876562E__INCLUDED_

#include "NuroDefine.h"

#define NURO_SPECIAL_POI_BMP			nuTEXT("Media\\3DPOI.bmp")

class CDrawSpecialPoiZ  
{
public:
	CDrawSpecialPoiZ();
	virtual ~CDrawSpecialPoiZ();

	nuBOOL	OpenBmpFile(const nuTCHAR *pTsPath);
	nuVOID	CloseBmpFile();

	nuINT	DrawSpPOI(nuINT x, nuINT y, nuLONG nIconNum, nuINT nIconAlignType = 0);

protected:
	NURO_BITMAPFILEHEADER m_bmpFileHead;
	NURO_BITMAPINFOHEADER m_bmpInfoHead;
	nuFILE*		m_pFile;
	nuLONG		m_nNowIconType;
	nuBYTE*		m_pNowIconBuff;
	nuWORD		m_nBuffLen;
};

#endif // !defined(AFX_DRAWSPECIALPOIZ_H__5960CD28_4580_486B_9FF7_BFF42876562E__INCLUDED_)
