#if !defined(AFX_MAPFILERDWY_H__49808FD5_CB7E_478E_ADC9_9D81B6DE9257__INCLUDED_)
#define AFX_MAPFILERDWY_H__49808FD5_CB7E_478E_ADC9_9D81B6DE9257__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroDefine.h"
#include "NuroClasses.h"
#include "nuSearchDefine.h"

class CMapFileRdwY : public CFileMapBaseZ  
{
public:
	CMapFileRdwY();
	virtual ~CMapFileRdwY();
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();

	nuBOOL GetBlkInfo(nuUINT addr, SEARCH_RDW_BLKINFO* pbi);
	nuBOOL GetFileData(nuUINT nDataAddr, nuVOID *pBuffer, nuUINT nLen);
	nuBOOL GetRoadPos(nuUINT nBlkAddr, nuUINT nRoadIdx, NUROPOINT *pos, SEARCH_RDW_BLKINFO *pBlkInfo=NULL);
	//nuWORD GetRoadTownID(nuUINT nBlkAddr, nuUINT nRoadIdx); 
	nuUINT GetRoadNameAddr(nuUINT nBlkAddr, nuUINT nRoadIdx, SEARCH_RDW_BLKINFO *pBlkInfo=NULL);
	nuBOOL GetRoadA2Info(nuUINT nBlkAddr, nuUINT nRoadIdx, SEARCH_RDW_A2 &A2, SEARCH_RDW_BLKINFO *pBlkInfo=NULL);
private:
	nuCFile file;
};

#endif // !defined(AFX_MAPFILERDWY_H__49808FD5_CB7E_478E_ADC9_9D81B6DE9257__INCLUDED_)
