// NuroCfgFileZK.cpp: implementation of the CNuroCfgFileZK class.
//
//////////////////////////////////////////////////////////////////////

#include "../NuroCfgFileZK.h"
#include "../../NuroDefine.h"
#include "../GNuroFile.h"
#include "../GNuroTempMem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const nuTCHAR* CNuroCfgFileZK::sc_tsCfgPostfix[] = {
    nuTEXT(".dat"),
    nuTEXT(".ini")
};
const nuCHAR* CNuroCfgFileZK::sc_tsCfgProperty[] = {
    "FONTCOMMON",
    "FONTSIZE",
    "FONTCOLOR",
};

#define _CFG_LINES_MAX                      400

CNuroCfgFileZK::CNuroCfgFileZK()
{

}

CNuroCfgFileZK::~CNuroCfgFileZK()
{

}

nuBOOL CNuroCfgFileZK::ReadCfgData(nuPVOID lpData, const nuTCHAR* ptsFile)
{
    if( NULL == lpData || NULL == ptsFile )
    {
        return nuFALSE;
    }
    if( ProcCfgDat(lpData, ptsFile) )
    {
        return nuTRUE;
    }
    if( !ReadCfgIni(lpData, ptsFile) )
    {
        return nuFALSE;
    }
    WriteCfgDat(lpData, ptsFile);
    return nuTRUE;
}

nuBOOL CNuroCfgFileZK::ProcCfgDat(nuPVOID lpData, const nuTCHAR* ptsFile)
{
    return nuFALSE;
}

nuBOOL CNuroCfgFileZK::ReadCfgIni(nuPVOID lpData, const nuTCHAR* ptsFont)
{
    CGNuroFile file;
    nuTCHAR tsFile[NURO_MAX_PATH];
    nuTcscpy(tsFile, ptsFont);
    nuTcscat(tsFile, sc_tsCfgPostfix[1]);
    if( !file.Open(tsFile, NURO_FS_RB) )
    {
        return nuFALSE;
    }
    CGNuroTempMem tmpMem;
    nuSIZE nSize = file.Fgetlen();
    nuBYTE *pData = (nuBYTE*)tmpMem.Malloc(nSize);
	nuMemset(pData,0,sizeof(nuCHAR)*nSize);
    if( NULL == pData )
    {
        return nuFALSE;
    }
    CGNuroTempMem memCfgLines;
    CFG_SINGLE_LINE* pCfgLines = (CFG_SINGLE_LINE*)memCfgLines.Malloc(_CFG_LINES_MAX*sizeof(CFG_SINGLE_LINE));
	nuMemset(pCfgLines,0,sizeof(nuCHAR)*(_CFG_LINES_MAX*sizeof(CFG_SINGLE_LINE)));
    if( NULL == pCfgLines )
    {
        return nuFALSE;
    }
    if( file.Read(pData, nSize,1) != 1 )
    {
        return nuFALSE;
    }
    nuUINT i = 0, j =0;
    while( i < nSize && j < _CFG_LINES_MAX )
    {
        i += DataAnalyse(&pData[i], nSize-i, pCfgLines[j]);
        if( pCfgLines[j].nValueNum > 0 )
        {
            ++j;
        }
    }
    return ProcCfgIni(lpData, pCfgLines, j);
    //
}

////...............................................................................................
inline nuBOOL CNuroCfgFileZK::IsLineEnd(const nuBYTE *pData, const nuUINT& nDataLen, nuUINT& i)
{
    if( 0x0D == pData[i] && i < (nDataLen-1) && 0x0A == pData[i+1] )
    {
        i += 2;
        return nuTRUE;
    }
    return nuFALSE;
}

inline nuVOID CNuroCfgFileZK::ToLineEnd(const nuBYTE *pData, const nuUINT& nDataLen, nuUINT& i)
{
    while ( i < nDataLen - 1 )
    {
        if( 0x0D == pData[i] && 0x0A == pData[i+1] )
        {
            i += 2;
            break;
        }
        ++i;
    }
}

inline nuBOOL CNuroCfgFileZK::GetPropertyName(const nuBYTE *pData, 
                                              const nuUINT& nDataLen, 
                                              nuUINT& i, 
                                              CFG_SINGLE_LINE& cfgLine)
{
    while( i < nDataLen )//skip the blanks of the beginning.
    {
        if( '[' == pData[i] || ' ' == pData[i] )
        {
            ++i;
        }
        else if( IsLineEnd(pData, nDataLen, i) )
        {
            return nuFALSE;
        }
        else
        {
            break;
        }
    }
    nuUINT nIdx1 = i;
    nuUINT nIdx2 = 0;
    nuBOOL bFindName = nuFALSE;
    while( i < nDataLen )//look for the ']' and skip the blanks of the end
    {
        if( ']' == pData[i] )
        {
            ++i;
            bFindName = nuTRUE;
            break;
        }
        else if( IsLineEnd(pData, nDataLen, i) )
        {
            break;
        }
        else
        {
            if( ' ' != pData[i] )
            {
                nIdx2 = i;
            }
            ++i;
        }
    }
    if( !bFindName )//Can't Find
    {
        return nuFALSE;
    }
    nuINT nCount = nIdx2 - nIdx1 + 1;
    if( nCount < 0 || nCount >= _CFG_PROPERTY_NAME_MAX_NUM )//it is an invalid property
    {
        ToLineEnd(pData, nDataLen, i);
        return nuFALSE;
    }
    nuMemcpy(cfgLine.sPropertyName, &pData[nIdx1], nCount);
    cfgLine.sPropertyName[nCount] = '\0';
    return nuTRUE;
}

