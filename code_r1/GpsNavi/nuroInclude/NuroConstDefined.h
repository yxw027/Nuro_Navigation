#ifndef __NURO_CONST_DEFINED_20080128
#define __NURO_CONST_DEFINED_20080128

#include "NuroDefine.h"

#define BSD_2K_BLOCK_RANGE						(200*COORCONSTANT)

#define CHECKRANGE								(2000*COORCONSTANT)


#define SCALETOSCREEN_LEN						48


#define POINTLISTCOUNT							2000


#define ANGLE_FIXTOROAD							60
#define ANGLE_NAVIPTTOROAD						90

#define CENTERROAD_SEEK_MAX_LEN					(500*COORCONSTANT)
#define CENTERROAD_SHOW_MAX_LEN					(60*COORCONSTANT)
#define CENTERPOI_SEEK_MAX_LEN					(100*COORCONSTANT)
#define CENTERPOI_SEEK_MAX_DIS					(500*COORCONSTANT)
#define CENTERPOI_SHOW_MAX_DIS					(100*COORCONSTANT)

#define DISTANCE_NEED_ANGLE_INSETMARKET			(500*COORCONSTANT)

//MDW
#define MDW_BLOCK_SIZE							(200*1000*COORCONSTANT)
#define MDW_X_BLOCK_NUM							(180)
//BGL
#define BGL_BLOCK_SIZE_1						(50*1000*COORCONSTANT)
#define BGL_BLOCK_SIZE							(500*1000*COORCONSTANT)
#define BGL_X_BLOCK_NUM							(180)
#define BGL_SCALE_VALUE_START					(5*1000*COORCONSTANT)
#define BGL_SCALE_50K_START						(10*1000*COORCONSTANT)

/**************** NaviCtrl define****************/
//LoadGSPOpen
#define DATATYPE_TABLE			1 //列表型態的資料
#define DATATYPE_ITEM			2 //解析的資料

#define ITEM_UTCTIME			1//GPS_COUNTER,VALID_UTC_TIME,UTC_TIME UTC標準時間
#define ITEM_STATUS				2//STATUS																衛星狀態
#define ITEM_LATITUDE			3//VALID_LATITUDE,LATITUDE ?度(DDMM.MMMM)
#define ITEM_NS_INDICATOR		4//NS_INDICATOR 南北?
#define ITEM_LONGITUDE			5//VALID_LONGITUDE,LONGITUDE ?度(DDDMM.MMMM)
#define ITEM_EW_INDICATOR		6//EW_INDICATOR 東西?
#define ITEM_SPEED				7//SPEED 速度(浬)
#define ITEM_COURSE				8//COURSE 方向
#define ITEM_UTC_DATE			9//VALID_UTC_DATE,UTC_DATE UTC標準日期
#define ITEM_AntennaHeight		10//海平面高度 Meter
#define ITEM_GeoidalSeparation	11//Wgs84高度(所在地與0?地平均高度差值) Meter


/**************** FileSystem define **************/
//------Constant strings--------------------------
#define _PIC_PAHT					nuTEXT("Media\\Pic\\")
#define _BMP_DAY					nuTEXT("_D.bmp")
#define _BMP_NIGHT					nuTEXT("_N.bmp")
#define _BMP_BG						nuTEXT("BgPic_")
#define _BMP_CROSS					nuTEXT("CrossPic")
/*
#ifdef _USE_DDB_BITMAP
#define FILE_BG_BMP_N				nuTEXT("Media\\Pic\\BgPic_N.bmp")
#define FILE_BG_BMP_D				nuTEXT("Media\\Pic\\BgPic_D.bmp")
#else
#define FILE_BG_BMP_N				nuTEXT("Media\\Pic\\BgPic_N.bmp")
#define FILE_BG_BMP_D				nuTEXT("Media\\Pic\\BgPic_D.bmp")
#endif
*/

