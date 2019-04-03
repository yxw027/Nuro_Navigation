// nuroftface.h: interface for the nuroftface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROFTFACE_H__AA3B1582_F9F0_4AB0_9F18_F3D42ADA0B4C__INCLUDED_)
#define AFX_NUROFTFACE_H__AA3B1582_F9F0_4AB0_9F18_F3D42ADA0B4C__INCLUDED_

#include "NuroDefine.h"
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/ftstroke.h>
#include <freetype/internal/ftstream.h>

FT_CALLBACK_DEF(nuULONG) nuro_FT_Stream_IO( FT_Stream stream, nuULONG offset, unsigned char*  buffer, nuULONG count );
FT_CALLBACK_DEF(nuVOID) nuro_FT_Stream_Close( FT_Stream  stream );
FT_BASE_DEF(FT_Error) nuro_FT_Stream_Open(FT_Stream stream, nuFILE* fp);
FT_BASE_DEF(FT_Error) nuro_FT_Open_Face( FT_Library library, FT_Long face_index, FT_Face *aface, nuFILE* fp);

#endif // !defined(AFX_NUROFTFACE_H__AA3B1582_F9F0_4AB0_9F18_F3D42ADA0B4C__INCLUDED_)
