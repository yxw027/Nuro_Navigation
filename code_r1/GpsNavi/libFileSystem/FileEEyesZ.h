// FileEEyesZ.h: interface for the CFileEEyesZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEEEYESZ_H__B962483F_7B72_4FA1_AEAC_FFD05BE747E4__INCLUDED_)
#define AFX_FILEEEYESZ_H__B962483F_7B72_4FA1_AEAC_FFD05BE747E4__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroMapFormat.h"

typedef struct tagEEYES_FILE_HEAD
{
	nuDWORD		nBlkCount;
	nuDWORD*	pIDList;
}EEYES_FILE_HEAD, *PEEYES_FILE_HEAD;

class CFileEEyesZ : public CFileMapBaseZ
{
public:
	CFileEEyesZ();
	virtual ~CFileEEyesZ();

	nuBOOL ReadEEyeBlk(EEYES_BLK& eEyesBlk, nuWORD nDwIdx);
	nuBOOL OpenFile();
	nuVOID CloseFile();

	static nuUINT GetEEyeTypeName(nuBYTE nType, nuPVOID pBuff, nuUINT nBuffLen);
//	static nuUINT AddLimitSpeed(nuINT nLimitSpeed, nuWCHAR* wsBuff, nuUINT nWsNum);

protected:
	nuWORD		m_nMapIdx;
	nuFILE*		m_pFile;
	EEYES_FILE_HEAD	m_fileHead;
};

#endif // !defined(AFX_FILEEEYESZ_H__B962483F_7B72_4FA1_AEAC_FFD05BE747E4__INCLUDED_)
