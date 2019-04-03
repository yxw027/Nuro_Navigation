#if !defined(AFX_MAPFILEBHY_H__6FDD0C4A_7FC3_42F1_BC8C_4CF014A223CD__INCLUDED_)
#define AFX_MAPFILEBHY_H__6FDD0C4A_7FC3_42F1_BC8C_4CF014A223CD__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"
#include "nuSearchDefine.h"
class CMapFileBhY : public CFileMapBaseZ  
{
public:
	CMapFileBhY();
	virtual ~CMapFileBhY();	
	nuBOOL Open(nuLONG nMapID);
	nuVOID Close();
    nuBOOL GetHeadInfo(SEARCH_BH_HEAD &head);
/*	nuBOOL GetBHInfo(nuUINT nBlkID, SEARCH_BH_INFO *pbi);*/
	nuUINT GetBHIdxInfo(nuVOID *pBuffer, nuUINT nMaxLen);
/*	nuUINT GetBHRoadAddr(const nuUINT &nBlkID);*/
	nuUINT GetBHPoiAddrByIdx(const nuUINT &nBlkIDx);
	nuUINT GetBHRoadAddrByIdx(const nuUINT &nBlkIDx);
	nuUINT GetBlkNum();
//	nuUINT GetBlkIdx(const nuUINT &nBlkID);
private:	
	nuCFile file;
	nuUINT blkNum;
    SEARCH_BH_HEAD m_head;
/*	nuUINT *pBHInfo;*/
};

#endif // !defined(AFX_MAPFILEBHY_H__6FDD0C4A_7FC3_42F1_BC8C_4CF014A223CD__INCLUDED_)
