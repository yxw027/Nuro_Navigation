
#include "DrawPoi.h"
#include "GdiMethodZK.h"
#include "NuroClasses.h"
#include "NuroModuleApiStruct.h"

CDrawPoi::CDrawPoi()
{
    CBmpMethodsZK::BmpInit(&m_PoiBMP);
}
CDrawPoi::~CDrawPoi()
{
    UnloadPoiInfo();
}

nuBOOL CDrawPoi::LoadPoiInfo(nuTCHAR* filename)
{
    CBmpLoader loader(&m_PoiBMP);
    if (!loader.Load(filename))
    {
        UnloadPoiInfo();
        return nuFALSE;
    }
    return nuTRUE;
}

nuVOID CDrawPoi::UnloadPoiInfo()
{
    CBmpMethodsZK::BmpDestroy(&m_PoiBMP);
}

nuUINT CDrawPoi::DrawPoi(NURO_BMP* pDstBmp, nuINT x, nuINT y, nuLONG Poi, nuUINT nRTPOSStyle)
{
    nuINT nPoiSizeH = m_PoiBMP.bmpHeight;
    nuINT nPoiSizeW = m_PoiBMP.bmpHeight;
    if(Poi<0 || Poi*nPoiSizeW >= m_PoiBMP.bmpWidth)
    {
        return 0;
    }
    switch(nRTPOSStyle)
    {
    case NURO_RTPOS_LT: break;
    case NURO_RTPOS_LC: y -= (nPoiSizeH>>1); break;
    case NURO_RTPOS_LB: y -= nPoiSizeH; break;        
    case NURO_RTPOS_CT: x -= (nPoiSizeW>>1); break;
    case NURO_RTPOS_CC: x -= (nPoiSizeW>>1); y -= (nPoiSizeH>>1); break;
    case NURO_RTPOS_CB: x -= (nPoiSizeW>>1); y -= nPoiSizeH; break;        
    case NURO_RTPOS_RT: x -= nPoiSizeW; break;
    case NURO_RTPOS_RC: x -= nPoiSizeW; y -= (nPoiSizeH>>1); break;
    case NURO_RTPOS_RB: x -= nPoiSizeW; y -= nPoiSizeH; break;
    default: break;
    }    
    nuBOOL bRes = CGdiMethodZK::nuGMCopyBmp(pDstBmp, x, y, nPoiSizeW, nPoiSizeH, &m_PoiBMP, Poi*nPoiSizeW, 0, NURO_BMP_TYPE_MIXX);
    if(bRes)
    {
        nPoiSizeW = nPoiSizeW<<16;
        return nPoiSizeH + nPoiSizeW;
    }
    else
    {
        return 0;
    }
}


