// NuroCodeListZK.h: interface for the CNuroCodeListZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROCODELISTZK_H__253344EC_2283_4628_8789_FF9995CBB00B__INCLUDED_)
#define AFX_NUROCODELISTZK_H__253344EC_2283_4628_8789_FF9995CBB00B__INCLUDED_

#include "../NuroDefine.h"

#define _CODE_ACTION_AT_END                 0x00
#define _CODE_ACTION_AT_HEAD                0x01
#define _CODE_ACTION_DEL_ALL_OTHERS         0x02
#define _CODE_ACTION_FULL_COVER             0x03
#define _CODE_ACTION_FULL_DEL               0x04

typedef struct tagNURO_CODE_NODE
{
    nuLONG      nCodeID;
    nuLONG      nParamX;
    nuLONG      nParamY;
    nuLONG      nParamZ;
    nuLONG      nParamEx;
    nuPVOID     pExtend;
}NURO_CODE_NODE, *PNURO_CODE_NODE;

#define _CODE_NODE_DEFAULT_COUNT                16

class CNuroCodeListZK  
{
public:
	CNuroCodeListZK();
	virtual ~CNuroCodeListZK();

    nuBOOL  CreateCodeList(nuUINT nCodeCount = _CODE_NODE_DEFAULT_COUNT);
    nuVOID  DeleteCodeList();

    nuBOOL  PushNode(const NURO_CODE_NODE& codeNode, nuBYTE nAcType = _CODE_ACTION_AT_END);
    nuBOOL  PopNode(NURO_CODE_NODE& codeNode, nuBYTE nAcType = _CODE_ACTION_AT_HEAD);
protected:
    nuUINT              m_nCodeCount;
    PNURO_CODE_NODE     m_pCodeList;
    nuUINT              m_nNowCount;
    nuUINT              m_nFIndex;
    nuUINT              m_nLIndex;
    nuBOOL              m_bWorking;

};

#endif // !defined(AFX_NUROCODELISTZK_H__253344EC_2283_4628_8789_FF9995CBB00B__INCLUDED_)
