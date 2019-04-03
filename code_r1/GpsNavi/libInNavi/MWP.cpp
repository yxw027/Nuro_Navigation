// MWP.cpp: implementation of the CMWP class.
//
//////////////////////////////////////////////////////////////////////

#include "MWP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMWP::CMWP()
{
	m_pNewMem = NULL;
	m_nNodeTotal = 0;
	nuMemset( &m_NowNodeInfo, 0, sizeof(m_NowNodeInfo) );
}

CMWP::~CMWP()
{
	FreeNewMem();
}

nuBOOL CMWP::GetNodeNameAddr(nuTCHAR *sFIleName, NODELISTINFO NodeInfo, nuULONG &nNameAddr, nuULONG &nNameLen )
{
	//首先检测内存指针是否为空或者RTB发生改变，若为空则根据读取文件的内容分配内存
	if (   m_pNewMem == NULL 
		|| NodeInfo.nFileIndex != m_NowNodeInfo.nFileIndex
		 )
	{
		nuFILE *fp = nuTfopen( sFIleName, NURO_FS_RB );
		if ( !fp)
		{
			return nuFALSE;
		}
		nuULONG nNodeTotal = 0;
		if ( nuFread( &nNodeTotal, sizeof(nNodeTotal), 1, fp ) != 1 )
		{
			nuFclose(fp);
			return nuFALSE;
		}
		if ( nNodeTotal == 0 )
		{
			nuFclose(fp);
			return nuFALSE;
		}
		if ( !AllocNewMem( nNodeTotal * sizeof(nuULONG) ))
		{
			nuFclose(fp);
			return nuFALSE;
		}
		if( nuFread( m_pNewMem,  nNodeTotal * sizeof(nuULONG), 1, fp ) != 1 )
		{
			FreeNewMem();
			nuFclose(fp);
			return nuFALSE;
		}
		nuMemcpy( &m_NowNodeInfo, &NodeInfo, sizeof(NodeInfo) );
		nuFclose(fp);
	}

	nuULONG *pNewMEM =(nuULONG *) m_pNewMem;
	if ( pNewMEM == NULL || m_nNodeTotal <= 0 )
	{
		return nuFALSE;
	}

	//二分法排序...
	/*nuULONG tmp;
	int i,j,k,low,high,mid;
	for( i = k = 1; i < nNodeTotal; i++)
	{ 
		low = 0; 
		high = k-1; 
		while( low <= high)
		{ 
			mid = (low + high) / 2; 
			if(  *(pNewMEM + mid) >=  *(pNewMEM + i) )
			{
				high = mid - 1;
			}
			else
			{
				low = mid + 1;
			}
		} 
		if( high < i ||   *(pNewMEM + low) !=  *(pNewMEM + i) )
		{
			tmp = *(pNewMEM + i);
			for( j = k-1; j >= low; j-- )
			{
				*( pNewMEM + j + 1 ) = *( pNewMEM + j );
			}
			*(pNewMEM + low ) = tmp;
			k++;
		}
	}*/
	
   //开始查找Node...方法：二分法查找
   nuLONG nLow = 0, nHigh = m_nNodeTotal - 1 ,nMid = ( nLow + nHigh ) / 2;
   nuBOOL bFind = nuFALSE;
   nuFILE *fp = NULL;
   while( nLow <= nHigh )
   {
	   nuLONG nMidValue = 0;
	   nMidValue = *( pNewMEM + nMid );
	   if (  nMidValue == NodeInfo.nNodeID )
	   {
		   if ( !fp )
		   {
			   fp = nuTfopen( sFIleName, NURO_FS_RB );
		   }
		   
		   if ( !fp)
		   {
			   return nuFALSE;
		   }
		   if (0 != nuFseek( fp, 
			             sizeof(nuULONG) + sizeof(nuULONG) * m_nNodeTotal + sizeof(nuULONG) *nMid,
						 NURO_SEEK_SET ) )
		   {
			   break;
		   }
		   if ( nuFread( &nNameAddr, sizeof(nuULONG), 1, fp ) != 1 )
		   {
			   break;
		   }
		   if ( nMid == m_nNodeTotal - 1 )
		   {
			   nNameLen = nuFgetlen( fp ) - nNameAddr;
		   }
		   else
		   {
				nuLONG nNextNameAddr = 0;
				if ( nuFread( &nNextNameAddr, sizeof(nuULONG), 1, fp ) != 1 )
				{
					break;
				}
				nNameLen = nNextNameAddr - nNameAddr;
		   }
		   bFind = nuTRUE;
		   break;
	   }
	   else if ( nMidValue > NodeInfo.nNodeID )
	   {
			nHigh = nMid - 1;
	   }
	   else
	   {
		   nLow = nMid + 1;
	   }
	   nMid = (nLow + nHigh) / 2;

   }
   if ( fp )
   {
	   nuFclose(fp);
   }
   if ( NodeInfo.nNodeIndex >= m_nNodeTotal - 1  )
   {
		FreeNewMem();
   }
	 
   return bFind;
}

nuBOOL CMWP::GetNodeName(nuTCHAR *sFIleName, nuULONG nNameAdd, nuULONG nNameLen, nuCHAR *sName, nuULONG nOutNameLen)
{
	nuFILE *fp = nuTfopen(sFIleName, NURO_FS_RB);
	if ( !fp )
	{
		return nuFALSE;
	}
	if ( nNameLen >= nOutNameLen )
	{
		nuFclose(fp);
		return nuFALSE;
	}
	if (0 != nuFseek( fp, nNameAdd, NURO_SEEK_SET) )
	{
		nuFclose(fp);
		return nuFALSE;
	}
	nuMemset( sName, 0, nOutNameLen );
	if( nuFread( sName, nNameLen, 1, fp ) != 1 )
	{
		nuFclose(fp);
		return nuFALSE;
	}
	nuFclose(fp);
	return nuTRUE;
}

//DEL nuBOOL CMWP::SetRTBNum(nuLONG nRTBNum)
//DEL {
//DEL 	if ( nRTBNum <= 0 )
//DEL 	{
//DEL 		return nuFALSE;
//DEL 	}
//DEL 	m_nRTBNum = nRTBNum;
//DEL 	return nuTRUE;
//DEL }

nuBOOL CMWP::AllocNewMem(nuULONG nMemOfByte)
{
	if ( m_pNewMem )
	{
		FreeNewMem();
	}
	m_pNewMem = new nuBYTE[nMemOfByte];
	if ( m_pNewMem == NULL )
	{
		return nuFALSE;
	}
	m_nNodeTotal = nMemOfByte / sizeof(nuLONG);
	return nuTRUE;
}

nuBOOL CMWP::FreeNewMem()
{
	if ( m_pNewMem == NULL )
	{
		 return nuFALSE;
	}
	delete []m_pNewMem;
	m_pNewMem = NULL;
	m_nNodeTotal = 0;
	return nuTRUE;
}
