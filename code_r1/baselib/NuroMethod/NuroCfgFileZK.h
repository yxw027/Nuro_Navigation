// NuroCfgFileZK.h: interface for the CNuroCfgFileZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROCFGFILEZK_H__475F3ED1_046E_4E0A_A066_C3941307588E__INCLUDED_)
#define AFX_NUROCFGFILEZK_H__475F3ED1_046E_4E0A_A066_C3941307588E__INCLUDED_

#include "../NuroDefine.h"

class CNuroCfgFileZK  
{
public:
#define _CFG_PROPERTY_NAME_MAX_NUM           64
#define _CFG_PROPERTY_VALUE_MAX_NUM          64
    typedef struct tagCFG_SINGLE_LINE
    {
        nuCHAR  sPropertyName[_CFG_PROPERTY_NAME_MAX_NUM];
        nuLONG  pValue[_CFG_PROPERTY_VALUE_MAX_NUM];
        nuUINT  nValueNum;
    }CFG_SINGLE_LINE, *PCFG_SINGLE_LINE;
    //....................................................
#define _CFG_PROPERTY_FONTCOMMON                0
#define _CFG_PROPERTY_FONTSIZE                  1
#define _CFG_PROPERTY_FONTCOLOR                 2
public:
	CNuroCfgFileZK();
	virtual ~CNuroCfgFileZK();
    //interface
    virtual nuBOOL ReadCfgFile(const nuTCHAR* ptsFile) = 0;
    virtual nuVOID FreeCfgData(/*nuPVOID lpData*/) = 0;
protected:
    virtual nuBOOL ProcCfgDat(nuPVOID lpData, const nuTCHAR* ptsFile) = 0;
    virtual nuBOOL ProcCfgIni(nuPVOID lpData, CFG_SINGLE_LINE* pCfgLine, nuUINT nCount) = 0;
    virtual nuBOOL WriteCfgDat(nuPVOID lpData, const nuTCHAR* ptsFile) = 0;
    //
    nuBOOL ReadCfgData(nuPVOID lpData, const nuTCHAR* ptsFile);
private:
    nuBOOL ReadCfgIni(nuPVOID lpData, const nuTCHAR* ptsFont);

protected:
    //.....................................................................................................
    nuUINT DataAnalyse( nuBYTE *pData, //the data buffer to process
        nuUINT nDataLen, //the length of data
        CFG_SINGLE_LINE& cfgLine );
private:
    inline nuBOOL IsLineEnd(const nuBYTE *pData, const nuUINT& nDataLen, nuUINT& i);
    inline nuVOID ToLineEnd(const nuBYTE *pData, const nuUINT& nDataLen, nuUINT& i);
    inline nuBOOL GetPropertyName(const nuBYTE *pData, const nuUINT& nDataLen, nuUINT& i, CFG_SINGLE_LINE& cfgLine);
    inline nuBOOL IsFindEqual(const nuBYTE* pData, const nuUINT& nDataLen, nuUINT& i);
    inline nuUINT GetValues(const nuBYTE* pData, const nuUINT& nDataLen, nuUINT& i, CFG_SINGLE_LINE& cfgLine);
    inline nuBOOL StrToValue(const nuCHAR *pStr, nuLONG& nValue);
protected:
    const static nuTCHAR* sc_tsCfgPostfix[];
    const static nuCHAR* sc_tsCfgProperty[];

};

#endif // !defined(AFX_NUROCFGFILEZK_H__475F3ED1_046E_4E0A_A066_C3941307588E__INCLUDED_)
