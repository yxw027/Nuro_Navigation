// DrawBlock.h: interface for the CDrawBlock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWBLOCK_H__ECB02B04_86B5_4BC7_9551_EA4BD4EAB337__INCLUDED_)
#define AFX_DRAWBLOCK_H__ECB02B04_86B5_4BC7_9551_EA4BD4EAB337__INCLUDED_

#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "NuroMapFormat.h"
#include "NuroEngineStructs.h"
#include "AreaCollect.h"
#include "LineCollect.h"

#define SPCOORBUFSIZE							200

typedef struct myPoint
{
	double x;
	double y;
}MYPOINT,*PMYPOINT;

class CDrawBlock  
{
public:
	CDrawBlock();
	virtual ~CDrawBlock();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL LoadScreenMap();
	nuBOOL CollectScreenMap();
	nuVOID FreeScreenMap();
	nuBOOL CollectScreenPoi();

	//for DTItest load DTI data;add by chang;
	nuBOOL SetDTIColor(nuUINT trafficevent,nuVOID* pRSet = NULL);
	//for DTItest draw DTI road;add by chang;
	nuBOOL DrawDTIRoad(nuBOOL bRimLine = nuTRUE, nuBOOL bDrawRdName = nuTRUE);

	nuBOOL DrawRoad(nuBOOL bRimLine = nuTRUE, nuBOOL bDrawRdName = nuTRUE);
	nuBOOL DrawBgLine();
	nuBOOL DrawBgArea();

	nuBOOL DrawZoomRoad();

	nuBOOL DrawZoomFake();

	nuBOOL SeekForRoad(nuLONG x, nuLONG y, nuLONG nCoverDis, PSEEKEDROADDATA pRoadData);
	nuBOOL CarToRoad(nuLONG x, nuLONG y, nuLONG nCoverDis, nuLONG nCarAngle, PSEEKEDROADDATA pRoadData);

	//add by chang;
	bool GetPointOfVerticalLine(MYPOINT pt1,MYPOINT pt2,MYPOINT *ptres1,MYPOINT *ptres2);
	bool NewGetPointOfVerticalLine(MYPOINT pt1,MYPOINT pt2,MYPOINT *ptres1,MYPOINT *ptres2);
	bool GetPointOfSameSide(MYPOINT pt1,MYPOINT pt2,MYPOINT pt3,MYPOINT *ptres1,MYPOINT *ptres2);
	double mult(MYPOINT a, MYPOINT b, MYPOINT c);
	bool intersect(MYPOINT aa, MYPOINT bb, MYPOINT cc, MYPOINT dd);
	double Inner_cos_of_vectors(MYPOINT pt1,MYPOINT pt2,MYPOINT pt3);
	bool IsEqual(double a, double b, double absError, double relError );
	double  m_Dis;
	double  m_absError;
	MYPOINT m_PointArray[SPCOORBUFSIZE];
	nuroPOINT m_SPointCoorBuf[SPCOORBUFSIZE];

protected:
	PSCREENMAP	m_pScreenMap;
	class CAreaCollect	m_bgAreaCol;
	class CLineCollect	m_roadCol;
	class CLineCollect	m_bgLineCol;
//	class CLineCollect	m_zoomRoadCol;
};

#endif // !defined(AFX_DRAWBLOCK_H__ECB02B04_86B5_4BC7_9551_EA4BD4EAB337__INCLUDED_)
