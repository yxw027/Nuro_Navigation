
#include "DrawIcon.h"
#include "GdiMethodZK.h"

CDrawIcon::CDrawIcon()
{
    m_fpIcon = NULL;
    m_pIconInfo = NULL;
}
CDrawIcon::~CDrawIcon()
{
    UnloadIconInfo();
}

nuBOOL CDrawIcon::LoadIconInfo(nuTCHAR* filename)
{
    m_fpIcon = nuTfopen(filename, NURO_FS_RB);
    if(m_fpIcon)
    {
        NUROICONFILEINFO iconFileInfo;
        if(nuFread(&iconFileInfo, sizeof(NUROICONFILEINFO), 1, m_fpIcon) == 1)
        {
            m_pIconInfo = (nuBYTE*)nuMalloc(sizeof(NUROICONFILEINFO)+iconFileInfo.nIconNum*sizeof(NUROICONINFO));
            if(m_pIconInfo == NULL)
            {
                UnloadIconInfo();
                return nuFALSE;
            }
            nuFseek(m_fpIcon, 0, NURO_SEEK_SET);
            if(nuFread(m_pIconInfo, sizeof(NUROICONFILEINFO)+iconFileInfo.nIconNum*sizeof(NUROICONINFO)
                , 1, m_fpIcon) != 1)
            {
                UnloadIconInfo();
                return nuFALSE;
            }
        }
	}
    return nuTRUE;
}
nuVOID CDrawIcon::UnloadIconInfo()
{
    if(m_pIconInfo != NULL)
    {
        nuFree(m_pIconInfo);
        m_pIconInfo = NULL;
    }
    
    if(m_fpIcon != NULL)
    {
        nuFclose(m_fpIcon);
        m_fpIcon = NULL;
	}
}
/*
nuBOOL CDrawIcon::DrawIcon(NURO_BMP* pDstBmp, int x, int y, nuLONG Icon)
{
    if(m_pIconInfo == NULL)
    {
        return nuFALSE;
    }
    NUROICONFILEINFO* pIconFileInfo = (NUROICONFILEINFO*)m_pIconInfo;
    NUROICONINFO* pIconInfo = (NUROICONINFO*)(m_pIconInfo+sizeof(NUROICONFILEINFO));
    nuUINT i = 0;
    for(i=0; i<pIconFileInfo->nIconNum; ++i)
    {
        if(pIconInfo->ID == Icon)
        {
            nuMemset(&nuIconBMP, 0, sizeof(nuIconBMP));
            nuIconBMP.bmpWidth = pIconInfo->nW;
            nuIconBMP.bmpHeight = pIconInfo->nH;
            nuIconBMP.bmpBuffLen = pIconInfo->nW*pIconInfo->nH*2;
            nuFseek(m_fpIcon, pIconInfo->nStartPos, NURO_SEEK_SET);
            if(nuFread(iconBuff, nuIconBMP.bmpBuffLen, 1, m_fpIcon) == 1)
            {
                nuIconBMP.pBmpBuff = iconBuff;
#ifdef VALUE_EMGRT
                if ( Icon == 10252 || Icon == 10253 || Icon == 10255 )
                {
                    CGdiMethodZK::nuGMCopyBmp(pDstBmp, x, y, nuIconBMP.bmpWidth, nuIconBMP.bmpHeight
                        , &nuIconBMP, 0, 0, NURO_BMP_TYPE_MIXA);
                }
                else
                {
                    CGdiMethodZK::nuGMCopyBmp(pDstBmp, x, y, nuIconBMP.bmpWidth, nuIconBMP.bmpHeight
                    , &nuIconBMP, 0, 0, NURO_BMP_TYPE_MIXX);
                }
#else
                //RotationAngle( &nuIconBMP, 0 );
                CGdiMethodZK::nuGMCopyBmp(pDstBmp, x, y, nuIconBMP.bmpWidth, nuIconBMP.bmpHeight
                    , &nuIconBMP, 0, 0, NURO_BMP_TYPE_MIXX);
#endif
            }
            break;
        }
        ++pIconInfo;
    }
    if(i == pIconFileInfo->nIconNum)
    {
        return nuFALSE;
    }
    return nuTRUE;
    
}

*/