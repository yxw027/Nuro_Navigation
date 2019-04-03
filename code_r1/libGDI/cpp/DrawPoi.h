
#ifndef _DRAWPOI_H_INCLUDE_FDAKFJANN234NMNB6B_898FA904
#define _DRAWPOI_H_INCLUDE_FDAKFJANN234NMNB6B_898FA904

#include "NuroDefine.h"

class CDrawPoi
{
public:
    CDrawPoi();
    ~CDrawPoi();

    nuBOOL LoadPoiInfo(nuTCHAR* filename);
    nuVOID UnloadPoiInfo();

    nuUINT DrawPoi(NURO_BMP* pDstBmp, nuINT x, nuINT y, nuLONG Poi, nuUINT nRTPOSStyle);

private:
    NURO_BMP m_PoiBMP;	
};

#endif

