// FileMethodsZK.h: interface for the CFileMethodsZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMETHODSZK_H__26539CDB_9308_47B9_B8F5_A6B751A79A81__INCLUDED_)
#define AFX_FILEMETHODSZK_H__26539CDB_9308_47B9_B8F5_A6B751A79A81__INCLUDED_

#include "../NuroDefine.h"

class CFileMethodsZK  
{
public:
	CFileMethodsZK();
	virtual ~CFileMethodsZK();

public:
    static nuINT CfgAnalyse( nuBYTE *pData, //the data buffer to process
                             nuUINT nDataLen, //the length of data
                             nuCHAR *pNameBuf, //for the property name
                             nuWORD *pNameBufNum,
                             nuWORD *pValue, //the value buffer
                             nuWORD *pValueNum );//IN: the number of value buffer; OUT:the number of usefull value.
};

#endif // !defined(AFX_FILEMETHODSZK_H__26539CDB_9308_47B9_B8F5_A6B751A79A81__INCLUDED_)
