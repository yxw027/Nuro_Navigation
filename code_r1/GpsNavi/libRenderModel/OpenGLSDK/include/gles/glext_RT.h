/*
//  (C) 2009-2010 Renesas Electronics Corporation. All rights reserved.
//
//  GL extended functions by Renesas Electronics.
//
// FILE     : glext_RT.h
// CREATED  : 2010.04.21
// MODIFIED : -
// AUTHOR   : Renesas Electronics Corporation
// DEVICE   : SH-Navi device family
// HISTORY  : 2010.04.21
//            - Created release code.
//
*/

#ifndef __glext_RT_h_
#define __glext_RT_h_


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


/*
	RT definitions
*/

/* common */
#define GL_NONE_RT                      0x00000000UL

/* DrawPixels */
#define GL_COLOR_INDEX                  0x1900					/* from desktop GL */
#define GL_ARGB_RT                      0x1981					/* from desktop GL */

#define GL_BITMAP                       0x1A00					/* from desktop GL */
#define GL_UNSIGNED_SHORT_1_5_5_5_RT    0x1481

/* Image */
#define GL_ALPHAMAP_IMAGE_RT            0x00121001UL
#define GL_COLOR_IMAGE_RT               0x00121002UL

#define GL_READ_WRITE                   0x88BA					/* from desktop GL */

/* Stipple */
#define GL_POLYGON                      0x0009					/* from desktop GL */

/* Color */
#define GL_COLOR_0_RT                   0x00000000UL
#define GL_COLOR_1_RT                   0x00000001UL

/* Enable/Disable/IsEnabled */
#define GL_LINE_STIPPLE_RT              0x0B24					/* from desktop GL */
#define GL_POLYGON_STIPPLE_RT           0x0B42					/* from desktop GL */
#define GL_POLYGON_SMOOTH_RT            0x0B41					/* from desktop GL */
#define GL_ALPHAMAP_RT                  0x00122001UL
#define GL_TRANSPARENT_RT               0x00122002UL

/* LineMode/GetLineMode */
#define GL_LINE_JOIN_RT                 0x00123002UL
#define GL_LINE_ALPHA_RT                0x00123003UL
#define GL_LINE_SMOOTH_RT               0x00123004UL
#define GL_LINE_FATMODE_RT              0x00123006UL
#define GL_LINE_PRECLIP_RT              0x00123008UL
#define GL_LINE_WIDTH_RT                0x00123009UL
#define GL_FLAT_RT                      0x00123101UL
#define GL_COLOR_RT                     0x00123102UL
#define GL_ALPHA_RT                     0x00123103UL
#define GL_FAST_RT                      0x00123104UL
#define GL_FINE_RT                      0x00123105UL

/* Get */
#define GL_ALPHAMAP_IMAGE_BINDING_RT    0x00124001UL
/* #define GL_COLOR_0_RT                   0x00000000UL */
/* #define GL_COLOR_1_RT                   0x00000001UL */
#define GL_COLOR_IMAGE_BINDING_RT       0x00124002UL
#define GL_CURRENT_RASTER_POSITION_RT   0x00124003UL
#define GL_LINE_STIPPLE_POINTER_RT      0x00124004UL
#define GL_LINE_STIPPLE_WIDTH_RT        0x00124006UL
#define GL_LINE_STIPPLE_HEIGHT_RT       0x00124007UL
#define GL_LINE_STIPPLE_FORMAT_RT       0x00124008UL
#define GL_POLYGON_STIPPLE_POINTER_RT   0x00124009UL
#define GL_POLYGON_STIPPLE_WIDTH_RT     0x0012400BUL
#define GL_POLYGON_STIPPLE_HEIGHT_RT    0x0012400CUL
#define GL_POLYGON_STIPPLE_FORMAT_RT    0x0012400DUL


/* TransparentColor */
#define GL_STRANS_COLOR_RT              0x00125001UL
#define GL_STRANS_BITMAP_RT             0x00125002UL

/* NewList */
#define GL_COMPILE                      0x1300					/* from desktop GL */