inline nuBOOL CNuroCfgFileZK::IsFindEqual(const nuBYTE* pData, const nuUINT& nDataLen, nuUINT& i)
{
    while( i < nDataLen )//look for the '='
    {
        if( '=' == pData[i] )
        {
            return nuTRUE;
        }
        else if( IsLineEnd(pData, nDataLen, i) )
        {
            return nuFALSE;
        }
        ++i;
    }
    return nuFALSE;
}

inline nuUINT CNuroCfgFileZK::GetValues(const nuBYTE* pData, const nuUINT& nDataLen, nuUINT& i, CFG_SINGLE_LINE& cfgLine)
{
    nuCHAR sNumName[50];
    nuUINT j = 0;
    nuUINT nIdx1 = 0;
    while( j < _CFG_PROPERTY_VALUE_MAX_NUM && i < nDataLen )
    {
        if( 0x0D == pData[i] || ' ' == pData[i] || /*tab*/0x09 == pData[i] )
        {
            if( 0 != nIdx1 )
            {
                if( i - nIdx1 < 50 )
                {
                    nuMemcpy(sNumName, &pData[nIdx1], i - nIdx1);
                    sNumName[i - nIdx1] = '\0';
                    StrToValue(sNumName, cfgLine.pValue[j]);
                    ++j;
                }
                nIdx1 = 0;
            }
            //
            ++i;
            if( i < nDataLen && 0x0A == pData[i] )
            {
                ++i;
                break;
            }
        }
        else
        {
            if( 0 == nIdx1 )
            {
                nIdx1 = i;
            }
            ++i;
        }
    }
    if( j < _CFG_PROPERTY_VALUE_MAX_NUM && 0 != nIdx1 )
    {
        if( i - nIdx1 < 50 )
        {
            nuMemcpy(sNumName, &pData[nIdx1], i - nIdx1);
            sNumName[i - nIdx1] = '\0';
            StrToValue(sNumName, cfgLine.pValue[j]);
            ++j;
        }
        nIdx1 = 0;
    }
    cfgLine.nValueNum = j;
    return 1;
}

inline nuBOOL CNuroCfgFileZK::StrToValue(const nuCHAR *pStr, nuLONG& nValue)
{
    nuINT nLen = nuStrlen(pStr);
    nuINT i = 0;
    nuINT nBase = 10;
    nValue = 0;
    while(i < nLen)
    {
        if( pStr[i] == 'x' || pStr[i] == 'X' )
        {
            ++i;
            nBase = 16;
            break;
        }
        else if( pStr[i] == 'o' || pStr[i] == 'O' )
        {
            ++i;
            nBase = 8;
            break;
        }
        else if( pStr[i] == 'b' || pStr[i] == 'B' )
        {
            ++i;
            nBase = 2;
            break;
        }
        else if( '1' <= pStr[i] && '9' >= pStr[i] )
        {
            break;
        }
        ++i;
    }
    if( i == nLen )
    {
        return nuFALSE;
    }
    nuCHAR* pSEnd = NULL;
    nValue = nuStrtol(&pStr[i], &pSEnd, nBase);
    return nuTRUE;
}

nuUINT CNuroCfgFileZK::DataAnalyse(nuBYTE *pData,  nuUINT nDataLen,  CFG_SINGLE_LINE& cfgLine)
{
    nuUINT i;
    cfgLine.sPropertyName[0] = '\0';
    cfgLine.nValueNum        = 0;
    for(i = 0; i < nDataLen; ++i)
    {
        if( '#' == pData[i] )//Skipping line, calculate the length of line
        {
            ++i;
            ToLineEnd(pData, nDataLen, i);
            break;
        }
        if( IsLineEnd(pData, nDataLen, i) )//end of a line
        {
            break;
        }
        //
        if( '[' == pData[i] )
        {
            ++i;
            //
            if( !GetPropertyName(pData, nDataLen, i, cfgLine) )//[STRING]
            {
                break;
            }
            if( !IsFindEqual(pData, nDataLen, i) )//can't find the '=', break
            {
                break;
            }
            ++i;
            GetValues(pData, nDataLen, i, cfgLine);
            break;
        }
    }
    return i;
}