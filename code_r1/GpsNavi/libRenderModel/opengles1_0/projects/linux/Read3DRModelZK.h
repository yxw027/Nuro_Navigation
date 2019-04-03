// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: Read3DRModelZK.h,v 1.9 2010/04/13 09:16:50 zealass Exp $
// $Author: zealass $
// $Date: 2010/04/13 09:16:50 $
// $Locker:  $
// $Revision: 1.9 $
// $Source: /home/nuCode/libRenderModel/Read3DRModelZK.h,v $
/////////////////////////////////////////////////////////////////////////

#if !defined(AFX_READ3DRMODELZK_H__59B804DD_468A_4F80_B058_3BCC6D8A6879__INCLUDED_)
#define AFX_READ3DRMODELZK_H__59B804DD_468A_4F80_B058_3BCC6D8A6879__INCLUDED_
#include "defines.h"

#include "ReadBaseZK.h"
#include "nuro3DModelStruct.h"
#include "nuroModuleApiStruct.h"

class CRead3DRModelZK  : public CReadBaseZK
{
public:
    CRead3DRModelZK();
    virtual ~CRead3DRModelZK();    
    
    bool	Initialize(class COpenGLProcessZK*	pOpenGL);
    void	Free();
    
    nuUINT	AddModel(long nID, long x, long y, const char* psz3DR
        , long rx, long ry, long ty, int height, int dCenD, float fs, float fz
        , PMATHTOOLAPI pMT);
    nuUINT	DelModel(long nID, long x, long y);
    nuUINT	DrawModel();
protected:
    bool	Read3DRModel(const char* psz3DR);
    bool	Read3DRData(nuFILE *pf3DR);
    void	DeltOneModel(PMODEL_3DMAX_DATA pstuMdl);
    bool	MakeOpenGLTexture(nuFILE *pf3DR);
    void	FreeOpenGLTexture(nuUINT nTexture);
protected:
    class COpenGLProcessZK*		m_pOpenGL;
    MODEL_3DMAX_DATA			m_pstuMdlArray[_ZK_MAX_COUNT_MODEL_LOADEN];
    nuBYTE						m_nNowMdlCount;
    
};

#endif // !defined(AFX_READ3DRMODELZK_H__59B804DD_468A_4F80_B058_3BCC6D8A6879__INCLUDED_)
