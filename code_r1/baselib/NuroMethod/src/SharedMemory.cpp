// SharedMemory.cpp: implementation of the CSharedMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "../SharedMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSharedMemory::CSharedMemory()
{
	m_hFileMapping  = NULL;
	m_pMemStartAddr = NULL;
}

CSharedMemory::~CSharedMemory()
{
	//UnmapViewOfFile( m_pMemStartAddr );
	//ReleaseSharedMemory();
}
/**/
int CSharedMemory::CreateSharedMemory( long nPerStruSize )
{
	m_hFileMapping = CreateFileMapping( INVALID_HANDLE_VALUE,
									    NULL,
										PAGE_READWRITE, 
										0, 
										SHARED_MEM_SIZE,
										TEXT("StarLeeSharedMemory") );
	if ( (m_hFileMapping == NULL) || (m_hFileMapping == INVALID_HANDLE_VALUE) )
	{
		return -1;
	}
	bool bExist = false;
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		bExist = true;
	}

	HEADERMEM *pHeader = (HEADERMEM *)MapViewOfFile( m_hFileMapping, 
									     FILE_MAP_WRITE,
										 0,
										 0,
										 SHARED_MEM_SIZE );
	if ( pHeader == NULL )
	{
		return -1;
	}
	m_pMemStartAddr = pHeader ;

	if ( !bExist )
	{
		pHeader->nTotalSize = ( SHARED_MEM_SIZE - sizeof(HEADERMEM ) )/ nPerStruSize;
		pHeader->nNowSize = 0;
	}

	return 0;
}	

int CSharedMemory::ReleaseSharedMemory()
{
	//if ( m_hFileMapping )
	//{
	//	CloseHandle( m_hFileMapping );
	//}
	return 0;
}

bool CSharedMemory::AddSharedMemory(long nSize)
{
    while( bInMutex() )
	{
		Sleep(10);
	}
	HEADERMEM *pHeader = (HEADERMEM *)m_pMemStartAddr;
	bool bRet = false;
	if ( nSize == 0 )
	{
		pHeader->nNowSize = 0;
	}
	else
	{
		if ( pHeader->nNowSize >= pHeader->nTotalSize )
		{
			bRet =  false;
		}
		else
		{
			bRet = true;
			pHeader->nNowSize += nSize;
		}
		
	}
	bOutMutex();
	return bRet;
}

int CSharedMemory::GetShareMem( void *pBuff,  long nSize, long nIndex )
{
	while( bInMutex() )
	{
		Sleep(10);
	}
	memcpy( pBuff, (BYTE *)m_pMemStartAddr + nIndex * nSize + sizeof(HEADERMEM), nSize );
	bOutMutex();
	return 0;
}
 
int CSharedMemory::WriteShareMem(void *pDate, long nSize, long nIndex)
{
	while( bInMutex() )
	{
		Sleep(10);
	}
	memcpy( (BYTE *)m_pMemStartAddr + nIndex * nSize + sizeof(HEADERMEM), pDate, nSize );
	bOutMutex();
	return 0;
}

long CSharedMemory::GetShareStruNum()
{
	long nRet = 0;
	while( bInMutex() )
	{
		Sleep(10);
	}
	nRet = ((HEADERMEM *)m_pMemStartAddr)->nNowSize;
	bOutMutex();
	return nRet;
}

bool CSharedMemory::bSetFirst()
{
	while( bInMutex() )
	{
		Sleep(10);
	}
	HEADERMEM *pHeader = (HEADERMEM *)m_pMemStartAddr;
	bool bRet = false;
	if ( pHeader->bFirst == 0 )
	{
		bRet = true;
		pHeader->bFirst = 1;
	}
	bOutMutex();
	return bRet;
}

long CSharedMemory::GetTotalSize()
{
	while( bInMutex() )
	{
		Sleep(10);
	}
	long nRet =  ((HEADERMEM *)m_pMemStartAddr)->nTotalSize;
	bOutMutex();
	return nRet;
}

bool CSharedMemory::bInMutex()
{
	bool bRet = false;
	if ( ((HEADERMEM *)m_pMemStartAddr)->bMutex == 0 )
	{
		((HEADERMEM *)m_pMemStartAddr)->bMutex = 1;
		bRet =  true;
	}
	return !bRet;
}

bool CSharedMemory::bOutMutex()
{
	bool bRet = false;
	if ( ((HEADERMEM *)m_pMemStartAddr)->bMutex == 1 )
	{
		((HEADERMEM *)m_pMemStartAddr)->bMutex = 0;
		bRet =  true;
	}
	return bRet;
}
