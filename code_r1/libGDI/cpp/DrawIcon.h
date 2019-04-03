
#ifndef _DRAWICON_H_INCLUDE_FAKJF2NMFANDF089_FAKJFK
#define _DRAWICON_H_INCLUDE_FAKJF2NMFANDF089_FAKJFK

#include "NuroDefine.h"

class CDrawIcon
{
public:
    CDrawIcon();
    ~CDrawIcon();
    nuBOOL  LoadIconInfo(nuTCHAR* filename);
    nuVOID  UnloadIconInfo();
//    bool DrawIcon(NURO_BMP* pDstBmp, int x, int y, long Icon);
private:
    nuFILE*     m_fpIcon;
    nuBYTE*     m_pIconInfo;
    nuBYTE      iconBuff[32*1024];
    NURO_BMP    nuIconBMP;
};

#endif