#define FILE_3D_BMP_N				nuTEXT("Media\\Pic\\Pic3D_N.bmp")
#define FILE_3D_BMP_D				nuTEXT("Media\\Pic\\Pic3D_D.bmp")
//------For LibFS_ReadConsWcs---------------------
#define WCS_TYPE_ROAD_NAME						0x01
#define WCS_TYPE_DES_NAME						0x02
#define WCS_TYPE_PASS_NAME						0x03
#define WCS_TYPE_ROAD_POSTFIX					0x04
#define WCS_TYPE_CCD_LIMIT						0x05
//------For LibFS_GetFilePath()-------------------
#define FILE_NAME_PATH							0x0000
#define FILE_NAME_USER_CFG						0x0001
#define FILE_NAME_USER_CFG_TMP					0x0002
#define FILE_NAME_MAP_CFG_DAY					0x0003
#define FILE_NAME_MAP_CFG_DAY2					0x0004
#define FILE_NAME_MAP_CFG_NIGHT					0x0005
#define FILE_NAME_MAP_CFG_NIGHT2				0x0006
#define FILE_NAME_EEYETYPE_TEXT					0x0007
//------For LibFS_GetData()------------------------------
#define DATA_USERCONFIG							0x01
#define DATA_MAPCONFIG							0x02
#define DATA_GLOBAL								0x03
#define DATA_DRAWINFO							0x04
#define DATA_DRAWMEM							0x05
#define DATA_SCREENTRANSF						0x06
#define DATA_TRACEMEM							0x07
#define DATA_OTHERGLOBAL						0x08
#define DATA_EEYESPLAYED						0x09
#define DATA_EFFECTIVEMAPDATA					0x10
//------For LibFS_ReLoadResidentData()--------------------
#define RES_DATA_DAYNIGHT_CHANGE				0x00000001
#define RES_DATA_SCALE_CHANGE					0x00000002
#define RES_DATA_3DMODE_CHANGE					0x00000004
/****************** MathTool define ****************/
//------For Screen extended-----------------------
#define SCREEN_HORIZONTAL_EXTEND				0
#define SCREEN_VERTICAL_EXTEND					0
//------For SaveOrShowBmp-------------------------
#define SHOW_DEFAULT							0x00//Show bitmap to screen;
#define SAVETOMEM_BMP1							0x01//Copy bitmap1 to memory
#define SAVETOMEM_BMP2							0x02//Copy bitmap2 to memory
#define SHOWFROMMEM_BMP1						0x11//Copy bitmap1 in memory to memory DC
#define SHOWFROMMEM_BMP2						0x12//Copy bitmap2 in memory to memory DC
#define SHOW_3D_BMP								0x13
#define PAINT_NUROBMP							0x21//
#define PAINT_NUROCROSSBMP						0x22
#define SHOW_MAP_SCREEN							0x30
#define SAVE_SHOW_MAP_TO_BMP1					0x31
#define SAVE_SHOW_MAP_TO_BMP2					0x32
#define SHOW_BMP1_SCREEN						0x33

#define SAVE_ZOOM_MAP							0x40
#define SHOW_ZOOM_MAP_TO_NURODC					0x41

//------For SelectObject--------------------------
/* nDrawType */
#define DRAW_TYPE_ERROR							0x00
#define DRAW_TYPE_ROADRIM						0x01
#define DRAW_TYPE_ROADCENTER					0x02
#define DRAW_TYPE_BGLINERIM						0x03
#define DRAW_TYPE_BGLINECENTER					0x04
#define DRAW_TYPE_BGAREA						0x05

#define DRAW_TYPE_ZOOMFAKE_LINE					0x06
#define DRAW_TYPE_ROADCLINE						0x07

#define DRAW_TYPE_RNFONT						0x10
#define DRAW_TYPE_RNFONTANDBANNER				0x11

#define DRAW_TYPE_POI_FONT						0x20

#define DRAW_TYPE_NAVI_ROAD						0x30
#define DRAW_TYPE_NAVI_ARROW					0x31

#define DRAW_TYPE_ZOOM							0x80
#define DRAW_TYPE_ZOOM_ROADRIM					(DRAW_TYPE_ZOOM + DRAW_TYPE_ROADRIM)
#define DRAW_TYPE_ZOOM_ROADCENTER				(DRAW_TYPE_ZOOM + DRAW_TYPE_ROADCENTER)
#define DRAW_TYPE_ZOOM_ROADCLINE				(DRAW_TYPE_ZOOM + DRAW_TYPE_ROADCLINE)

#define DRAW_TYPE_ZOOM_NAVI_ROAD				(DRAW_TYPE_ZOOM + DRAW_TYPE_NAVI_ROAD)
#define DRAW_TYPE_ZOOM_NAVI_ARROW				(DRAW_TYPE_ZOOM + DRAW_TYPE_NAVI_ARROW)

