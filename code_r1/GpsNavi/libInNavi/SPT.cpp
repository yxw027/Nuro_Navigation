// SPT.cpp: implementation of the CSPT class.
//
//////////////////////////////////////////////////////////////////////

#include "SPT.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSPT::CSPT()
{
	m_pNewMem = NULL;
	m_nNodeTotal = 0;
	m_nSignpostDirTotal = 0;
	nuMemset( &m_NowNodeInfo, 0, sizeof(m_NowNodeInfo) );
}

CSPT::~CSPT()
{
	FreeNewMem();
}

nuBOOL CSPT::GetNodeNameAddr( nuTCHAR *sFIleName, SPTNODELISTINFO NodePreInfo, SPTNODELISTINFO NodeNextInfo, 
							  nuULONG &nNameAddr, nuULONG &nNameLen )
{
	//首先检测内存指针是否为空或者RTB发生改变，若为空则根据读取文件的内容分配内存
	if( m_pNewMem == NULL || NodePreInfo.nFileIndex != m_NowNodeInfo.nFileIndex )
	{
		if( m_pNewMem != NULL )
		{
			FreeNewMem();
		}
		nuFILE *fp = nuTfopen(sFIleName, NURO_FS_RB);
		if( !fp )
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
		nuLONG nAllocLen = nNodeTotal * (sizeof(NODEID) + sizeof(nuBYTE)) ;
		m_nNodeTotal = nNodeTotal;
		if ( !AllocNewMem( nAllocLen))
		{
			nuFclose(fp);
			return nuFALSE;
		}
		 //所有Signpost指向的方向个数
		if( nuFread( &m_nSignpostDirTotal,  sizeof(m_nSignpostDirTotal), 1, fp ) != 1 )
		{
			FreeNewMem();
			nuFclose(fp);
			return nuFALSE;
		}
		if( nuFread( m_pNewMem,  nAllocLen, 1, fp ) != 1 )
		{
			FreeNewMem();
			nuFclose(fp);
			return nuFALSE;
		}
		nuMemcpy( &m_NowNodeInfo, &NodePreInfo, sizeof(NodePreInfo) );
		nuFclose(fp);
	}

	nuUCHAR *pNewMEM = m_pNewMem;
	if ( pNewMEM == NULL || m_nNodeTotal <= 0 )
	{
		return nuFALSE;
	}
	
   //开始查找Node..
    nuBOOL bFind = nuFALSE;
	nuFILE *fp = NULL;
	nuLONG nDirCount = 0;
	for( nuLONG nIndex = 0; nIndex < m_nNodeTotal; nIndex++ )
	{
	   NODEID TmpID = {0};
	   TmpID.nBlockID = *(nuWORD *)( pNewMEM + nIndex * sizeof(NODEID) );
	   TmpID.nRoadID  = *(nuWORD *)( pNewMEM + nIndex * sizeof(NODEID) + sizeof(TmpID.nBlockID) );
	   if( TmpID.nBlockID == 7124 || TmpID.nBlockID == 7221 )
	   {
		   nuBOOL bTest = nuFALSE;
	   }
	   nuBYTE nTmpDirCount = *( pNewMEM + sizeof(NODEID) * m_nNodeTotal +  nIndex );
	   if (  0 == nuMemcmp( &TmpID,  &(NodePreInfo.NodeId), sizeof(TmpID) ) )
	   {
		   if ( !fp )
		   {
			   fp = nuTfopen( sFIleName, NURO_FS_RB );
		   }
		   
		   if ( !fp)
		   {
			   return nuFALSE;
		   }
           for ( long nIndex2 = nDirCount; nIndex2 < nDirCount + nTmpDirCount; nIndex2++ )
           {
			   if ( nuFseek( fp, 
				   sizeof(nuULONG) * 2 + ( sizeof(NODEID)  + sizeof(nuBYTE))* m_nNodeTotal + nIndex2 * sizeof(NODEID),
				   NURO_SEEK_SET ) )
			   {
				   break;
				}
                NODEID TmpIDNext = {0};
			   if( 1 != nuFread( &TmpIDNext, sizeof(TmpIDNext),1,fp) )
			   {
				   break;		
			   }
			   if( 0 == nuMemcmp( &TmpIDNext, &NodeNextInfo.NodeId, sizeof(TmpIDNext)) )
			   {
				   if ( nuFseek( fp, 
					   sizeof(nuULONG) * 2 + ( sizeof(NODEID)  + sizeof(nuBYTE))* m_nNodeTotal + sizeof(NODEID)  * m_nSignpostDirTotal + sizeof(long) *nIndex2,
					   NURO_SEEK_SET ) )
				   {
					   break;
				   }
				   if ( nuFread( &nNameAddr, sizeof(nuULONG), 1, fp ) != 1 )
				   {
					   break;
				   }
				   long nNextNameAddr = 0;
				   if ( nuFread( &nNextNameAddr, sizeof(nuULONG), 1, fp ) != 1 )
				   {
					   break;
				   }
			    	nNameLen = nNextNameAddr - nNameAddr;
					bFind = nuTRUE;
					break;
			   }
           }
		  
	   }
	   
	   if( nuFALSE == bFind)
	   {
		   nDirCount += nTmpDirCount;
	   }
	   else
	   {
		   break;
	   }

   }
   if ( fp )
   {
	   nuFclose(fp);
   }
   return bFind;

}

nuBOOL CSPT::GetNodeName(nuTCHAR *sFIleName, nuULONG nNameAdd, nuULONG nNameLen, nuCHAR *sName, nuULONG nOutNameLen)
{
	nuFILE *fp = nuTfopen( sFIleName, NURO_FS_RB );
	if ( !fp )
	{
		return nuFALSE;
	}
	if ( nNameLen >= nOutNameLen )
	{
		return nuFALSE;
	}
	if ( nuFseek( fp, nNameAdd, NURO_SEEK_SET) )
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

nuBOOL CSPT::AllocNewMem(nuULONG nMemOfByte)
{
	if ( m_pNewMem )
	{
		delete []m_pNewMem;
		m_pNewMem = NULL;
	}
	m_pNewMem = new nuBYTE[nMemOfByte];
	if ( m_pNewMem == NULL )
	{
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CSPT::FreeNewMem()
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
