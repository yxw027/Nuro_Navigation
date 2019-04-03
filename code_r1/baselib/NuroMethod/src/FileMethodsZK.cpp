// FileMethodsZK.cpp: implementation of the CFileMethodsZK class.
//
//////////////////////////////////////////////////////////////////////

#include "../FileMethodsZK.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileMethodsZK::CFileMethodsZK()
{
	
}

CFileMethodsZK::~CFileMethodsZK()
{

}

nuINT CFileMethodsZK::CfgAnalyse( nuBYTE *pData,  nuUINT nDataLen,  nuCHAR *pNameBuf,  
                                  nuWORD *pNameBufNum, nuWORD *pValue,  nuWORD *pValueNum )
{
    if( NULL == pData || 
        NULL == pNameBuf || 
        NULL == pValue || 
        NULL == pValueNum )
    {
        return 0;
    }
    nuUINT i;
    nuUINT nMaxValue = *pValueNum;
    nuUINT nMaxNamBuf = *pNameBufNum;
    *pNameBufNum = 0;
    *pValueNum   = 0;
    for(i = 0; i < nDataLen; ++i)
    {
        if( '#' == pData[i] )//Skipping line, calculate the 
        {
            ++i;
            while ( i < nDataLen - 1 )
            {
                if( 0x0D == pData[i] && 0x0A == pData[i+1] )
                {
                    i += 2;
                    break;
                }
                ++i;
            }
            break;
        }
        else if( 0x0D == pData[i] )//end of a line
        {
            ++i;
            if( i < nDataLen && 0x0A == pData[i] )
            {
                ++i;
            }
            break;
        }
        else if( '[' == pData[i] )
        {
            ++i;
            while( i < nDataLen )//skip the blanks of the beginning.
            {
                if( '[' == pData[i] || ' ' == pData[i] )
                {
                    ++i;
                }
                else
                {
                    break;
                }
            }
            nuDWORD nIdx1 = i;
            nuDWORD nIdx2 = 0;
            nuBOOL bFindName = nuFALSE;

			while( i < nDataLen ) //look for the ']' and skip the blanks of the end
			{
                if( ']' == pData[i] )
                {
                    ++i;
                    bFindName = nuTRUE;
					break;
                }
                else if( 0x0D == pData[i] )
                {
                    ++i;
                    if( i < nDataLen && 0x0A == pData[i] )
                    {
                        ++i;
                    }
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
                break;
            }
            if( (nIdx2 < nIdx1) || (nIdx2-nIdx1+1) >= nMaxNamBuf )//it is an invalid property
            {
                while( i < nDataLen )//reposition to the end of a line
                {
                    if( 0x0D == pData[i] )
                    {
                        ++i;
                        if( i < nDataLen && 0x0A == pData[i] )
                        {
                            ++i;
                        }
                        break;
                    }
                    ++i;
                }
                break;
            }
            bFindName = nuFALSE;
            while( i < nDataLen )//look for the '='
            {
                if( '=' == pData[i] )
                {
                    ++i;
                    bFindName = nuTRUE;
                    break;
                }
                else if( 0x0D == pData[i] )
                {
                    ++i;
                    if( i < nDataLen && 0x0A == pData[i] )
                    { 
                        ++i;
                    }
                    break;
                }
                else
                {
                    ++i;
                }
            }
            if( !bFindName )//can't find the '=', break
            {
                break;
            }
            *pNameBufNum = (nuWORD)(nIdx2 - nIdx1 + 1);
            nuMemcpy(pNameBuf, &pData[nIdx1], *pNameBufNum);
            pNameBuf[*pNameBufNum] = '\0';
            nuCHAR sNumName[50];
            nuUINT j = 0;
            nIdx1 = 0;
            nuCHAR *pStr;
            while( j < nMaxValue && i < nDataLen )
            {
                if( 0x0D == pData[i] || ' ' == pData[i] || 0x09 == pData[i] )
                {
                    if( 0 != nIdx1 )
                    {
                        if( i - nIdx1 < 50 )
                        {
                            nuMemcpy(sNumName, &pData[nIdx1], i - nIdx1);
                            sNumName[i - nIdx1] = '\0';
                            nuLONG nValue = nuStrtol(sNumName, &pStr, 10);
                            pValue[j] = nuLOWORD(nValue);
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
            *pValueNum = (nuWORD)j;
            break;
        }
    }
    return i;
}