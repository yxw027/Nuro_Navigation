// BmpMethodsZK.h: interface for the CBmpMethodsZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPMETHODSZK_H__236330C4_7B36_4801_9171_2B3AE29A01F6__INCLUDED_)
#define AFX_BMPMETHODSZK_H__236330C4_7B36_4801_9171_2B3AE29A01F6__INCLUDED_

#include "../NuroDefine.h"

class CBmpMethodsZK
{
public:
    static nuVOID BmpInit(PNURO_BMP pBmp);
    static nuVOID BmpDestroy(PNURO_BMP pNuroBmp);
};

class CBmpLoader
{
private:
    NURO_BMP* m_bitmap;
public:
    CBmpLoader(NURO_BMP* bitmap);
    virtual ~CBmpLoader();
    //nuBOOL Load(const nuTCHAR *ptsBmpName);
    nuBOOL Load(const nuCHAR *ptsBmpName);
    nuBOOL Load(const nuWCHAR *ptsBmpName);
    static void Save(const nuCHAR *ptsBmpName, NURO_BMP* pBmp);
    static void Save(const nuWCHAR *ptsBmpName, NURO_BMP* pBmp);
};

#endif // !defined(AFX_BMPMETHODSZK_H__236330C4_7B36_4801_9171_2B3AE29A01F6__INCLUDED_)
