// WaypointMgrZ.h: interface for the CWaypointMgrZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAYPOINTMGRZ_H__6646FFEA_5A6F_4798_922B_AD0A24167226__INCLUDED_)
#define AFX_WAYPOINTMGRZ_H__6646FFEA_5A6F_4798_922B_AD0A24167226__INCLUDED_

#include "FileSystemBaseZ.h"


//#define _USE_WAYPOINTS_MGRZ

class CWaypointsMgrZ : public CFileSystemBaseZ
{
public:
	CWaypointsMgrZ();
	virtual ~CWaypointsMgrZ();

	bool Initialize();
	void Free();

	nuUINT SetWaypoints(nuWORD idx, long x, long y, nuWCHAR* pwsName = NULL, short nAngle = -1);
	nuUINT DltWaypoints(nuWORD idx, short nAngle = -1);
	nuUINT GetWaypoints(PPARAM_GETWAYPOINTS pParam);
	nuUINT KptWaypoints(long x, long y);
protected:
	bool ReadWaypointsFile();
	bool AmendWaypointsFile(int nIdx);

public:
	PWAYPOINTS_NODE	m_pWaypointsList;
	nuWORD			m_nMaxCount;
	nuWORD			m_nNowCount;
	nuWORD			m_nMaxFixedDis;
	nuroPOINT		m_ptNextCoor;

};

#endif // !defined(AFX_WAYPOINTMGRZ_H__6646FFEA_5A6F_4798_922B_AD0A24167226__INCLUDED_)
