// DrawCarXQ.h: interface for the CDrawCarXQ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWCARXQ_H__CD2B5D3B_C48F_4AFC_A07A_2B19021B3825__INCLUDED_)
#define AFX_DRAWCARXQ_H__CD2B5D3B_C48F_4AFC_A07A_2B19021B3825__INCLUDED_

#include "NuroDefine.h"

#define _CAR_BMP_FILE_PATH_      nuTEXT("MEDIA\\PIC\\NAVIKING\\")
#define _CAR_B_BMP_FILE_PATH_    nuTEXT("MEDIA\\PIC\\BNAVIKING\\")
#define _CAR_L_BMP_FILE_PATH_    nuTEXT("MEDIA\\PIC\\LNAVIKING\\")
class CDrawCarXQ  
{
public:
	CDrawCarXQ();
	virtual ~CDrawCarXQ();

	nuBOOL    Initialize();
	nuVOID    Free();
	//
	nuUINT  DrawCar(nuLONG nCarInScreenX, nuLONG nCarInScreenY, nuBYTE byIconType); 

private:
	nuUINT  LoadCarIcon(nuBYTE byIconType);
	nuUINT  RotationBmp(nuBYTE *pBuff, nuWORD nWidth, nuWORD nHeight, nuINT nAngle);

protected:
	NURO_BMP    m_bmpCar;
	nuUINT      m_OldCarBmpState;
	nuBOOL      m_3DMode;
	nuBYTE		m_byIconType;
	nuWORD      m_nOldRotAngle;
	nuSHORT     m_nOldCarAngle;
};

#endif // !defined(AFX_DRAWCARXQ_H__CD2B5D3B_C48F_4AFC_A07A_2B19021B3825__INCLUDED_)
