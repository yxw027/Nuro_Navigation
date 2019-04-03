// TransfDataFhZ.cpp: implementation of the CTransfDataFhZ class.
//
//////////////////////////////////////////////////////////////////////

#include "TransfDataFhZ.h"
#include <tchar.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransfDataFhZ::CTransfDataFhZ()
{

}

CTransfDataFhZ::~CTransfDataFhZ()
{

}

bool CTransfDataFhZ::TransfFh(const TCHAR *tsBh, const TCHAR*tsFh)
{
	if( tsBh == NULL || tsFh == NULL )
	{
		return false;
	}
	FILE *pFileBh = _tfopen(tsBh, TEXT("rb"));
	if( pFileBh == NULL )
	{
		return false;
	}
	FILE *pFileFh = _tfopen(tsFh, TEXT("wb"));
	if( pFileFh == NULL )
	{
		fclose(pFileBh);
		return false;
	}
	FhDataProc(pFileBh, pFileFh);
	fclose(pFileFh);
	fclose(pFileBh);
	return true;
}
bool CTransfDataFhZ::FhDataProc(FILE* pFileBh, FILE *pFileFh)
{
	DWORD nBlockCountUseful;
	if( fseek(pFileBh, 28, SEEK_SET) != 0 ||
		fread(&nBlockCountUseful, 4, 1, pFileBh) != 1 )
	{
		return false;
	}
	DWORD* pBlockID = new DWORD[nBlockCountUseful];
	if( pBlockID == NULL )
	{
		return false;
	}
	if( fread(pBlockID, 4, nBlockCountUseful, pFileBh) == nBlockCountUseful )
	{
		FHHEADER	fhHeader;
		fhHeader.nYBlockStart	= LOWORD(pBlockID[0] / BLOCK_COUNT_OF_EACH_LINE);
		fhHeader.nYBlockEnd		= LOWORD(pBlockID[nBlockCountUseful-1] / BLOCK_COUNT_OF_EACH_LINE);
		fhHeader.nBlockLines	= fhHeader.nYBlockEnd - fhHeader.nYBlockStart + 1;
		fhHeader.nXBlockStart	= BLOCK_COUNT_OF_EACH_LINE;
		fhHeader.nXBlockEnd		= 0;
		fhHeader.nBlockCount	= 0;
		DWORD yNow = fhHeader.nYBlockStart;
		DWORD xMin = BLOCK_COUNT_OF_EACH_LINE, xMax = 0;
		DWORD xStart = BLOCK_COUNT_OF_EACH_LINE, xEnd = 0;
		XSTARTEND* pXStartEnd = new XSTARTEND[fhHeader.nBlockLines];
		if( pXStartEnd != NULL )
		{
			DWORD nLines = 0;
			bool bRight = true;
			for (DWORD i = 0; i < nBlockCountUseful; ++i)
			{
				WORD x = LOWORD(pBlockID[i] % BLOCK_COUNT_OF_EACH_LINE);
				WORD y = LOWORD(pBlockID[i] / BLOCK_COUNT_OF_EACH_LINE);
				//.. Find the min and max volume
				if( x < fhHeader.nXBlockStart )
				{
					fhHeader.nXBlockStart = x;
				}
				if( x > fhHeader.nXBlockEnd )
				{
					fhHeader.nXBlockEnd = x;
				}
				if( yNow != y )
				{
					pXStartEnd[nLines].nXStart	= LOWORD(xStart);
					pXStartEnd[nLines].nXEnd	= LOWORD(xEnd);
					if( xEnd >= xStart )
					{
						fhHeader.nBlockCount += (xEnd - xStart + 1);
					}
					xStart = xEnd = x;
					++nLines;
					int nkk = y - yNow;
					while(nkk > 1)
					{
						pXStartEnd[nLines].nXStart	= BLOCK_COUNT_OF_EACH_LINE;
						pXStartEnd[nLines].nXEnd	= 0;
						++nLines;
						--nkk;
					}
					if( nLines == fhHeader.nBlockLines || y < yNow )
					{
						bRight = false;
						break;
					}
					yNow = y;
				}
				else
				{
					if( x < xStart )
					{
						xStart = x;
					}
					if( x > xEnd )
					{
						xEnd = x;
					}
				}
			}
			if( bRight )
			{
				pXStartEnd[nLines].nXStart	= LOWORD(xStart);
				pXStartEnd[nLines].nXEnd	= LOWORD(xEnd);
				if( xEnd >= xStart )
				{
					fhHeader.nBlockCount += (xEnd - xStart + 1);
				}
				++nLines;
				if( fwrite(&fhHeader, sizeof(FHHEADER), 1, pFileFh) != 1 ||
					fwrite(pXStartEnd, sizeof(XSTARTEND), fhHeader.nBlockLines, pFileFh) != fhHeader.nBlockLines )
				{
					bRight = false;
				}
			}
			if( bRight )
			{
				BYTE* pState = new BYTE[(fhHeader.nBlockCount+7) / 8];
				DWORD ntmpCount = fhHeader.nBlockCount;
				if( pState != NULL )
				{
					memset(pState, (-1), sizeof(BYTE) * ((fhHeader.nBlockCount+7) / 8));
					DWORD nBlkIdx = 0;
					DWORD nStateIdx = 0;
					for(WORD i = fhHeader.nYBlockStart; i <= fhHeader.nYBlockEnd; ++i)
					{
						WORD idx = i - fhHeader.nYBlockStart;
						DWORD nBlockID = i * BLOCK_COUNT_OF_EACH_LINE + pXStartEnd[idx].nXStart;
						for(WORD j = pXStartEnd[idx].nXStart; j <= pXStartEnd[idx].nXEnd; ++j)
						{
							if( nBlockID == pBlockID[nBlkIdx] )
							{
								++nBlkIdx;
							}
							else
							{
								int nByteIdx = nStateIdx / 8;
								int nBitIdx = nStateIdx % 8;
								pState[nByteIdx] &= (BYTE)(~(1<<nBitIdx));
								--ntmpCount;
							}
							++nStateIdx;
							++nBlockID;
						}
					}
					if( nBlkIdx == ntmpCount &&  ntmpCount == nBlockCountUseful )
					{
						fwrite(pState, (fhHeader.nBlockCount+7) / 8, 1, pFileFh);
					}
					else
					{
						int nError = 0;
					}
					delete[] pState;
					pState=NULL;
				}
			}
			delete[] pXStartEnd;
			pXStartEnd=NULL;
		}
	}
	delete[] pBlockID;
	pBlockID=NULL;
	return true;
}
