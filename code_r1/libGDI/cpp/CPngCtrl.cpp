//#include "stdafx.h"
#include "CPngCtrl.h"
#ifdef USEPNG
	#include "png.h"
	#ifndef ANDROID
	#pragma comment(lib, "png.lib")
	#endif
#endif

CPngCtrlLU::CPngCtrlLU()
{
	pBufferPng		=	NULL;
	nBufferPngCount	=	0;
	bInitPngCtrl	=	false;
	nMemDCColorType	=	0;
}

CPngCtrlLU::~CPngCtrlLU()
{
	RelPngCtrl();
}

nuBOOL CPngCtrlLU::InitPngCtrl(nuINT PngBufferCount)//初始化記憶體區塊(用來作為日後PNG資料的使用區)
{
		nuINT i;
		if(PngBufferCount==0)
		{
			return false;
		}

		if(bInitPngCtrl)//清空舊資料(如果是重覆呼叫的話)
		{
			RelPngCtrl();
		}
		
		nBufferPngCount = PngBufferCount;
		pBufferPng		= new PNGCTRLSTRU[nBufferPngCount];	
		//Buffer清空
		if(pBufferPng)
		{
			for(i=0;i<nBufferPngCount;i++)
			{
				pBufferPng[i].PngID		=	-1;
				pBufferPng[i].bUseCount	=	0;
				pBufferPng[i].Height	=	0;
				pBufferPng[i].Width		=	0;
				pBufferPng[i].Color		=	NULL;
			}
			bInitPngCtrl=true;
			return true;
		}
		else
		{
			bInitPngCtrl=false;
			return false;
		}
}

nuVOID CPngCtrlLU::RelPngCtrl()
{
		nuINT i=0;
		bInitPngCtrl=false;
		//Buffer清空
		if(pBufferPng)
		{
			for(i=0;i<nBufferPngCount;i++)
			{
				Clean_Png(i,true);
			}
			delete [] pBufferPng;
			pBufferPng=NULL;
		}
		nBufferPngCount=0;
}

nuBOOL CPngCtrlLU::Clean_Png(nuINT PngIndex,nuBOOL bCleanAll)
{
	if(bInitPngCtrl==false || pBufferPng==NULL || nBufferPngCount==0)
	{
		return false;
	}

	if(0<=PngIndex && PngIndex<nBufferPngCount)
	{
		if(bCleanAll)
		{
			pBufferPng[PngIndex].bUseCount	=	0;
			pBufferPng[PngIndex].PngID		=	-1;
		}
		else
		{
			pBufferPng[PngIndex].bUseCount--;
			if(pBufferPng[PngIndex].bUseCount!=0)
			{
				return true;
			}
		}
		pBufferPng[PngIndex].Height=0;
		pBufferPng[PngIndex].Width=0;
		if(pBufferPng[PngIndex].Color)
		{
			delete [] pBufferPng[PngIndex].Color;
			pBufferPng[PngIndex].Color=NULL;
		}
		return true;
	}
	else
	{
		return false;
	}
}


