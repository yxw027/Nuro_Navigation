// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/05/20 07:32:16 $
// $Revision: 1.6 $
#include "stdAfxRoute.h"
//#include "NuroClib.h"
//#include "libInterface.h"
#include "RouteBase.h"
#include "Ctrl_ND6.h"

Class_ND6Ctrl::Class_ND6Ctrl()
{
	InitClass();
}

Class_ND6Ctrl::~Class_ND6Ctrl()
{
	CloseClass();
}

nuBOOL Class_ND6Ctrl::InitClass()
{
	DataCnt    = 0;
	ppND6Data  = NULL;
	m_pND6File = NULL;
	nuMemset(Reserve, 0, sizeof(nuBYTE) * _RESERVESIZE);
	nuMemset(m_tsFName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID Class_ND6Ctrl::CloseClass()
{
	Release();
}

nuVOID Class_ND6Ctrl::Release()
{
	if(m_pND6File != NULL)
	{
		nuFclose(m_pND6File);
		m_pND6File = NULL;
	}
	if(ppND6Data != NULL)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)ppND6Data);
		ppND6Data = NULL;
	}
	DataCnt    = 0;
	nuMemset(Reserve, 0, sizeof(nuBYTE) * _RESERVESIZE);
	nuMemset(m_tsFName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
}

nuBOOL Class_ND6Ctrl::ReadFile()
{
	if(m_pND6File != NULL)
	{
		return nuFALSE;
	}
	m_pND6File = nuTfopen(m_tsFName, NURO_FS_RB);
	if(m_pND6File == NULL)
	{
		return nuFALSE;
	}
	if(1 != nuFread(&DataCnt,sizeof(nuLONG),1,m_pND6File))
	{	
		Release();	
		return nuFALSE;	
	}
	if(DataCnt <= 0)
	{
		Release();	
		return nuFALSE;
	}
	if(_RESERVESIZE != nuFread(&Reserve,sizeof(nuBYTE), _RESERVESIZE, m_pND6File))
	{
		Release();	
		return nuFALSE;	
	}
	ppND6Data = (ND6_Data_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(ND6_Data_Stru) * DataCnt);
	if(ppND6Data == NULL)
	{
		Release();	
		return nuFALSE;
	}
	nuMemset((*ppND6Data), 0, sizeof(ND6_Data_Stru) * DataCnt);
	if(DataCnt != nuFread(*ppND6Data, sizeof(ND6_Data_Stru), DataCnt, m_pND6File))
	{
		Release();	
		return nuFALSE;
	}
	nuFclose(m_pND6File);
	m_pND6File = NULL;
	return nuTRUE;
}


nuBOOL Class_ND6Ctrl::MappingFile(nuDWORD RtbID, nuDWORD NodeID, NUROPOINT Coor)
{
	nuLONG MaxID = DataCnt;
	nuLONG MinID = 0, MidID = 0;
	/*while(nuTRUE)
	{
		MidID = (MaxID + MinID) / 2;
		if(RtbID > (*ppND6Data)[MidID].RtBID)
		{
			MinID = MidID;
		}
		else if(RtbID < (*ppND6Data)[MidID].RtBID)
		{
			MaxID = MidID;
		}
		else if((RtbID == (*ppND6Data)[MidID].RtBID))
		{
			if(NodeID > (*ppND6Data)[MidID].NODEID)		
			{
				MinID = MidID;	
			}
			else if(NodeID < (*ppND6Data)[MidID].NODEID)
			{
				MaxID = MidID;	
			}
			else if(NodeID == (*ppND6Data)[MidID].NODEID)
			{
				if(Coor.x == (*ppND6Data)[MidID].NodeCoor.x && 
					Coor.y == (*ppND6Data)[MidID].NodeCoor.y)
				{
					return nuTRUE;
				}
				else
				{
					return nuFALSE;
				}
			}
		}
		else if( (MaxID - MinID) <= 1)
		{
			break;
		}
	}*/
	
	while(MinID <= MaxID)
	{
		MidID = (MaxID + MinID) / 2;
		if(RtbID > (*ppND6Data)[MidID].RtBID)
		{
			MinID = MidID + 1;
		}
		else if(RtbID < (*ppND6Data)[MidID].RtBID)
		{
			MaxID = MidID - 1;
		}
		else if((RtbID == (*ppND6Data)[MidID].RtBID))
		{
			if(Coor.x > (*ppND6Data)[MidID].NodeCoor.x)		
			{
				MinID = MidID + 1;	
			}
			else if(Coor.x < (*ppND6Data)[MidID].NodeCoor.x)
			{
				MaxID = MidID - 1;	
			}
			else if(Coor.x == (*ppND6Data)[MidID].NodeCoor.x)
			{
				if(Coor.y > (*ppND6Data)[MidID].NodeCoor.y)
				{
					MinID = MidID + 1;
				}
				else if(Coor.y < (*ppND6Data)[MidID].NodeCoor.y)
				{
					MaxID = MidID - 1;
				}
				else if((Coor.y == (*ppND6Data)[MidID].NodeCoor.y))
				{
					return nuTRUE;
				}
			}
		}
		/*if( (MaxID - MinID) <= 1)
		{
			return nuFALSE;
		}*/
	}
	return nuFALSE;
}