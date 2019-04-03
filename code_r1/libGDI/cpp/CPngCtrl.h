#ifndef DEF_CPNGCTRL_LOUIS
	#define DEF_CPNGCTRL_LOUIS

	#include "NuroClasses.h"

	#define USEPNG

	#define PNG_DRAWMODE_LT			0x00//以PNG左上角為繪圖原點(往右下角畫)	<一般畫圖常用此法>
	#define PNG_DRAWMODE_LC			0x01//以PNG左邊中間為繪圖原點(往右畫)
	#define PNG_DRAWMODE_LB			0x02//以PNG左下角為繪圖原點(往右上角畫)
	#define PNG_DRAWMODE_CT			0x03//以PNG上邊中間為繪圖原點(往下畫)
	#define PNG_DRAWMODE_CC			0x04//以PNG中間為繪圖原點				<2D POI常用此法>
	#define PNG_DRAWMODE_CB			0x05//以PNG下邊中間為繪圖原點(往上畫)	<3D POI常用此法>
	#define PNG_DRAWMODE_RT			0x06//以PNG右上角為繪圖原點(往左下角畫)	
	#define PNG_DRAWMODE_RC			0x07//以PNG右邊中間為繪圖原點(往左畫)
	#define PNG_DRAWMODE_RB			0x08//以PNG左下角為繪圖原點(往右上角畫)

	#define MEMDCCOLORTYPE_16_565	0x00//16位元色 RGBA配置為5650
	#define MEMDCCOLORTYPE_24_888	0x01//24位元色 RGBA配置為8880
	#define MEMDCCOLORTYPE_32_8888	0x02//32位元色 RGBA配置為8888

	#define nuGet565BValue(rgb)     ( (unsigned int)((rgb)&0x001f) )
	#define nuGet565GValue(rgb)     ( (unsigned int)((rgb)&0x07D0)>>5 ) 
	#define nuGet565RValue(rgb)     ( (unsigned int)((rgb))>>11 )
//	#define nu565RGB(r,g,b)          ((nuCOLORREF)(((nuBYTE)(r)>>3|((nuWORD)(((nuBYTE)(g))>>2)<<5))|(((nuDWORD)(nuBYTE)(b)>>3)<<11)))
	#define nu565RGB(r,g,b)          ((nuCOLORREF)(((nuBYTE)(b)|((nuWORD)(((nuBYTE)(g)))<<5))|(((nuDWORD)(nuBYTE)(r))<<11)))

	#define PNG_BYTES_TO_CHECK 4
	
	//#ifdef ANDROID
 #if (defined ANDROID) || (defined NURO_OS_LINUX)
	#define GetRValue(rgb)      (nuLOBYTE(rgb))
	#define GetGValue(rgb)      (nuLOBYTE(((nuWORD)(rgb)) >> 8))
	#define GetBValue(rgb)      (nuLOBYTE((rgb)>>16))
	#define RGB(r,g,b)          ((nuCOLORREF)(((nuBYTE)(r)|((nuWORD)((nuBYTE)(g))<<8))|(((nuDWORD)(nuBYTE)(b))<<16)))
	#endif

	class CPngCtrlLU   
	{
		public:
			CPngCtrlLU();
			virtual ~CPngCtrlLU();

			//PNG File Control
			nuBOOL	InitPngCtrl(nuINT PngBufferCount);//初始化PNG Ctrl的所有內容
			nuVOID	RelPngCtrl();//解構記憶體

			nuINT		check_if_png(nuTCHAR *file_name, nuFILE **fp);
			nuBOOL	load_png(nuTCHAR *file_name,nuINT &PngIndex,nuINT PngID);//PngID可以用來進行指定搜尋,不使用的話直接填-1即可
			nuBOOL	Clean_Png(nuINT PngIndex,nuBOOL bCleanAll);
			nuVOID	Draw_PNG_EX(nuINT PngIndex,nuBYTE *MemBuffer,NURORECT Targetrect,nuINT DrawX,nuINT DrawY,nuINT PNGDrawMode);
			nuVOID	Draw_PNG(nuINT PngIndex,nuBYTE *MemBuffer,NURORECT Targetrect,nuINT DrawX,nuINT DrawY);
			nuVOID	SetMemDCColorType(nuINT ColorType);
			nuBOOL	Get_PngData_idx(nuINT PngIndex,PPNGCTRLSTRU &p_png);//取得資料的方式 使用index
			nuBOOL	Get_PngData_id(nuINT PngID,PPNGCTRLSTRU &p_png);//取得資料的方式 使用ID
			nuVOID	Draw_PNG_EX2(PPNGCTRLSTRU p_png,nuBYTE *MemBuffer,NURORECT Targetrect,nuINT DrawX,nuINT DrawY,nuINT PNGDrawMode);
		public:
			PNGCTRLSTRU *pBufferPng;
			nuINT nBufferPngCount;
			nuBOOL bInitPngCtrl;
			nuINT nMemDCColorType;
		private:
			nuBOOL	nuPtInRect(nuINT x,nuINT y, NURORECT rect);

	};

#endif