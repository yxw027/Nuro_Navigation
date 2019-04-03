#include "Ctrl_ECI.h"
#include "RouteBase.h"
CCtrl_ECI::CCtrl_ECI(void)
{
	InitConst();
}

CCtrl_ECI::~CCtrl_ECI(void)
{
	CloseClass();
}

nuBOOL  CCtrl_ECI::InitClass()
{
	nuMemset(ECIFName, NULL, sizeof(nuTCHAR) * NURO_MAX_PATH);
	m_pECIFile  = NULL;
	m_ppECIdata = NULL;
	return nuTRUE;
}
nuVOID CCtrl_ECI::CloseClass()
{
	Free();
}
nuVOID CCtrl_ECI::InitConst()
{
	nuMemset(ECIFName, NULL, sizeof(nuTCHAR) * NURO_MAX_PATH);
	nuMemset(&m_ECIHeader,  0, sizeof(ECI_Header));
	m_pECIFile  = NULL;
	m_ppECIdata = NULL;
	m_ppECINode = NULL;
}
nuVOID CCtrl_ECI::Free()
{
	if(NULL != m_pECIFile)
	{
		nuFclose(m_pECIFile);
	}
	if(NULL != m_ppECIdata)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)m_ppECIdata);
		m_ppECIdata = NULL;
	}
	if(NULL != m_ppECINode)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)m_ppECINode);
		m_ppECINode = NULL;
	}
	nuMemset(&m_ECIHeader,  0, sizeof(ECI_Header));
}
nuBOOL CCtrl_ECI::ReadFile()
{
	m_pECIFile  = nuTfopen(ECIFName, NURO_FS_RB);
	if(m_pECIFile == NULL)
	{
		return nuFALSE;
	}
	if(1 != nuFread(&m_ECIHeader, sizeof(ECI_Header), 1, m_pECIFile))
	{
		Free();
		return nuFALSE;
	}
	if(0 >= m_ECIHeader.ECITotoalCount)
	{
		Free();
		return nuFALSE;
	}
	m_ppECINode = (tag_ECINode**)g_pRtMMApi->MM_AllocMem(sizeof(tag_ECINode) * m_ECIHeader.MaxNodeIDCount);
	if(NULL == m_ppECINode)
	{
		Free();
		return nuFALSE;
	}
	m_ppECIdata = (tag_ECI_Data**)g_pRtMMApi->MM_AllocMem(sizeof(tag_ECI_Data) * m_ECIHeader.ECITotoalCount);
	if(NULL == m_ppECIdata)
	{
		Free();
		return nuFALSE;
	}
	if(m_ECIHeader.ECITotoalCount != nuFread(*m_ppECIdata, sizeof(ECI_Data), m_ECIHeader.ECITotoalCount, m_pECIFile))
	{
		Free();
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL CCtrl_ECI::CheckECI(nuLONG RTB, nuDWORD *pNodeIDList, nuULONG &NodeCount, nuLONG &ALPReadID, nuLONG NSSNodeCount)
{
	nuINT nMin = 0, nMid = 0, i = 0, j = 0; 
    nuINT nMax = m_ECIHeader.ECITotoalCount - 1; 
	nuBOOL bFind = nuFALSE ;
	NodeCount = 0;
    while(nMin <= nMax) 
	{ 
        nMid = (nMin+nMax) / 2; 
        if((*m_ppECIdata)[nMid].SRtBID < RTB) 
		{
            nMin = nMid+1; 
		}
		else if((*m_ppECIdata)[nMid].SRtBID > RTB) 
		{
            nMax = nMid - 1; 
		}
		else 
		{
			if((*m_ppECIdata)[nMid].SNodeID < pNodeIDList[0]) 
			{
				nMin = nMid+1; 
			}
			else if((*m_ppECIdata)[nMid].SNodeID > pNodeIDList[0]) 
			{
				nMax = nMid - 1; 
			}
			else
			{
				bFind = nuTRUE;
				break;
			}
		}
	}
	if(!bFind)
	{
		return nuFALSE;
	}
	else
	{
		bFind = nuFALSE;
	}
	for(i = nMid; i >= 0; i--)
	{
		if((*m_ppECIdata)[i].SRtBID != RTB ||
			(*m_ppECIdata)[i].SNodeID != pNodeIDList[0])
		{
			break;
		}
		nMid = i;
	}
	for(j = nMid; j < m_ECIHeader.ECITotoalCount; j++)
	{
		if((*m_ppECIdata)[j].SRtBID != RTB)
		{
			break;
		}
		if((*m_ppECIdata)[j].SRtBID == RTB
			&& (*m_ppECIdata)[j].SNodeID == pNodeIDList[0]
			&& (*m_ppECIdata)[j].PNodeID == pNodeIDList[1])
		{
			nuMemset(*m_ppECINode, 0, sizeof(ECINode));
			if(NULL !=nuFseek(m_pECIFile, (*m_ppECIdata)[j].B_Address, NURO_SEEK_SET))
			{
				return nuFALSE;
			}
			if((*m_ppECIdata)[j].nNodeCount != 
				nuFread(*m_ppECINode, sizeof(ECINode), (*m_ppECIdata)[j].nNodeCount, m_pECIFile))
			{
				return nuFALSE;
			}
			for(i = 0; i < (*m_ppECIdata)[j].nNodeCount; i++)
			{
				if(i + 2 >= NSSNodeCount)
				{
					break;
				}
				else if((*m_ppECINode)[i].NodeID != pNodeIDList[i + 2])
				{
					break;
				}
				else if(i == (*m_ppECIdata)[j].nNodeCount - 1)
				{
					ALPReadID  = (*m_ppECIdata)[j].ALPReadID;
					NodeCount = (*m_ppECIdata)[j].nNodeCount;
					return nuTRUE;
				}
			}
		}		
	}
	return bFind;
}