/*
	Renesas original API
*/
GL_API void		 GL_APIENTRY glAppendListRT( GLuint list );
GL_API void		 GL_APIENTRY glBindImageRT ( GLenum target, GLuint image );
GL_API void		 GL_APIENTRY glCallListRT( GLuint list );
GL_API void		 GL_APIENTRY glColorRT ( GLenum target, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
GL_API void		 GL_APIENTRY glDeleteImageRT ( GLuint image );
GL_API void		 GL_APIENTRY glDeleteListsRT( GLuint list, GLsizei range );
GL_API void		 GL_APIENTRY glDrawImageRT ( GLint dstX, GLint dstY, GLsizei dstWidth, GLsizei dstHeight, GLint srcX, GLint srcY, GLsizei srcWidth, GLsizei srcHeight );
GL_API void		 GL_APIENTRY glDrawLinebRT ( GLenum linemode, GLint count, GLbyte * vertices );
GL_API void		 GL_APIENTRY glDrawLinesRT ( GLenum linemode, GLint count, GLshort * vertices );
GL_API void		 GL_APIENTRY glDrawLineiRT ( GLenum linemode, GLint count, GLint * vertices );
GL_API void		 GL_APIENTRY glDrawPixelsRT ( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
GL_API void		 GL_APIENTRY glDrawPolygonbRT ( GLint count, GLbyte * vertices );
GL_API void		 GL_APIENTRY glDrawPolygonsRT ( GLint count, GLshort * vertices );
GL_API void		 GL_APIENTRY glDrawPolygoniRT ( GLint count, GLint * vertices );
GL_API void		 GL_APIENTRY glDrawRectbRT ( GLbyte x1, GLbyte y1, GLbyte x2, GLbyte y2 );
GL_API void		 GL_APIENTRY glDrawRectsRT ( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
GL_API void		 GL_APIENTRY glDrawRectiRT ( GLint x1, GLint y1, GLint x2, GLint y2 );
GL_API void		 GL_APIENTRY glEnableRT ( GLenum cap );
GL_API void		 GL_APIENTRY glDisableRT ( GLenum cap );
GL_API void		 GL_APIENTRY glGenImageRT ( GLuint * image );
GL_API GLuint	 GL_APIENTRY glGenListsRT( GLsizei range );
GL_API void		 GL_APIENTRY glGetRT( GLenum pname, GLvoid *params );
GL_API void		 GL_APIENTRY glGetLineModeRT ( GLenum pname, GLvoid *params );
GL_API void		 GL_APIENTRY glGetListSizeRT( GLsizei * size );
GL_API void		 GL_APIENTRY glGetTransparentColorRT( GLenum pname, GLvoid * params );
GL_API void		 GL_APIENTRY glImageDataRT ( GLenum target, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels );
GL_API GLboolean GL_APIENTRY glIsEnabledRT ( GLenum cap );
GL_API GLboolean GL_APIENTRY glIsListRT( GLuint list );
GL_API void		 GL_APIENTRY glLineModeRT ( GLenum pname, const GLvoid *params );
GL_API void		 GL_APIENTRY glListSizeRT( GLsizei size );
GL_API void *	 GL_APIENTRY glMapImageRT ( GLenum target, GLenum access );
GL_API void		 GL_APIENTRY glNewListRT( GLuint list, GLenum mode );
GL_API void		 GL_APIENTRY glEndListRT( void );
GL_API void		 GL_APIENTRY glRasterPos2iRT ( GLint x, GLint y );
GL_API void		 GL_APIENTRY glStippleRT ( GLenum target, GLenum format, GLenum type, GLsizei width, GLsizei height, GLsizei stride, const GLvoid *pattern );
GL_API void		 GL_APIENTRY glTransparentColorRT( GLenum pname, const GLvoid * params );
GL_API GLboolean GL_APIENTRY glUnmapImageRT ( GLenum target );


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


#endif /* #ifndef __glext_RT_h_ */


/*------------------------------------------------------------------------*/
/*(C) 2009-2010 Renesas Electronics Corporation. All rights reserved.     */
/*------------------------------------------------------------------------*/