nuINT CPngCtrlLU::check_if_png(nuTCHAR *file_name, nuFILE **fp)
{
	#ifdef USEPNG
	   char buf[PNG_BYTES_TO_CHECK];

	   // Open the prospective PNG file.
	   if ((*fp = nuTfopen(file_name, NURO_FS_RB)) == NULL)
		  return 0;

	   // Read in some of the signature bytes
	   if (nuFread(buf, 1, PNG_BYTES_TO_CHECK, *fp) != PNG_BYTES_TO_CHECK)
		  return 0;

		nuFclose(*fp);

	   // Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	   // Return nonzero (true) if they match
	   return(!png_sig_cmp((png_const_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
	#else
		return 0;
	#endif
}

nuBOOL CPngCtrlLU::load_png(nuTCHAR *file_name,nuINT &PngIndex,nuINT PngID)  /* We need to open the file */
{
	#ifdef USEPNG
		png_structp png_ptr;
		png_infop info_ptr;
		nuUINT sig_read = 0;
		nuFILE *fp;
		png_bytep *row_ponuINTers;
		nuINT i,j;
		nuINT pos=0;
		PngIndex=-1;

		if(bInitPngCtrl==false || pBufferPng==NULL || nBufferPngCount==0)
		{
			return false;
		}

		for(i=0;i<nBufferPngCount;i++)
		{
			if(pBufferPng[i].bUseCount==0)
			{
				PngIndex=i;
				break;
			}
			else if(i==(nBufferPngCount-1))
			{
				return false;
			}
		}

		//開啟png檔案
		if ((fp = nuTfopen(file_name, NURO_FS_RB)) == NULL)
			return false;

		//創建png主體結構
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png_ptr == NULL)
		{
			nuFclose(fp);
			return false;
		}

		//創建png info結構
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL)
		{
			nuFclose(fp);
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return false;
		}

		//檢測png_ptr是否有錯誤狀態
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			nuFclose(fp);
			return false;
		}

		png_init_io(png_ptr, (png_FILE_p)fp);//設定png結構讀取來源(適用於單點讀擋 如果是多處則會因為使用libpng內部參數處理 而發生干擾)
		png_set_sig_bytes(png_ptr, sig_read);//類似fseek的動作 用來設定需pass多少資料 目前預設是完全使用
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);//取得png資料(一次性讀取) 不轉換

		pBufferPng[PngIndex].Width	=	png_get_image_width(png_ptr,info_ptr);
		pBufferPng[PngIndex].Height	=	png_get_image_height(png_ptr,info_ptr);
		pBufferPng[PngIndex].Color	=	new unsigned char[pBufferPng[PngIndex].Width*pBufferPng[PngIndex].Height*4];
		if(pBufferPng[PngIndex].Color==NULL)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			nuFclose(fp);
			return false;
		}
		row_ponuINTers=(png_bytep*)png_get_rows(png_ptr,info_ptr);
		for(i = 0; i < pBufferPng[PngIndex].Height; i++)
		{
			for(j = 0; j < (4 * pBufferPng[PngIndex].Width); j += 4)
			{
				pBufferPng[PngIndex].Color[pos++] = row_ponuINTers[i][j];		// red
				pBufferPng[PngIndex].Color[pos++] = row_ponuINTers[i][j + 1]; // green
				pBufferPng[PngIndex].Color[pos++] = row_ponuINTers[i][j + 2]; // blue
				pBufferPng[PngIndex].Color[pos++] = row_ponuINTers[i][j + 3]; // alpha 0(全透)~255(不透)
			}
		}
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);//解構資料png資料
		nuFclose(fp);//關閉程式
		pBufferPng[PngIndex].bUseCount++;
		pBufferPng[PngIndex].PngID=PngID;
		return true;
	#else
		PngIndex=-1;
		return false;
	#endif
}

nuBOOL CPngCtrlLU::Get_PngData_idx(nuINT PngIndex,PPNGCTRLSTRU &p_png)
{
	if(bInitPngCtrl==false || pBufferPng==NULL || nBufferPngCount==0)
	{
		p_png=NULL;
		return false;
	}
	
	if(0<=PngIndex && PngIndex<nBufferPngCount && pBufferPng[PngIndex].bUseCount>0)
	{
		p_png=&(pBufferPng[PngIndex]);
		return true;
	}
	else
	{
		p_png=NULL;
		return false;
	}
}

nuBOOL CPngCtrlLU::Get_PngData_id(nuINT PngID,PPNGCTRLSTRU &p_png)
{
	if(bInitPngCtrl==false || pBufferPng==NULL || nBufferPngCount==0)
	{
		p_png=NULL;
		return false;
	}
	nuINT i=0;
	for(i=0;i<nBufferPngCount;i++)
	{
		if(PngID==pBufferPng[i].PngID)
		{
			p_png=&(pBufferPng[i]);
			return true;
		}
	}
	p_png=NULL;
	return false;
}


