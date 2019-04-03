
#include "SearchBaseZ.h"

PFILESYSAPI	CSearchBaseZ::m_pFsApi	= NULL;
PMEMMGRAPI_SH	CSearchBaseZ::m_pMmApi	= NULL;
nuUINT		CSearchBaseZ::m_nCityTotal = 0;
nuUINT		CSearchBaseZ::m_nCBInfoLen = 0;
nuUINT	CSearchBaseZ::m_nStrokeLimIdx = 0;
NURO_SEARCHINFO	CSearchBaseZ::m_searchInfo;

CSearchBaseZ::CSearchBaseZ()
{
	m_bRun = nuFALSE;
}

CSearchBaseZ::~CSearchBaseZ()
{
}
nuBOOL CSearchBaseZ::IsRun()
{
	return m_bRun;
}
nuBOOL CSearchBaseZ::InitData(PAPISTRUCTADDR pApiAddr)
{
	m_pFsApi	= pApiAddr->pFsApi;
	m_pMmApi	= pApiAddr->pMm_SHApi;
	return nuTRUE;
}
nuVOID CSearchBaseZ::FreeData()
{
}
nuBOOL CSearchBaseZ::TranslateSS(const NURO_SEARCH_TYPE &type
										, const nuUINT &nMax
										, const nuUINT &lastPos
										, const nuUINT &lastCount
										, nuUINT *pTranResTagPos
										, nuUINT *pTranResMaxNum)
{
	switch(type)
	{
	case NURO_SEARCH_ALL:
	case NURO_SEARCH_NEW:
		*pTranResTagPos = 0;
		*pTranResMaxNum = nMax;
		break;
	case NURO_SEARCH_NEXT:
		*pTranResTagPos = lastPos;
		*pTranResMaxNum = nMax;
		break;
	case NURO_SEARCH_PRE:
		if (!lastPos || !lastCount || lastPos<=lastCount)
		{
			return nuFALSE;
		}
		*pTranResTagPos = lastPos - lastCount;
		*pTranResTagPos = (*pTranResTagPos<=nMax)?0:(*pTranResTagPos-nMax);
		*pTranResMaxNum = nMax;
		break;
	case NURO_SEARCH_RELOAD:
		if (!lastPos || !lastCount || lastPos<lastCount || nMax<lastCount)
		{
			return nuFALSE;
		}
		else
		{
			*pTranResTagPos = lastPos - lastCount;
			*pTranResMaxNum = lastCount;
		}
		break;
	default:
		return nuFALSE;
	}
	return nuTRUE;
}

