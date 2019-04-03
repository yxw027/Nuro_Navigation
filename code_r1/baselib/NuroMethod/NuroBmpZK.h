// NuroBmpZK.h: interface for the CNuroBmpZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROBMPZK_H__FD36C3A3_B566_492E_A988_5D28BC09D369__INCLUDED_)
#define AFX_NUROBMPZK_H__FD36C3A3_B566_492E_A988_5D28BC09D369__INCLUDED_

#include "../NuroDefine.h"

class CNuroBmpZK  
{
public:

#ifdef NURO_OS_WINDOWS
#pragma pack(push) 
#pragma pack(1)
#endif
    
    __ATTRIBUTE_PRE struct nutagBITMAPFILEHEADER {
        nuWORD		bfType;  /* 位图的文件类型 BM */
        nuDWORD	    bfSize;  /* 位图文件的大小 */
        nuWORD		bfReserved1;
        nuWORD		bfReserved2;
        nuDWORD	bfOffBits;   /* 位图数据的起始位置 */
    }__ATTRIBUTE_PACKED__;
    typedef struct nutagBITMAPFILEHEADER NURO_BITMAPFILEHEADER;
    
    __ATTRIBUTE_PRE struct nutagBITMAPINFOHEADER{
        nuDWORD	    biSize;      /* 本结构所占用字节数 */
        nuINT		biWidth;     /* 位图宽度  */
        nuINT		biHeight;    /* 位图高度 */
        nuWORD		biPlanes;    /* 目标设备的级别 */
        nuWORD		biBitCount;  /* 每个像素所占用的位数 1(双色) 4(16色) 8(256) 24(真彩)*/
        nuDWORD	biCompression;   /* 位图压缩类型 0 1 2 之一 */
        nuDWORD	biSizeImage;     /* 位图的大小，以字节为单位 */
        nuINT		biXPelsPerMeter;  /* 位图水平分辨率 */
        nuINT		biYPelsPerMeter;  /* 位图垂直分辨率 */
        nuDWORD	biClrUsed;           /* 实际使用的颜色表中的颜色数 */
        nuDWORD	biClrImportant;      /* 位图显示中过程中的重要颜色数 */
    }__ATTRIBUTE_PACKED__;
    typedef struct nutagBITMAPINFOHEADER NURO_BITMAPINFOHEADER;
    
    __ATTRIBUTE_PRE struct nutagRGBQUAD {
        nuBYTE    rgbBlue;    /* 蓝色的亮度 */
        nuBYTE    rgbGreen;   /* 绿色的亮度 */
        nuBYTE    rgbRed;     /* 红色的亮度 */
        nuBYTE    rgbReserved; /* 保留，必须为0 */
    }__ATTRIBUTE_PACKED__;
    typedef struct nutagRGBQUAD NURO_RGBQUAD;
    
    struct nutagBITMAPINFO {
        NURO_BITMAPINFOHEADER	bmiHeader;   /* 位图信息头 */
        NURO_RGBQUAD			bmiColors[1]; /* 位图颜色表 */
    }__ATTRIBUTE_PACKED__;
    typedef struct nutagBITMAPINFO NURO_BITMAPINFO; /* 位图信息 */
    
    
	/* 一个扫描行内占用的字节数，必须是4的倍数,不足的补零 */
#ifdef NURO_OS_WINDOWS
#pragma pack(pop) 
#endif

public:
	CNuroBmpZK();
	virtual ~CNuroBmpZK();

    nuBOOL LoadNuroBmp(const nuCHAR* ptsBmpName);
    nuBOOL LoadNuroBmp(const nuWCHAR* ptsBmpName);
	nuBOOL CreateNuroBmp(const nuWCHAR* ptsBmpName,nuLONG With,nuLONG High);
    nuVOID FreeNuroBmp();

public:
    NURO_BMP    m_bmp;
};

#endif // !defined(AFX_NUROBMPZK_H__FD36C3A3_B566_492E_A988_5D28BC09D369__INCLUDED_)
