#include "PrevRoadNameInfoList.h"

CPrevRoadNameInfoList g_PrevRoadName;

CPrevRoadNameInfoList::CPrevRoadNameInfoList(nuVOID)
{
	nuMemset(this, 0, sizeof (CPrevRoadNameInfoList));
}

CPrevRoadNameInfoList::~CPrevRoadNameInfoList(nuVOID)
{
}

int CPrevRoadNameInfoList::FindFirstUnused(nuVOID)
{
	int index;

	for (index = 0; index < MAX_COUNT; index++)
	{
		if (!m_PrevNodes[index].bInUse)
			return index;
	}

	return -1;
}

nuBOOL CPrevRoadNameInfoList::ResetExistTag(nuVOID)
{
#if 0
	int index;

	for (index = 0; index < MAX_COUNT; index++)
	{
		m_PrevNodes[index].bExist = nuFALSE;
	}
#else

	int index;

	for (index = 0; index < MAX_COUNT; index++)
	{
		NURORECT rect = m_PrevNodes[index].data.rect;
		
		ConvertRect_MapToBmp(rect);

		if (rect.bottom < 0 || rect.right < 0 || rect.left > 800 || rect.top > 480)
		{
			m_PrevNodes[index].bExist = nuFALSE;
			m_PrevNodes[index].bInUse = nuFALSE;
		}
	}

#endif

	return nuTRUE;
}

nuBOOL CPrevRoadNameInfoList::ResetAll(nuVOID)
{
	nuMemset(m_PrevNodes, 0, sizeof (m_PrevNodes));

	return nuTRUE;
}

nuBOOL CPrevRoadNameInfoList::RemoveNoneExist(nuVOID)
{
#if 0
	for (int index = 0; index < MAX_COUNT; index++)
	{
		if (m_PrevNodes[index].bInUse && !m_PrevNodes[index].bExist)
		{
			m_PrevNodes[index].bInUse = nuFALSE;
		}
	}
#endif

	return nuTRUE;
}

nuBOOL CPrevRoadNameInfoList::RectModify(nuDWORD dwRoadNameAddr, NURORECT& roadNameRect, nuBYTE rdAlign)
{
// 	int index;
// 
// 	for (index = 0; index < MAX_COUNT; index++)
// 	{
// 		if (!m_PrevNodes[index].bInUse)
// 			continue;
// 
// 
// 
// 		if (m_PrevNodes[index].data.rdNameAddr == dwRoadNameAddr)
// 		{
// 			if (924552 == dwRoadNameAddr)
// 			{
// 				//::MessageBox(NULL, TEXT("JIJIJ"), TEXT(""), MB_OK);
// 				NURORECT rect = m_PrevNodes[index].data.rect;
// 				//ConvertRect_MapToBmp(rect);
// 				rect.top = 100;
// 				rect.left = 100;
// 				rect.bottom = 300;
// 				rect.right = 300;
// 
// 				roadNameRect = rect;
// 				return nuFALSE;
// 			}
// 			else
// 			{
// 				NURORECT rect = m_PrevNodes[index].data.rect;
// 				ConvertRect_MapToBmp(rect);
// 				roadNameRect = rect;
// 				return nuFALSE;
// 			}
// 		}
// 	}
// 
// 	index = FindFirstUnused();
// 
// 	if (-1 == index)
// 	{
// 		return nuFALSE;
// 	}
// 
// 	m_PrevNodes[index].bInUse = nuTRUE;
// 	m_PrevNodes[index].bExist = nuTRUE;
// 	m_PrevNodes[index].dwRoadNameAddr = dwRoadNameAddr;
// 	m_PrevNodes[index].roadNameRect = roadNameRect;
// 	m_PrevNodes[index].rdAlign = rdAlign;
// 	
// 	ConvertRect_BmpToMap(m_PrevNodes[index].roadNameRect);

	return nuTRUE;
}

