// NuroEncryptZK.h: interface for the CNuroEncryptZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROENCRYPTZK_H__96EF6179_E0D4_41AD_B8A3_A1F85B29970E__INCLUDED_)
#define AFX_NUROENCRYPTZK_H__96EF6179_E0D4_41AD_B8A3_A1F85B29970E__INCLUDED_

#ifndef MAKE_LICENSE

#include "NuroDefine.h"

#else
//For making license file
//#define nuBYTE          BYTE
//#define nuINT           int
//#define nuUINT          UINT
//#define nuWORD          WORD
//#define nuVOID          void
#define nuLOBYTE(w)           ((nuBYTE)(w))
#define nuHIBYTE(w)           ((nuBYTE)(((nuWORD)(w) >> 8) & 0xFF))
#define NURO_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define NURO_MIN(a, b) (((a) > (b)) ? (b) : (a))
#define nuMAKEWORD(a, b)      ((nuWORD)(((nuBYTE)(a)) | ((nuWORD)((nuBYTE)(b))) << 8))
#endif

#define DES_NUM             8
typedef struct tagDES_DATA
{
    nuBYTE  pData[DES_NUM];
}DES_DATA;

class CNuroEncryptZK  
{
public:
	CNuroEncryptZK();
	virtual ~CNuroEncryptZK();
    nuUINT SetKey(nuBYTE* pKey, nuINT nKeyLen);

    nuUINT Encrypt(DES_DATA& data, nuINT nLoops = 16);
private:
    inline nuVOID IPMapping(DES_DATA& data);
    inline nuVOID IP1Mapping(DES_DATA& data);
    inline nuBYTE GetBit(nuBYTE nValue, nuBYTE nBits);
    inline nuVOID SetBit(nuBYTE& nDesV, nuBYTE nValue, nuBYTE nBits);

    inline nuVOID ETrans32to48(nuBYTE* pIn, nuBYTE *pOut);
    inline nuVOID SMapping(nuBYTE* pData48, nuBYTE *pData32);
    inline nuVOID PMapping(nuBYTE* pData32);
    //For Key 
    inline nuVOID Key64To56(nuBYTE* pKey64, nuBYTE *pKey56);
    inline nuVOID KeyLsCD(nuBYTE *pKey56, nuBYTE nBits);
    inline nuVOID Key56To48(nuBYTE *pKey56, nuBYTE *pKey48);

    inline nuVOID DataCopy(nuBYTE* pDes, const nuBYTE* pSrc, nuINT nLen);
    inline nuVOID DataSet(nuBYTE* pDes, nuBYTE nV, nuINT nLen);
    //For mapping value
public:
    DES_DATA    m_key;
};

#endif // !defined(AFX_NUROENCRYPTZK_H__96EF6179_E0D4_41AD_B8A3_A1F85B29970E__INCLUDED_)
