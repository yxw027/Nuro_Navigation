// NuroEncryptZK.cpp: implementation of the CNuroEncryptZK class.
//
//////////////////////////////////////////////////////////////////////

#ifdef MAKE_LICENSE
#include "stdafx.h"
#endif
#include "NuroEncryptZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const nuBYTE pgIP_MAP[] = {
    58, 50, 42, 34, 26, 18, 10, 2, 
    60, 52, 44, 36, 28, 20, 12, 4, 
    62, 54, 46, 38, 30, 22, 14, 6, 
    64, 56, 48, 40, 32, 24, 16, 8, 
    57, 49, 41, 33, 25, 17, 9, 1, 
    59, 51, 43, 35, 27, 19, 11, 3, 
    61, 53, 45, 37, 29, 21, 13, 5, 
    63, 55, 47, 39, 31, 23, 15, 7 
};
const nuBYTE pgP_MAP[] = {
    16, 7,  20, 21, 
    29, 12, 28, 17, 
    1,  15, 23, 26, 
    5,  18, 31, 10, 
    2,  8,  24, 14, 
    32, 27, 3,  9, 
    19, 13, 30, 6, 
    22, 11, 4,  25  
};
const nuBYTE pgIP_1_MAP[] = {
    40, 8, 48, 16, 56, 24, 64, 32, 
    39, 7, 47, 15, 55, 23, 63, 31, 
    38, 6, 46, 14, 54, 22, 62, 30, 
    37, 5, 45, 13, 53, 21, 61, 29, 
    36, 4, 44, 12, 52, 20, 60, 28, 
    35, 3, 43, 11, 51, 19, 59, 27, 
    34, 2, 42, 10, 50, 18, 58, 26, 
    33, 1, 41,  9, 49, 17, 57, 25  
};
const nuBYTE pgS_MAP[8*4*16] = { 
    //S1
    14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
    0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
    4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
    15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,
    //S2
    15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
    3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
    0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
    13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,
    //S3
    10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
    13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
    13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
    1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,
    //S4
    7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
    13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
    10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
    3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,
    //S5
    2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
    14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
    4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
    11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,
    //S6
    12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
    10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
    9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
    4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,
    //S7
    4, 11, 2, 14, 15, 0, 9, 13, 3, 12, 9, 7, 5, 10, 6, 1,
    13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
    1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
    6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,
    //S8
    13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
    1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
    7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
    2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11,
};
//

//For key
const nuBYTE pgPC_1_MAP[] = {
    57, 49, 41, 33, 25, 17, 9, 
    1, 58, 50, 42, 34, 26, 18, 
    10, 2, 59, 51, 43, 35, 27, 
    19, 11, 3, 60, 52, 44, 36, 
    63, 55, 47, 39, 31, 23, 15, 
    7, 62, 54, 46, 38, 30, 22, 
    14, 6, 61, 53, 45, 37, 29, 
    21, 13, 5, 28, 20, 12, 4 
};
const nuBYTE pgLS_MAP[] =  {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};
const nuBYTE pgPC_2_MAP[] = {
    14, 17, 11, 24, 1, 5, 
    3, 28, 15, 6, 21, 10, 
    23, 19, 12, 4, 26, 8, 
    16, 7, 27, 20, 13, 2, 
    41, 52, 31, 37, 47, 55, 
    30, 40, 51, 45, 33, 48, 
    44, 49, 39, 56, 34, 53, 
    46, 42, 50, 36, 29, 32 
};

CNuroEncryptZK::CNuroEncryptZK()
{

}

CNuroEncryptZK::~CNuroEncryptZK()
{

}

nuUINT CNuroEncryptZK::SetKey(nuBYTE* pKey, nuINT nKeyLen)
{
    for(nuINT i = 0; i < NURO_MIN(nKeyLen, DES_NUM); ++i)
    {
        m_key.pData[i] = pKey[i];
    }
    return 1;
}

