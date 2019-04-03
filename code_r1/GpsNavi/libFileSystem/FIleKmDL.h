// FilKwS.h: interface for the CFileKwS class.

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

#if !defined(AFX_KMDL_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_)
#define AFX_KMDL_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_

#include "FileMapBaseZ.h"
#include "nuRouteData.h"
typedef struct Strcture_KM_Head
{
	nuLONG KMCount;
}KM_Head;

/*typedef struct Strcture_KMInfoData
{
	nuLONG  lBlockIdx;
	nuLONG  lRoadIdx;
	nuLONG  X;
	nuLONG  Y;
	nuBYTE  nClass;
	nuBYTE  nOneWay;
	nuWORD  KmName;
}KMInfoData;*/

class CFileKMDL : public CFileMapBaseZ
{
public:
	CFileKMDL();
	virtual ~CFileKMDL();

	nuBOOL ReadKM();
	nuBOOL ColKMBlockIndex();
	nuBOOL Initial();
	KMInfoData* GetKMDataDL(nuLONG BlockIdx, /*output*/ nuLONG &Counter);
	KMInfoData* GetKMDataMapping(const SEEKEDROADDATA &RoadData, nuLONG &Counter);

	nuBOOL SeekKMBlock(const nuLONG &lBlockIdx, nuLONG &Index);
	nuBOOL SeekKMRoad(const nuLONG &lBlockIdx, const nuLONG &lRoadIdx, nuLONG &Index);
	nuBOOL SeekKMData(const NUROPOINT &ptCar, const nuLONG &lBlockIdx, const nuLONG &lRoadIdx, 
		const nuLONG &RoadClass, nuWCHAR *wsKilometer, nuLONG &lKMNum, nuLONG &lKMDis, nuWCHAR *wsKMName);

	nuBOOL CheckKMInfo(PKMINFO pKMInfo, nuWCHAR *wsKilometer, nuLONG &lKMNum, nuLONG &lKMDis, nuWCHAR *wsKMName);

	nuVOID CloseFile();
	nuBOOL OpenFile();
	nuVOID Free();

	KMInfoData  *m_pKmInfoData;
	nuLONG		m_lKMCount;
	nuSHORT		*m_pBlockIdx;
	
protected:
	nuFILE*		m_pFile;

};

#endif// !defined(AFX_KMDL_H__3AE18899_131B_45C7_88D9_B975ECA49449__INCLUDED_)