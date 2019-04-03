#ifndef __NURO_BMP_H_2012_07_23
#define __NURO_BMP_H_2012_07_23

#include "../Inc/NuroTypes.h"

//For nuro bitmap
//@bmpActionType
#define NURO_BMP_TYPE_MIX0			0X00
#define NURO_BMP_TYPE_MIX1			0X01
#define NURO_BMP_TYPE_MIX2			0X02
#define NURO_BMP_TYPE_MIX3			0X03
#define NURO_BMP_TYPE_MIX4			0X04
#define NURO_BMP_TYPE_MIX5			0X05
#define NURO_BMP_TYPE_MIX6			0X06
#define NURO_BMP_TYPE_MIX7			0X07
#define NURO_BMP_TYPE_MIX8			0X08
#define NURO_BMP_TYPE_MIX9			0X09
#define NURO_BMP_TYPE_MIXA			0X0A
#define NURO_BMP_TYPE_MIXB			0X0B
#define NURO_BMP_TYPE_MIXC			0X0C
#define NURO_BMP_TYPE_MIXD			0X0D
#define NURO_BMP_TYPE_MIXE			0X0E
#define NURO_BMP_TYPE_MIXF			0X0F
#define NURO_BMP_TYPE_MIXX			0X10

#define NURO_BMP_TYPE_USE_TRANSCOLOR    NURO_BMP_TYPE_MIXX  //DON'T USE "NURO_BMP_TYPE_USE_TRANSCOLOR"


#define NURO_BMP_TYPE_AND			0x11
#define NURO_BMP_TYPE_OR			0x12
#define NURO_BMP_TYPE_XOR			0x14

#define NURO_BMP_TYPE_NBK			0x20
#define NURO_BMP_TYPE_ANDNBK		0x21
#define NURO_BMP_TYPE_ORNBK			0x22
#define NURO_BMP_TYPE_XORNBK		0x24

#define NURO_BMP_TYPE_COPY			0x40

#define NURO_BMP_BITCOUNT_16			16
#define NURO_BMP_BITCOUNT_24			24

#define NURO_BMP_BITCOUNT				(NURO_BMP_BITCOUNT_16)
#define NURO_BYTES_PERPIXEL	            (2)

#define NURO_BMP_16_MR					0xF800
#define NURO_BMP_16_MG					0x07E0
#define NURO_BMP_16_MB					0x001F

#define NURO_BMP_FORMAT_565             0

#define NURO_BMP_BUFFERTYPE_OUT     0
#define NURO_BMP_BUFFERTYPE_NEW     1
#define NURO_BMP_BUFFERTYPE_MALLOC  2

#define NURO_BMP_TRANSCOLOR_565 1440

//--------------------------------------------------
//bmpReserve
#define NURO_BMP_DIB_BUFFER					0x00
#define NURO_BMP_DIB_SECTION				0x01
//--------------------------------------------------

#ifdef NURO_OS_WINDOWS
#pragma pack(push) 
#pragma pack(1)
#endif

typedef struct tagNURO_BMP
{
	//
	nuBYTE		bmpType;
	nuBYTE		bmpHasLoad:1;//load bitmap tag
	//
	nuBYTE		bmpReserve;
	nuBYTE      bmpBufferType;
	nuWORD		bmpWidth;
	nuWORD		bmpHeight;
	nuWORD		bmpBitCount;
	nuWORD      bytesPerLine;
	nuDWORD		bmpTransfColor;
	nuDWORD		bmpBuffLen;
	nuBYTE*		pBmpBuff;

}NURO_BMP, *PNURO_BMP;

__ATTRIBUTE_PRE struct nutagBITMAPFILEHEADER {
	nuWORD		bfType;
	nuDWORD	bfSize;
	nuWORD		bfReserved1;
	nuWORD		bfReserved2;
	nuDWORD	bfOffBits;
}__ATTRIBUTE_PACKED__;
typedef struct nutagBITMAPFILEHEADER NURO_BITMAPFILEHEADER;

__ATTRIBUTE_PRE struct nutagBITMAPINFOHEADER{
	nuDWORD	biSize;
	nuINT		biWidth;
	nuINT		biHeight;
	nuWORD		biPlanes;
	nuWORD		biBitCount;
	nuDWORD	biCompression;
	nuDWORD	biSizeImage;
	nuINT		biXPelsPerMeter;
	nuINT		biYPelsPerMeter;
	nuDWORD	biClrUsed;
	nuDWORD	biClrImportant;
}__ATTRIBUTE_PACKED__;
typedef struct nutagBITMAPINFOHEADER NURO_BITMAPINFOHEADER;

__ATTRIBUTE_PRE struct nutagRGBQUAD {
	nuBYTE    rgbBlue;
	nuBYTE    rgbGreen;
	nuBYTE    rgbRed;
	nuBYTE    rgbReserved;
}__ATTRIBUTE_PACKED__;
typedef struct nutagRGBQUAD NURO_RGBQUAD;

struct nutagBITMAPINFO {
	NURO_BITMAPINFOHEADER	bmiHeader;
	NURO_RGBQUAD			bmiColors[1];
}__ATTRIBUTE_PACKED__;
typedef struct nutagBITMAPINFO NURO_BITMAPINFO;


#ifdef NURO_OS_WINDOWS
#pragma pack(pop) 
#endif


#endif