nuUINT CNuroEncryptZK::Encrypt(DES_DATA& data, nuINT nLoops /* = 16 */)
{
	nuBYTE pDataE[6] = {0};
	nuBYTE pKey[7]	 = {0};
	nuBYTE pKey48[6] = {0};
	nuBYTE pData32[4] = {0};
	nuBYTE pDataR[4]  = {0}, pDataL[4] = {0};
    IPMapping(data);//bits' replacement
    DataCopy(pDataL, data.pData, 4);
    DataCopy(pDataR, &data.pData[4], 4);
    Key64To56(m_key.pData, pKey);
    for(nuINT i = 0; i < nLoops; ++i)
    {
        //GetKey
        KeyLsCD(pKey, pgLS_MAP[i]);
        Key56To48(pKey, pKey48);
        //
        ETrans32to48(pDataR, pDataE);//Ri to 48bits
        nuINT j;
        for(j = 0; j < 6; ++j)//XOR
        {
            pDataE[j] ^= pKey48[j];
        }
        SMapping(pDataE, pData32);//S mapping
        PMapping(pData32);//P mapping to f1
        for( j = 0; j < 4; ++j )
        {
            pData32[j]  ^= pDataL[j];
            pDataL[j]   = pDataR[j];
            pDataR[j]   = pData32[j];
        }
    }
    DataCopy(data.pData, pDataR, 4);
    DataCopy(&data.pData[4], pDataL, 4);
    IP1Mapping(data);
    return 1;
}

inline nuVOID CNuroEncryptZK::IPMapping(DES_DATA& data)
{
	DES_DATA tmp = {0};
    for(nuUINT i = 0; i < 64; ++i)
    {
        nuUINT idx1 = i / 8;
        nuUINT bit1 = i % 8;
        nuUINT j = (pgIP_MAP[i]-1);
        nuUINT idx2 = j / 8;
        nuUINT bit2 = j % 8;
        SetBit(tmp.pData[idx1], GetBit(data.pData[idx2], bit2), bit1);
    }
    data = tmp;
}

inline nuVOID CNuroEncryptZK::IP1Mapping(DES_DATA& data)
{
    DES_DATA tmp = {0};
    for(nuUINT i = 0; i < 64; ++i)
    {
        nuUINT idx1 = i / 8;
        nuUINT bit1 = i % 8;
        nuUINT j = (pgIP_1_MAP[i]-1);
        nuUINT idx2 = j / 8;
        nuUINT bit2 = j % 8;
        SetBit(tmp.pData[idx1], GetBit(data.pData[idx2], bit2), bit1);
    }
    data = tmp;
}

inline nuBYTE CNuroEncryptZK::GetBit(nuBYTE nValue, nuBYTE nBits)
{
    return (nuBYTE)( (nValue>>nBits)&1 );
}

inline nuVOID CNuroEncryptZK::SetBit(nuBYTE& nDesV, nuBYTE nValue, nuBYTE nBits)
{
    nDesV &= ~(1<<nBits);//clear
    nDesV |= (nValue<<nBits);//set
}

inline nuVOID CNuroEncryptZK::ETrans32to48(nuBYTE* pIn, nuBYTE *pOut)
{
    nuUINT j = 31;
    nuUINT nCount = 0;
    for(nuUINT i = 0; i < 48; ++i)
    {
        nuUINT idx1 = i / 8;
        nuUINT bit1 = i % 8;
        nuUINT idx2 = j / 8;
        nuUINT bit2 = j % 8;
        SetBit(pOut[idx1], GetBit(pIn[idx2], bit2), bit1);
        ++j;
        ++nCount;
        if( 32 == j )
        {
            j = 0;
        }
        if( 6 == nCount )
        {
            j -= 2;
        }
    }
}

