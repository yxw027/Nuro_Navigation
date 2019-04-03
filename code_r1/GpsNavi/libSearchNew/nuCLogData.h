// nuCLogData.h: interface for the nuCLogData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUCLOGDATA_H__C041AF75_1AE1_432F_8A9B_E7CD6A325727__INCLUDED_)
#define AFX_NUCLOGDATA_H__C041AF75_1AE1_432F_8A9B_E7CD6A325727__INCLUDED_

#include "NuroClasses.h"
#include "nuSearchDefine.h"

/************
* @class nuCLogData
* 
* @brief 
***************/
class nuCLogData
{	
public:
	static nuBOOL Create(nuTCHAR *pFileName, nuUINT nMax, nuUINT nEachRecordLen, nuINT mode=0);

public:
	nuCLogData();
	virtual ~nuCLogData();

	nuBOOL Open(nuTCHAR *pFileName);
	nuBOOL IsOpen();
	nuVOID Close();

	nuBOOL GetHeader(LOGDATAFILEHEADER *fileHeader);
	nuBOOL AddRecord(nuVOID *pBuffer, nuUINT nLen);
	nuVOID DelRecord(nuUINT idx);
	nuVOID ClearRecord();

	nuUINT GetRecord(nuUINT sIdx, nuUINT nMax, nuVOID *pBuffer, nuUINT nBufferLen);

private:
	nuBOOL WriteAtPos(nuUINT idx, nuVOID *pBuffer, nuUINT nLen);
	nuBOOL FlushFileInfo(nuUINT num);
	
private:
	nuCFile m_file;
	LOGDATAFILEHEADER m_header;
	nuWORD m_table[_LOGDATAFILE_DATA_MAXNUM];

};

#endif // !defined(AFX_NUCLOGDATA_H__C041AF75_1AE1_432F_8A9B_E7CD6A325727__INCLUDED_)