nuBOOL CPrevRoadNameInfoList::RectModify(RDNAMENODE& node)
{
#if 0
	static RDNAMENODE *p = NULL;//new RDNAMENODE;

	if (924552 != node.rdNameAddr)
	{
		return nuTRUE;
	}

	if (!p)
	{
		p = new RDNAMENODE;
		*p = node;
		ConvertRect_					BmpToMap(p->rect);
	}
	else
	{
		node = *p;
		ConvertRect_MapToBmp(node.rect);
	}

	return nuTRUE;
#else
	int index;
	// if (924552 == dwRoadNameAddr)
	for (index = 0; index < MAX_COUNT; index++)
	{
		if (!m_PrevNodes[index].bInUse)
			continue;

		if (m_PrevNodes[index].data.rdNameAddr == node.rdNameAddr)
		{
			RDNAMENODE node_tmp = m_PrevNodes[index].data;
			ConvertRect_MapToBmp(node_tmp.rect);
			m_PrevNodes[index].bExist = nuTRUE;
			node = node_tmp;
// 			if (924552 == node.rdNameAddr)
// 			{
// 				// 12151910
// 				// 2509297
// 				// 12151963
// 				// 2509263 
// 				node = node_tmp;
// 				node.rect.left = 12151910;
// 				node.rect.top = 2509297;
// 				node.rect.right = 12151963;
// 				node.rect.bottom = 2509263;
// 				ConvertRect_MapToBmp(node.rect);
// 				// 188 347 260 399
// 
// 			}
			return nuTRUE;
		}
	}

	index = FindFirstUnused();

	if (-1 == index)
	{
		return nuFALSE;
	}

	m_PrevNodes[index].bInUse = nuTRUE;
	m_PrevNodes[index].bExist = nuTRUE;
	m_PrevNodes[index].data = node;

	GetRoadName(node, m_PrevNodes[index].text);
	ConvertRect_BmpToMap(m_PrevNodes[index].data.rect);
	

	return nuTRUE;
#endif
}

nuBOOL CPrevRoadNameInfoList::ConvertRect_BmpToMap(NURORECT& rect)
{
	BmpToMap(rect.left, rect.top, rect.left, rect.top);
	BmpToMap(rect.right, rect.bottom, rect.right, rect.bottom);

	return nuTRUE;
}

nuBOOL CPrevRoadNameInfoList::ConvertRect_MapToBmp(NURORECT& rect)
{
	MapToBmp(rect.left, rect.top, rect.left, rect.top);
	MapToBmp(rect.right, rect.bottom, rect.right, rect.bottom);

	return nuTRUE;
}

nuBOOL CPrevRoadNameInfoList::ConvertRect_BmpToMap(NURORECT& rect1, const NURORECT& rect2)
{
	BmpToMap(rect2.left, rect2.top, rect1.left, rect1.top);
	BmpToMap(rect2.right, rect2.bottom, rect1.right, rect1.bottom);

	return nuTRUE;
}

nuBOOL CPrevRoadNameInfoList::ConvertRect_MapToBmp(NURORECT& rect1, const NURORECT& rect2)
{
	MapToBmp(rect2.left, rect2.top, rect1.left, rect1.top);
	MapToBmp(rect2.right, rect2.bottom, rect1.right, rect1.bottom);

	return nuTRUE;
}

nuBOOL CPrevRoadNameInfoList::BmpToMap(nuLONG bmpX, nuLONG bmpY, nuLONG& mapX, nuLONG& mapY)
{
	return g_pDMLibMT->MT_BmpToMap(bmpX, bmpY, &mapX, &mapY);
}

nuBOOL CPrevRoadNameInfoList::MapToBmp(nuLONG mapX, nuLONG mapY, nuLONG& bmpX, nuLONG& bmpY)
{
	return g_pDMLibMT->MT_MapToBmp(mapX, mapY, &bmpX, &bmpY);
}

nuBOOL CPrevRoadNameInfoList::GetRoadName(RDNAMENODE& node, nuWCHAR *wsRoadName)
{
	RNEXTEND rnExtend = {0};

	g_pDMLibFS->FS_GetRoadName(node.rdDwIdx, node.rdNameAddr, 
		node.rdType, wsRoadName, RN_MAX_LEN, &rnExtend);
	
		return nuTRUE;
}