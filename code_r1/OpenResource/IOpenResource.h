// IOpenResource.h: interface for the CIOpenResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINOPENRESZK_H__7D29681E_7033_4BE6_97C4_10B4E5CC10B7__INCLUDED_)
#define AFX_MAINOPENRESZK_H__7D29681E_7033_4BE6_97C4_10B4E5CC10B7__INCLUDED_

#include "GApiOpenResource.h"
#include "NuroResource.h"

class CIOpenResource : public CGApiOpenResource
{
public:
	CIOpenResource();
	virtual ~CIOpenResource();

    virtual nuUINT          IOprInit(const OPR_INIT_PARAM& opriParam);
    virtual nuVOID          IOprFree();
    virtual OPR_PACK*       IOprLoadPack(const nuTCHAR* ptsPackage, nuINT nLoadType, nuUINT& nTotalLen);
    virtual nuVOID          IOprUnloadPack(OPR_PACK* nPackID);
    virtual OPR_PACK_FILE*  IOprFindFile(OPR_PACK* nPackID, const nuCHAR* ptsFile);
    virtual nuUINT          IOprGetFileLength(OPR_PACK* nPackID, OPR_PACK_FILE* file);
    virtual nuBOOL          IOprLoadFileData(OPR_PACK* nPackID, OPR_PACK_FILE* file, nuBYTE *pDataBuffer, nuUINT nBufLen);
    virtual nuBOOL          IOprLoadNuroBmp(OPR_PACK* nPackID, OPR_PACK_FILE* file, NURO_BMP& bitmap);
    virtual nuVOID          IOprUnloadNuroBmp(NURO_BMP& bitmap);
protected:
    nuroResource        m_resZip;

};

#endif // !defined(AFX_MAINOPENRESZK_H__7D29681E_7033_4BE6_97C4_10B4E5CC10B7__INCLUDED_)
