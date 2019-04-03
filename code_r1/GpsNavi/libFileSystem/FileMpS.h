// FilMpS.h: interface for the CFileKwS class.

///////////////////////////////////////////////////////////////////////
// Purpose:
//       
//     
//     
// Parameters:
//     {The description of all parameters}
// Return value: 
// Exceptions: {The description of all exceptions}
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MP_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_)
#define AFX_MP_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroMapFormat.h"

typedef struct tag_KP_Header_Data		             //.MP Header_DATA
{
	nuDWORD  nPOICount;
} MP_Header;

typedef struct tag_POI_Info		                    //.MP POIInfo
{ 
	nuDWORD POICoorX;
    nuDWORD POICoorY;
    nuDWORD POIRoadInfoAddr;	
} POI_Info;

/*typedef struct tag_POI_Info_Data		               //.MP POI_Info_Data
{ 
	nuWORD  MapIndex;
	nuWORD  Reserve;
	nuDWORD BlockIndex;
	nuDWORD RoadIndex;
	nuDWORD RoadNameAddr;
	nuWORD  CityID;
	nuWORD  TownID; 
	nuLONG    x;
	nuLONG    y;
} POI_Info_Data;*/

class CFileMpS : public CFileMapBaseZ
{
public:
	CFileMpS();
	virtual ~CFileMpS();

	nuBOOL ChangePoiInfo(/*InPut*/NUROPOINT PoiCoor ,/*output*/POI_Info_Data& PoiData);

	nuVOID CloseFile();
	nuBOOL OpenFile();
	nuVOID Free();

	POI_Info       **m_pPoiInfo;

protected:
	nuFILE*		m_pFile;
	nuWORD		m_nMapIdx;
};

#endif// !defined(AFX_MP_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_)