//需要了解系統所用的顏色格式 才能順利套用
nuVOID CPngCtrlLU::Draw_PNG_EX(nuINT PngIndex,nuBYTE *MemBuffer,NURORECT Targetrect,nuINT DrawX,nuINT DrawY,nuINT PNGDrawMode)
{
	nuroPOINT PNGShift;
	if(bInitPngCtrl==false || pBufferPng==NULL || nBufferPngCount==0)
	{
		return;
	}
	nuINT ColorIndex=0;
	if(0<=PngIndex && PngIndex<nBufferPngCount && pBufferPng[PngIndex].bUseCount>0)
	{
		//依據不同的DrawMode會有不同的起點
		switch(PNGDrawMode)
		{
			case PNG_DRAWMODE_LT:
				PNGShift.x=0;
				PNGShift.y=0;
				break;
			case PNG_DRAWMODE_LC:
				PNGShift.x=0;
				PNGShift.y=-pBufferPng[PngIndex].Height/2;
				break;
			case PNG_DRAWMODE_LB:
				PNGShift.x=0;
				PNGShift.y=-pBufferPng[PngIndex].Height;
				break;
			case PNG_DRAWMODE_CT:
				PNGShift.x=-pBufferPng[PngIndex].Width/2;
				PNGShift.y=0;
				break;
			case PNG_DRAWMODE_CC:
				PNGShift.x=-pBufferPng[PngIndex].Width/2;
				PNGShift.y=-pBufferPng[PngIndex].Height/2;
				break;
			case PNG_DRAWMODE_CB:
				PNGShift.x=-pBufferPng[PngIndex].Width/2;
				PNGShift.y=-pBufferPng[PngIndex].Height;
				break;
			case PNG_DRAWMODE_RT:
				PNGShift.x=-pBufferPng[PngIndex].Width;
				PNGShift.y=0;
				break;
			case PNG_DRAWMODE_RC:
				PNGShift.x=-pBufferPng[PngIndex].Width;
				PNGShift.y=-pBufferPng[PngIndex].Height/2;
				break;
			case PNG_DRAWMODE_RB:
				PNGShift.x=-pBufferPng[PngIndex].Width;
				PNGShift.y=-pBufferPng[PngIndex].Height;
				break;
			default:
				PNGShift.x=0;
				PNGShift.y=0;
				break;
		}

		Draw_PNG(PngIndex,MemBuffer,Targetrect,DrawX+PNGShift.x,DrawY+PNGShift.y);
	}

	return;
}

nuVOID CPngCtrlLU::SetMemDCColorType(nuINT ColorType)
{
	nMemDCColorType=ColorType;
}

nuVOID CPngCtrlLU::Draw_PNG(nuINT PngIndex,nuBYTE *MemBuffer,NURORECT Targetrect,nuINT DrawX,nuINT DrawY)
{
	nuINT i,j;
	nuCOLORREF TmpColor;
	float falpha,balpha;
	float newcolorR,newcolorG,newcolorB;
	nuroPOINT DrawPt;
	
	if(bInitPngCtrl==false || pBufferPng==NULL || nBufferPngCount==0)
	{
		return;
	}

	nuINT ColorIndex=0;
	if(0<=PngIndex && PngIndex<nBufferPngCount && pBufferPng[PngIndex].bUseCount>0)
	{
		switch(nMemDCColorType)
		{
			case MEMDCCOLORTYPE_16_565://RGBA 5650
				for(i=0; i < pBufferPng[PngIndex].Height; i++)
				{
					for(j=0; j < pBufferPng[PngIndex].Width; j++)
					{
						DrawPt.x=DrawX+j;
						DrawPt.y=DrawY+i;
						if(!nuPtInRect(DrawPt.x,DrawPt.y, Targetrect)) continue;
						ColorIndex=(i*pBufferPng[PngIndex].Width+j)*4;
						//與背景混合
						TmpColor=0;
						nuMemcpy(&TmpColor,&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*2]),2);
						falpha=(float)(pBufferPng[PngIndex].Color[ColorIndex+3]*1.00/255.00);
						balpha=(float)((255-pBufferPng[PngIndex].Color[ColorIndex+3])*1.00/255.00);
						newcolorR	=	falpha*(pBufferPng[PngIndex].Color[ColorIndex]>>3)		+	balpha*nuGet565RValue(TmpColor);
						newcolorG	=	falpha*(pBufferPng[PngIndex].Color[ColorIndex+1]>>2)	+	balpha*nuGet565GValue(TmpColor);
						newcolorB	=	falpha*(pBufferPng[PngIndex].Color[ColorIndex+2]>>3)	+	balpha*nuGet565BValue(TmpColor);
						TmpColor=nu565RGB((unsigned char)newcolorR,(unsigned char)newcolorG,(unsigned char)newcolorB);
						nuMemcpy(&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*2]),&TmpColor,2);
					}
				}
				break;
			case MEMDCCOLORTYPE_24_888://RGBA 8880 混色處理還沒確認
				for(i=0; i < pBufferPng[PngIndex].Height; i++)
				{
					for(j=0; j < pBufferPng[PngIndex].Width; j++)
					{
						DrawPt.x=DrawX+j;
						DrawPt.y=DrawY+i;
						if(!nuPtInRect(DrawPt.x,DrawPt.y, Targetrect)) continue;
						ColorIndex=(i*pBufferPng[PngIndex].Width+j)*4;
						//與背景混合
						nuMemcpy(&TmpColor,&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*3]),3);
						falpha=(float)(pBufferPng[PngIndex].Color[ColorIndex+3]*1.00/255.00);
						balpha=(float)((255-pBufferPng[PngIndex].Color[ColorIndex+3])*1.00/255.00);
						newcolorR=falpha*pBufferPng[PngIndex].Color[ColorIndex]+balpha*GetRValue(TmpColor);
						newcolorG=falpha*pBufferPng[PngIndex].Color[ColorIndex+1]+balpha*GetGValue(TmpColor);
						newcolorB=falpha*pBufferPng[PngIndex].Color[ColorIndex+2]+balpha*GetBValue(TmpColor);
						TmpColor=RGB((unsigned char)newcolorR,(unsigned char)newcolorG,(unsigned char)newcolorB);
						nuMemcpy(&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*3]),&TmpColor,3);
					}
				}
				break;
			case MEMDCCOLORTYPE_32_8888://RGBA 8888 混色處理還沒確認
				for(i=0; i < pBufferPng[PngIndex].Height; i++)
				{
					for(j=0; j < pBufferPng[PngIndex].Width; j++)
					{
						DrawPt.x=DrawX+j;
						DrawPt.y=DrawY+i;
						if(!nuPtInRect(DrawPt.x,DrawPt.y, Targetrect)) continue;
						ColorIndex=(i*pBufferPng[PngIndex].Width+j)*4;
						//與背景混合
						nuMemcpy(&TmpColor,&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*4]),4);
						falpha=(float)(pBufferPng[PngIndex].Color[ColorIndex+3]*1.00/255.00);
						balpha=(float)((255-pBufferPng[PngIndex].Color[ColorIndex+3])*1.00/255.00);
						newcolorR=falpha*pBufferPng[PngIndex].Color[ColorIndex]+balpha*GetRValue(TmpColor);
						newcolorG=falpha*pBufferPng[PngIndex].Color[ColorIndex+1]+balpha*GetGValue(TmpColor);
						newcolorB=falpha*pBufferPng[PngIndex].Color[ColorIndex+2]+balpha*GetBValue(TmpColor);
						TmpColor=RGB((unsigned char)newcolorR,(unsigned char)newcolorG,(unsigned char)newcolorB);
						nuMemcpy(&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*4]),&TmpColor,4);
					}
				}
				break;
		}
	}
	return;
}