inline nuVOID CNuroEncryptZK::SMapping(nuBYTE* pData48, nuBYTE *pData32)
{
    nuBYTE nTmp = 0;
    nuBYTE nBits = 0;
    nuWORD nWord = 0;
    nuBYTE x = 0, y = 0;
    DataSet(pData32, 0, 4);
    for(nuINT i = 0; i < 8; ++i)
    {
        nuINT idx = nBits / 8;
        nuINT bit = nBits % 8;
        if( idx < 7 )
        {
            nWord = nuMAKEWORD(pData48[idx], pData48[idx+1]);
        }
        else
        {
            nWord = pData48[idx];
        }
        nWord >>= nBits;
        nTmp = (nuBYTE)(nWord &0x3F);
        x = GetBit(nTmp, 5) + (GetBit(nTmp, 0)<<1);
        y = GetBit(nTmp, 4) + (GetBit(nTmp, 3)<<1) + (GetBit(nTmp, 2)<<2) + (GetBit(nTmp, 1)<<3);
        nuINT nxy = i * 4 * 16 + x * 16 + y;
        pData32[i/2] |= (pgS_MAP[nxy] << ((i%2)*4) );
        //
        nBits += 6;
    }
}

inline nuVOID CNuroEncryptZK::PMapping(nuBYTE* pData32)
{
    nuBYTE pTmp32[4] = {0};
    DataCopy(pTmp32, pData32, 4);
    for(nuINT i = 0; i < 32; ++i)
    {
        nuUINT idx1 = i / 8;
        nuUINT bit1 = i % 8;
        nuUINT j = (pgP_MAP[i]-1);
        nuUINT idx2 = j / 8;
        nuUINT bit2 = j % 8;
        SetBit(pData32[idx1], GetBit(pTmp32[idx2], bit2), bit1);
    }
}

inline nuVOID CNuroEncryptZK::Key64To56(nuBYTE* pKey64, nuBYTE *pKey56)
{
    for(nuUINT i = 0; i < 56; ++i)
    {
        nuUINT idx1 = i / 8;
        nuUINT bit1 = i % 8;
        nuUINT j = (pgPC_1_MAP[i]-1);
        nuUINT idx2 = j / 8;
        nuUINT bit2 = j % 8;
        SetBit(pKey56[idx1], GetBit(pKey64[idx2], bit2), bit1);
    }
}

inline nuVOID CNuroEncryptZK::KeyLsCD(nuBYTE *pKey56, nuBYTE nBits)
{
    nuWORD nNewV = 0;
    nuBYTE nLbyte = 0;
    for(nuUINT i = 0; i < 7; ++i)
    {
        nNewV = pKey56[i];
        nNewV <<= nBits;
        pKey56[i] = nuLOBYTE(nNewV) | nLbyte;
        nLbyte    = nuHIBYTE(nNewV);
    }
    pKey56[0] |= ( pKey56[4] << (4-nBits) ) >> 4;
    pKey56[4] &= ~(( 0xF0 << (4-nBits) ) >> (4-nBits));
    pKey56[4] |= (nLbyte<<(4+nBits));
}

inline nuVOID CNuroEncryptZK::Key56To48(nuBYTE *pKey56, nuBYTE *pKey48)
{
    for(nuUINT i = 0; i < 48; ++i)
    {
        nuUINT idx1 = i / 8;
        nuUINT bit1 = i % 8;
        nuUINT j = (pgPC_2_MAP[i] - 1);
        nuUINT idx2 = j / 8;
        nuUINT bit2 = j % 8;
        SetBit(pKey48[idx1], GetBit(pKey56[idx2], bit2), bit1);
    }
}

inline nuVOID CNuroEncryptZK::DataCopy(nuBYTE* pDes, const nuBYTE* pSrc, nuINT nLen)
{
    for( nuINT i = 0; i < nLen; ++i )
    {
        pDes[i] = pSrc[i];
    }
}

inline nuVOID CNuroEncryptZK::DataSet(nuBYTE* pDes, nuBYTE nV, nuINT nLen)
{
    for( nuINT i = 0; i < nLen; ++i )
    {
        pDes[i] = nV;
    }
}