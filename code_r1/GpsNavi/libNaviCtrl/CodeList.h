// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: CodeList.h,v 1.2 2009/08/28 03:26:54 wuzhikun Exp $
// $Author: wuzhikun $
// $Date: 2009/08/28 03:26:54 $
// $Locker:  $
// $Revision: 1.2 $
// $Source: /home/nuCode/libNaviCtrl/CodeList.h,v $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODELIST_H__168455D8_9D43_421F_BECE_6A5149248537__INCLUDED_)
#define AFX_CODELIST_H__168455D8_9D43_421F_BECE_6A5149248537__INCLUDED_

#include "NuroDefine.h"

#define POP_ATHEAD						0x00

#define PUSH_ADD_CIRCLE					0x00//if too many, rewrite the first node
#define PUSH_ADD_ATEND					0x01//add a node at the end
#define PUSH_ADD_ATHEAD					0x02//add a node at the head
#define PUSH_ADD_DELOTHERS				0x03//delete other nodes and add a new node
#define PUSH_ADD_ISEMPTY				0x04//if the list is empty, add a node

#define	LISTCOUNT						16
//--- tagCODENODE::nDrawType #define --------------------
#define DRAWTYPE_UI						0x01
#define DRAWTYPE_GPS					0x02
#define DRAWTYPE_SIMULATION				0x03
#define DRAWTYPE_TIMER					0x04
#define DRAWTYPE_NAVIGATION				0x05
//--- tagCODENODE::nActionType #define-------------------
//For Draw, Pre and Navi List
#define AC_DRAW_NOACTION				0x0000
#define AC_DRAW_ONPAINT					0x0001
#define AC_DRAW_REDRAWINFO				0x0002
#define AC_DRAW_REDRAWMAP				0x0003
#define AC_DRAW_DRAWINFODLG				0x0004
#define AC_DRAW_SHOWMENU				0x0005
#define AC_DRAW_HIDEMENU				0x0006
#define AC_DRAW_PAUSETHREAD				0x0007
#define AC_DRAW_RESUMETHREAD			0x0008
#define AC_DRAW_GETROUTELIST			0x0009
#define AC_DRAW_GETMAPINFO				0x000A
#define AC_DRAW_SETNAVITHD				0x000B
#define AC_DRAW_TRACEACTION				0x000C

#define AC_DRAW_POWERON					0x0010
#define	AC_DRAW_JUMPTOMAPXY				0x0011
#define AC_DRAW_JUMPTOSCREENXY			0x0012
#define AC_DRAW_JUMPDISTOCENTER			0x0013
#define AC_DRAW_DRAGMAP					0x0014
#define AC_DRAW_RESETSCREEN				0x0015
#define AC_DRAW_JUMPTOGPS				0x0016
#define AC_DRAW_ROTATEMAP				0x0026
#define AC_DRAW_SETUSERCFG				0x0027
	#define SETUSERCFG_NOEXTEND				0x0000
	#define SETUSERCFG_ZOOMIN				0x0001
	#define SETUSERCFG_ZOOMOUT				0x0002
	#define SETUSERCFG_COVERFROMDFT			0x0004				
#define AC_DRAW_BACKTOCAR				0x0028
#define AC_DRAW_MSGBOX					0x0029
#define AC_DRAW_SHOWHIDEBUT				0x002A
#define AC_DRAW_SPLITSCREEN				0x002B
#define AC_DRAW_SETDRAWINFO				0x002C
#define AC_DRAW_SETSCALE				0x002D

#define AC_DRAW_MOUSEEVENT				0x0030//For judge the Mouse button selected.
#define AC_DRAW_KEYEVENT				0x0038
#define AC_DRAW_OPERATEMAPXY				0x0040
#define AC_DRAW_SETVOICE				0x0050

#define AC_DRAW_POINTOTROAD				0x0060
#define AC_DRAW_UI_SAVE_LAST_POSITION			0x0061
#define AC_DRAW_SETMAPPT                		0x0062//For Set MapPT File
#define AC_DRAW_TMC_DISPLAY_MAP				0x0063
#define AC_DRAW_TMC_NAVI_LINE_TRAFFIC_EVENT_DISPLAY 	0x0064
#define AC_DRAW_TMC_FLUSH_MAP				0x0065

#define SETMAPPT_NORMAL					0x0000
#define SETMAPPT_COVERFROMDFT				0x0001	

#define AC_NAVI_STARTROUTE				0x1001
	
#define AC_NAVI_STOPROUTE				0x1002
#define AC_NAVI_REROUTE					0x1003
#define AC_NAVI_SETNAVIMODE				0x1006//Simulation or GPS
#define AC_NAVI_PAUSE					0x1007
#define AC_NAVI_CONTINUE				0x1008
#define AC_NAVI_NEAREND					0x1009
#define AC_NAVI_IFSTOP					0x100A
#define AC_NAVI_SETMARKETPT				0x1010
#define AC_NAVI_DELMARKETPT				0x1011
#define AC_NAVI_CONFIGROUTE				0x1012
#define AC_NAVI_TMC_ROUTE				0x1013//added by daniel 20110830

#define AC_GPSSET_TOBERESETGYRO         0x1030 // Added by Damon 20110906
#define AC_GPSSET_TOBEGETVERSION        0x1031 // Added by Louis 20111110
#define AC_NAVI_RTFULLVIEW              0x1032 // Added by Xiaoqin 20120517
#define AC_GPSSET_RESET                 0x1033 // Added by Damon 20120705

#define AC_DRAW_GPSGETDATA				0x200F//added by daniel
#define AC_DRAW_DAIWOO_BT				0x2010//added by daniel
#define AC_DRAW_NEARPOI					0x2011
#define AC_DRAW_JUMPROAD				0x2012 //added by Prosper , Jump Road
#define AC_DRAW_QIUT					0x2013
#define AC_DRAW_READINITIALDATA			0x2014
#define AC_DRAW_JUMPTOSCREENXY2			0x2015
#define AC_DRAW_OPERATEMAPXY2			0x2016
#define AC_DRAW_JUMPTOMAPXY2			0x2017
#define AC_DRAW_CHOSE_ROAD				0x2018
//========For SDK========================//
#define AC_DRAW_SET_USER_DATA			0x2019
#define AC_DRAW_GETROUTELIST_D			0x2020
#define AC_DRAW_GET_TMC_NAVI_LIST_D		0x2021
//========For SDK========================//
//-------------------------------------------------------
typedef struct tagCODENODE
{
	nuBYTE	nDrawType;
	nuWORD	nActionType;
	nuLONG	nX;
	nuLONG	nY;
	nuLONG	nExtend;
	nuPVOID pDataBuf[6];
}CODENODE, *PCODENODE;

class CCodeList  
{
public:
	CCodeList();
	virtual ~CCodeList();
	nuBOOL  Initialize();
	nuVOID  Free();
	nuBOOL  PushNode(PCODENODE pNode, nuBYTE byPushType = PUSH_ADD_ATEND);
	nuBOOL  PopNode(PCODENODE pNode, nuBYTE byPopType = POP_ATHEAD);

	nuINT   GetNowListCount() { return m_nNowCount; }
	nuVOID  ClearList();

protected:
	nuVOID  CodeCopy(PCODENODE pDesCode, PCODENODE pSrcCode);

protected:
	CODENODE	m_pCodeList[LISTCOUNT];
	nuSHORT		m_nFirstIdx;
	nuSHORT		m_nNowCount;
	nuBOOL		m_bWorking;
};

#endif // !defined(AFX_CODELIST_H__168455D8_9D43_421F_BECE_6A5149248537__INCLUDED_)