nuVOID CPngCtrlLU::Draw_PNG_EX2(PPNGCTRLSTRU p_png,nuBYTE *MemBuffer,NURORECT Targetrect,nuINT DrawX,nuINT DrawY,nuINT PNGDrawMode)
{
	nuINT i,j;
	nuCOLORREF TmpColor;
	float falpha,balpha;
	float newcolorR,newcolorG,newcolorB;
	nuroPOINT DrawPt;
	
	if(bInitPngCtrl==false || pBufferPng==NULL || nBufferPngCount==0)
	{
		return;
	}

	nuINT ColorIndex=0;
	if(p_png->bUseCount>0)
	{

		switch(PNGDrawMode)
		{
			case PNG_DRAWMODE_LC:
				DrawY -= p_png->Height/2;
				break;
			case PNG_DRAWMODE_LB:
				DrawY -= p_png->Height;
				break;
			case PNG_DRAWMODE_CT:
				DrawX -= p_png->Width/2;
				break;
			case PNG_DRAWMODE_CC:
				DrawX -= p_png->Width/2;
				DrawY -= p_png->Height/2;
				break;
			case PNG_DRAWMODE_CB:
				DrawX -= p_png->Width/2;
				DrawY -= p_png->Height;
				break;
			case PNG_DRAWMODE_RT:
				DrawX -= p_png->Width;
				break;
			case PNG_DRAWMODE_RC:
				DrawX -= p_png->Width;
				DrawY -= p_png->Height/2;
				break;
			case PNG_DRAWMODE_RB:
				DrawX -= p_png->Width;
				DrawY -= p_png->Height;
				break;
			default:
			case PNG_DRAWMODE_LT:
				break;
		}

		switch(nMemDCColorType)
		{
			case MEMDCCOLORTYPE_16_565://RGBA 5650
				for(i=0; i < p_png->Height; i++)
				{
					for(j=0; j < p_png->Width; j++)
					{
						DrawPt.x=DrawX+j;
						DrawPt.y=DrawY+i;
						if(!nuPtInRect(DrawPt.x,DrawPt.y, Targetrect)) continue;
						if( DrawPt.y == Targetrect.bottom - Targetrect.top )
						{
							continue;
						}
						ColorIndex=(i*p_png->Width+j)*4;
						//與背景混合
						TmpColor=0;
						nuMemcpy(&TmpColor, &(MemBuffer[(DrawPt.y * (Targetrect.right - Targetrect.left) + DrawPt.x)*2]), 2);
						falpha=(float)(p_png->Color[ColorIndex+3]*1.00/255.00);
						balpha=(float)((255-p_png->Color[ColorIndex+3])*1.00/255.00);
						newcolorR	=	falpha*(p_png->Color[ColorIndex]>>3)	+	balpha*nuGet565RValue(TmpColor);
						newcolorG	=	falpha*(p_png->Color[ColorIndex+1]>>2)	+	balpha*nuGet565GValue(TmpColor);
						newcolorB	=	falpha*(p_png->Color[ColorIndex+2]>>3)	+	balpha*nuGet565BValue(TmpColor);
						TmpColor=nu565RGB((unsigned char)newcolorR,(unsigned char)newcolorG,(unsigned char)newcolorB);
						nuMemcpy(&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*2]),&TmpColor,2);
					}
				}
				break;
			case MEMDCCOLORTYPE_24_888://RGBA 8880 混色處理還沒確認
				for(i=0; i < p_png->Height; i++)
				{
					for(j=0; j < p_png->Width; j++)
					{
						DrawPt.x=DrawX+j;
						DrawPt.y=DrawY+i;
						if(!nuPtInRect(DrawPt.x,DrawPt.y, Targetrect)) continue;
						ColorIndex=(i*p_png->Width+j)*4;
						//與背景混合
						nuMemcpy(&TmpColor,&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*3]),3);
						falpha=(float)(p_png->Color[ColorIndex+3]*1.00/255.00);
						balpha=(float)((255-p_png->Color[ColorIndex+3])*1.00/255.00);
						newcolorR=falpha*p_png->Color[ColorIndex]+balpha*GetRValue(TmpColor);
						newcolorG=falpha*p_png->Color[ColorIndex+1]+balpha*GetGValue(TmpColor);
						newcolorB=falpha*p_png->Color[ColorIndex+2]+balpha*GetBValue(TmpColor);
						TmpColor=RGB((unsigned char)newcolorR,(unsigned char)newcolorG,(unsigned char)newcolorB);
						nuMemcpy(&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*3]),&TmpColor,3);
					}
				}
				break;
			case MEMDCCOLORTYPE_32_8888://RGBA 8888 混色處理還沒確認
				for(i=0; i < p_png->Height; i++)
				{
					for(j=0; j < p_png->Width; j++)
					{
						DrawPt.x=DrawX+j;
						DrawPt.y=DrawY+i;
						if(!nuPtInRect(DrawPt.x,DrawPt.y, Targetrect)) continue;
						ColorIndex=(i*p_png->Width+j)*4;
						//與背景混合
						nuMemcpy(&TmpColor,&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*4]),4);
						falpha=(float)(p_png->Color[ColorIndex+3]*1.00/255.00);
						balpha=(float)((255-p_png->Color[ColorIndex+3])*1.00/255.00);
						newcolorR=falpha*p_png->Color[ColorIndex]+balpha*GetRValue(TmpColor);
						newcolorG=falpha*p_png->Color[ColorIndex+1]+balpha*GetGValue(TmpColor);
						newcolorB=falpha*p_png->Color[ColorIndex+2]+balpha*GetBValue(TmpColor);
						TmpColor=RGB((unsigned char)newcolorR,(unsigned char)newcolorG,(unsigned char)newcolorB);
						nuMemcpy(&(MemBuffer[(DrawPt.y*(Targetrect.right-Targetrect.left)+DrawPt.x)*4]),&TmpColor,4);
					}
				}
				break;
		}
	}
	return;
}


nuBOOL CPngCtrlLU::nuPtInRect(nuINT x,nuINT y, const NURORECT rect)
{
	return (( x >= rect.left && x <= rect.right ) & 
		( y >= rect.top && y <= rect.bottom ));
}