/* return type */
#define DRWA_OBJ_ONLYCOLNAME					(-1)
#define DRAW_OBJ_SKIP							0//skip drawing, less than 1, no drawing.
#define DRAW_OBJ_2D_LINE						1//line in 2D mode
#define DRAW_OBJ_3D_LINE						2//line in 3D mode, thickness is unchangable
#define DRAW_OBJ_3D_BIRDVIEW					3//line in 3D mode, thickness is changable.
#define DRAW_OBJ_2D_DASH						4
#define DRAW_OBJ_3D_DASH						5
#define DRAW_OBJ_2D_AREA						6
#define DRAW_OBJ_3D_AREA						7
#define DRAW_OBJ_2D_NAVIARROW					8
#define DRAW_OBJ_3D_NAVIARROW					9
#define DRAW_OBJ_2D_NAVILINE					10
#define DRAW_OBJ_3D_NAVILINE					11
#define DRAW_OBJ_2D_ARROWLINE					12
#define DRAW_OBJ_3D_ARROWLINE					13
#define DRAW_OBJ_SQUARE							60
#define DRAW_OBJ_ROADNAME						64
#define DRAW_OBJ_POINAME						65


#define DRAW_OBJ_ZOOM							256		
#define DRAW_OBJ_ZOOM_2D_LINE					(DRAW_OBJ_ZOOM + DRAW_OBJ_2D_LINE)
#define DRAW_OBJ_ZOOM_3D_LINE					(DRAW_OBJ_ZOOM + DRAW_OBJ_3D_LINE)
#define DRAW_OBJ_ZOOM_3D_BIRDVIEW				(DRAW_OBJ_ZOOM + DRAW_OBJ_3D_BIRDVIEW)

#define DRAW_OBJ_ZOOM_2D_NAVIARROW				(DRAW_OBJ_ZOOM + DRAW_OBJ_2D_NAVIARROW)
#define DRAW_OBJ_ZOOM_3D_NAVIARROW				(DRAW_OBJ_ZOOM + DRAW_OBJ_3D_NAVIARROW)
#define DRAW_OBJ_ZOOM_2D_NAVILINE				(DRAW_OBJ_ZOOM + DRAW_OBJ_2D_NAVILINE)
#define DRAW_OBJ_ZOOM_3D_NAVILINE				(DRAW_OBJ_ZOOM + DRAW_OBJ_3D_NAVILINE)
#define DRAW_OBJ_ZOOM_2D_ARROWLINE				(DRAW_OBJ_ZOOM + DRAW_OBJ_2D_ARROWLINE)
#define DRAW_OBJ_ZOOM_3D_ARROWLINE				(DRAW_OBJ_ZOOM + DRAW_OBJ_3D_ARROWLINE)
/* nShowMode */
#define SHOWMODE_2DOBJ							0x00//2D objects
#define SHOWMODE_3DOBJ							0x01//3D objects
//--------For BmpToMap---------------------------------------------
#define MAP_DEFAULT								0x00
#define MAP_ZOOM								0x01

//------------------------------------------------------------------
/******************* nuGDI *************************************/
//For nuDrawTextW's Format flag
#define NURO_DT_TOP              0x00000000
#define NURO_DT_LEFT             0x00000000
#define NURO_DT_CENTER           0x00000001
#define NURO_DT_RIGHT            0x00000002
#define NURO_DT_VCENTER          0x00000004
#define NURO_DT_BOTTOM           0x00000008
#define NURO_DT_WORDBREAK        0x00000010
#define NURO_DT_SINGLELINE       0x00000020
#define NURO_DT_EXPANDTABS       0x00000040
#define NURO_DT_TABSTOP          0x00000080
#define NURO_DT_NOCLIP           0x00000100
#define NURO_DT_EXTERNALLEADING  0x00000200
#define NURO_DT_CALCRECT         0x00000400
#define NURO_DT_NOPREFIX         0x00000800
#define NURO_DT_INTERNAL         0x00001000

#define NURO_DT_EDITCONTROL      0x00002000
#define NURO_DT_PATH_ELLIPSIS    0x00004000
#define NURO_DT_END_ELLIPSIS     0x00008000
#define NURO_DT_MODIFYSTRING     0x00010000
#define NURO_DT_RTLREADING       0x00020000
#define NURO_DT_WORD_ELLIPSIS    0x00040000

/**************nuGDI define**************************/

#define FLAG_START					128
#define POI_START					256		

#define INNAVI_POINT_TO_NAVI_ERROR				0
#define INNAVI_POINT_TO_NAVI_ANTIDIRT			1
#define INNAVI_POINT_TO_NAVI_RIGHT				2
		

///////////////////////////////////////////////////////////////////
#ifdef USING_FILE_SETTING_VIEW_ANGLE
// Added by Dengxu @ 2012 11 04
//#define _3D_MIN_ANGLE             LibMT_GetMinViewAngle()
//#define _3D_MAX_ANGLE             LibMT_GetMaxViewAngle()
#else
#define _3D_MIN_ANGLE				10
#define _3D_MAX_ANGLE				25
#endif




#define _3D_MIN_SCALE_V				25
#define _3D_MAX_SCALE_V				100

#endif