nuBOOL CSearchBaseZ::QueueBUffer(SEARCH_BUFFER *pBuffer, nuUINT max, nuBYTE type)
{
	if (!max || (nuUINT)-1==max)
	{
		return nuFALSE;
	}
	SEARCH_BUFFER temp = {0};
	nuUINT tag2 = 0;
	nuUINT last = 0;
	nuUINT times = max - 1; 
	nuUINT i = 0;
	nuUINT j = 0;
	switch(type)
	{
	case 1:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].cityID<=pBuffer[i].cityID)
				{
					tag2 = i;
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 2:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].strokes<pBuffer[i].strokes)
				{
					tag2 = i;
				}
				else if (pBuffer[tag2].strokes==pBuffer[i].strokes)
				{					
					if (nuWcscmp(pBuffer[tag2].name, pBuffer[i].name)<=0)
					//if (pBuffer[tag2].nameAddr<=pBuffer[i].nameAddr)
					{
						tag2 = i;
					}
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 3:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].nameAddr<=pBuffer[i].nameAddr)
				{
					tag2 = i;
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 4:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].cityID<pBuffer[i].cityID)
				{
					tag2 = i;
				}
				else if (pBuffer[tag2].cityID==pBuffer[i].cityID)
				{
					if(pBuffer[tag2].townID<pBuffer[i].townID)
					{
						tag2 = i;
					}
					else if (pBuffer[tag2].townID==pBuffer[i].townID)
					{
						if(pBuffer[tag2].strokes<pBuffer[i].strokes)
						{
							tag2 = i;
						}
						else if (pBuffer[tag2].strokes==pBuffer[i].strokes)
						{
							if (nuWcscmp(pBuffer[tag2].name, pBuffer[i].name)<=0)
							//if (pBuffer[tag2].nameAddr<=pBuffer[i].nameAddr)
							{
								tag2 = i;
							}
						}
					}
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 5:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
 				if( pBuffer[tag2].nFindCount > pBuffer[i].nFindCount)
 				{
 					tag2 = i;
 				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 6:  // Dis
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].nNearDis < pBuffer[i].nNearDis)
				{
					tag2 = i;
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 7:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{				
				if (pBuffer[tag2].nNearDis < pBuffer[i].nNearDis)
				{
					tag2 = i;
				}
				else if( pBuffer[tag2].nNearDis == pBuffer[i].nNearDis)
				{
					if(pBuffer[tag2].strokes<pBuffer[i].strokes)
					{
						tag2 = i;
					}
					else if (pBuffer[tag2].strokes==pBuffer[i].strokes)
					{					
						if (nuWcscmp(pBuffer[tag2].name, pBuffer[i].name)<=0)
							//if (pBuffer[tag2].nameAddr<=pBuffer[i].nameAddr)
						{
							tag2 = i;
						}
					}				
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 8:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if (pBuffer[tag2].nNearDis < pBuffer[i].nNearDis)
				{
					tag2 = i;
				}
				else if( pBuffer[tag2].nNearDis == pBuffer[i].nNearDis)
				{
					if( pBuffer[tag2].nFindCount > pBuffer[i].nFindCount)
					{
						tag2 = i;
					}
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	default:
	    break;
	}
	return nuFALSE;
}

nuBOOL CSearchBaseZ::QueueBUfferRoad(SEARCH_BUFFER_ROAD *pBuffer, nuUINT max, nuBYTE type)
{
	if (!max || (nuUINT)-1==max)
	{
		return nuFALSE;
	}
	SEARCH_BUFFER_ROAD temp = {0};
	nuUINT tag2 = 0;
	nuUINT last = 0;
	nuUINT times = max - 1; 
	nuUINT i = 0;
	nuUINT j = 0;
	switch(type)
	{
	case 1:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].cityID<=pBuffer[i].cityID)
				{
					tag2 = i;
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 2:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].strokes<pBuffer[i].strokes)
				{
					tag2 = i;
				}
				else if (pBuffer[tag2].strokes==pBuffer[i].strokes)
				{
					if (pBuffer[tag2].nameAddr<=pBuffer[i].nameAddr)
					{
						tag2 = i;
					}
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 3:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].nameAddr<=pBuffer[i].nameAddr)
				{
					tag2 = i;
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	case 4:
		for (j=0; j<times; j++)
		{
			tag2 = 0;
			last = times-j;
			for(i=1;i<=last;i++)
			{
				if(pBuffer[tag2].cityID<pBuffer[i].cityID)
				{
					tag2 = i;
				}
				else if (pBuffer[tag2].cityID==pBuffer[i].cityID)
				{
					if(pBuffer[tag2].townID<pBuffer[i].townID)
					{
						tag2 = i;
					}
					else if (pBuffer[tag2].townID==pBuffer[i].townID)
					{
						if(pBuffer[tag2].strokes<pBuffer[i].strokes)
						{
							tag2 = i;
						}
						else if (pBuffer[tag2].strokes==pBuffer[i].strokes)
						{
							if (pBuffer[tag2].nameAddr<=pBuffer[i].nameAddr)
							{
								tag2 = i;
							}
						}
					}
				}
			}
			if (tag2!=last)
			{
				temp = pBuffer[last];
				pBuffer[last] = pBuffer[tag2];
				pBuffer[tag2] = temp;
			}
		}
		return nuTRUE;
	default:
	    break;
	}
	return nuFALSE;
}