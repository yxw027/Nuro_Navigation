// FileRn.h: interface for the CFileRn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILERNI_H__21249658_FDD5_4CC6_B2B4_4788F6F79340__INCLUDED_)
#define AFX_FILERNI_H__21249658_FDD5_4CC6_B2B4_4788F6F79340__INCLUDED_

#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "NuroEngineStructs.h"

#define _BGL_50K_RNI_    nuTEXT("Map\\Major\\China50k.RNI")
#define _BGL_5K_RNI_     nuTEXT("Map\\Major\\China5k.RNI")

typedef struct Tag_StrRniHeader
{
	nuWORD    RniCount;
	nuBYTE    RniLen;
	nuBYTE    Reserve;
}StrRniHeader;

class CFileRni  
{
public:
	CFileRni();
	virtual ~CFileRni();

	nuBOOL  Initialize();
	nuVOID  Free();
	nuBOOL  GetRoadIconInfo(nuWORD nDwIdx, nuLONG nScaleValue, nuLONG nBGLStartScale, PRNEXTEND pRnEx, nuWORD *RoadIconInfo);

protected:
	nuLONG    m_nScaleValue;
};

#endif // !defined(AFX_FILERN_H__21249658_FDD5_4CC6_B2B4_4788F6F79340__INCLUDED_)
