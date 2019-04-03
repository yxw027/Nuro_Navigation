#ifndef __egl_h_
#define __egl_h_

/*
** Copyright 2002-2003 Promoters of the Khronos Group (3Dlabs, ARM Ltd.,
** ATI Technologies, Inc., Discreet, Ericsson Mobile, Imagination
** Technologies Group plc, Motorola, Inc., Nokia, Silicon Graphics, Inc.,
** SK Telecom, and Sun Microsystems).
**
** This document is protected by copyright, and contains information
** proprietary to The Khronos Group. Any copying, adaptation, distribution,
** public performance, or public display of this document without the
** express written consent of the copyright holders is strictly prohibited.
** The receipt or possession of this document does not convey any rights to
** reproduce, disclose, or distribute its contents, or to manufacture, use,
** or sell anything that it may describe, in whole or in part.
*/

/*------------------------------------------------------------------------
 * Platform-independent types.
 *----------------------------------------------------------------------*/
#include "gl.h"

#define EGL_STATIC_LIBRARY

typedef int             EGLint;
typedef int             EGLenum;
typedef unsigned int    EGLBoolean;
typedef int             EGLConfig;
typedef void*           EGLContext;
typedef int             EGLDisplay;
typedef void*           EGLSurface;
typedef unsigned int    EGLClientBuffer;

#if defined(EGL_STATIC_LIBRARY)
#	define EGL_APICALL 
#else
#	if defined(_WIN32) || defined(__VC32__)				/* Win32 */
#		if defined (OPENVG_DLL_EXPORTS)
#			define EGL_APICALL __declspec(dllexport)
#		else
#			define EGL_APICALL __declspec(dllimport)
#		endif
#	elif defined(__APPLE__)
#		define EGL_APICALL extern
#	endif /* defined(_WIN32)... */
#endif /* defined(EGL_STATIC_LIBRARY) */

// mark edited in 2006/06/01 -- Below it's for pixmap format of EGL 1.2
/*
typedef struct
{
	int				format;		// VGImageFormat cast to int
	int				width;
	int				height;
	int				stride;		// in bytes
	void*			data;
} NativePixmap;
*/

typedef EGLint          NativeDisplayType;
typedef void*           NativeWindowType;
//typedef NativePixmap*	NativePixmapType;				// mark edited in 2006/06/01 -- Below it's for pixmap format of EGL 1.2
typedef void*			NativePixmapType;

#define EGL_DEFAULT_DISPLAY ((NativeDisplayType)0)
//#define EGL_DEFAULT_DISPLAY ((NativeDisplayType)1)	// mark edited in 2006/06/01 -- Below it's for EGL 1.2
#define EGL_NO_CONTEXT      ((EGLContext)0)
#define EGL_NO_DISPLAY      ((EGLDisplay)0)
#define EGL_NO_SURFACE      ((EGLSurface)0)

/*------------------------------------------------------------------------
 * EGL Enumerants.
 *----------------------------------------------------------------------*/

/* Versioning and extensions */
#define EGL_VERSION_1_0                1
#define EGL_VERSION_1_1                1
#define EGL_VERSION_1_2                1

/* Boolean */
#define EGL_FALSE                      0
#define EGL_TRUE                       1

/* Errors */
#define EGL_SUCCESS                    0x3000
#define EGL_NOT_INITIALIZED            0x3001
#define EGL_BAD_ACCESS                 0x3002
#define EGL_BAD_ALLOC                  0x3003
#define EGL_BAD_ATTRIBUTE              0x3004
#define EGL_BAD_CONFIG                 0x3005
#define EGL_BAD_CONTEXT                0x3006
#define EGL_BAD_CURRENT_SURFACE        0x3007
#define EGL_BAD_DISPLAY                0x3008
#define EGL_BAD_MATCH                  0x3009
#define EGL_BAD_NATIVE_PIXMAP          0x300A
#define EGL_BAD_NATIVE_WINDOW          0x300B
#define EGL_BAD_PARAMETER              0x300C
#define EGL_BAD_SURFACE                0x300D
#define EGL_CONTEXT_LOST               0x300E
/* 0x300F - 0x301F reserved for additional errors. */


/* Config attributes */
#define EGL_BUFFER_SIZE                0x3020
#define EGL_ALPHA_SIZE                 0x3021
#define EGL_BLUE_SIZE                  0x3022
#define EGL_GREEN_SIZE                 0x3023
#define EGL_RED_SIZE                   0x3024
#define EGL_DEPTH_SIZE                 0x3025
#define EGL_STENCIL_SIZE               0x3026
#define EGL_CONFIG_CAVEAT              0x3027
#define EGL_CONFIG_ID                  0x3028
#define EGL_LEVEL                      0x3029
#define EGL_MAX_PBUFFER_HEIGHT         0x302A
#define EGL_MAX_PBUFFER_PIXELS         0x302B
#define EGL_MAX_PBUFFER_WIDTH          0x302C
#define EGL_NATIVE_RENDERABLE          0x302D
#define EGL_NATIVE_VISUAL_ID           0x302E
#define EGL_NATIVE_VISUAL_TYPE         0x302F
/*#define EGL_PRESERVED_RESOURCES        0x3030*/
#define EGL_SAMPLES                    0x3031
#define EGL_SAMPLE_BUFFERS             0x3032
#define EGL_SURFACE_TYPE               0x3033
#define EGL_TRANSPARENT_TYPE           0x3034
#define EGL_TRANSPARENT_BLUE_VALUE     0x3035
#define EGL_TRANSPARENT_GREEN_VALUE    0x3036
#define EGL_TRANSPARENT_RED_VALUE      0x3037
#define EGL_BIND_TO_TEXTURE_RGB        0x3039
#define EGL_BIND_TO_TEXTURE_RGBA       0x303A
#define EGL_MAX_SWAP_INTERVAL          0x303B
#define EGL_MIN_SWAP_INTERVAL          0x303C
#define EGL_LUMINANCE_SIZE				0x3101
#define EGL_COLOR_BUFFER_TYPE			0x3102
#define EGL_RENDERABLE_TYPE				0x3103
#define EGL_RENDER_BUFFER				0x3104
#define EGL_HORIZONTAL_RESOLUTION		0x3105
#define EGL_VERTICAL_RESOLUTION			0x3106
#define EGL_PIXEL_ASPECT_RATIO			0x3107
#define EGL_SWAP_BEHAVIOR				0x3108
#define EGL_UNKNOWN						0x3109
#define EGL_BUFFER_PRESERVED			0x310a
#define EGL_OPENVG_IMAGE				0x310b
#define EGL_OPENVG_API					0x310c
#define EGL_CONTEXT_CLIENT_TYPE			0x310d
/* \todo [sampo 23/May/05] verify the value of EGL_OPENGL_ES_API? */
#define EGL_OPENGL_ES_API				0x310e
/* \todo [sampo 26/May/05] verify the value of EGL_BUFFER_DESTROYED? */
#define EGL_BUFFER_DESTROYED			0x310f
/* \todo verify the value of these too */
#define EGL_ALPHA_MASK_SIZE				0x3110
#define EGL_ALPHA_FORMAT				0x3111
#define EGL_COLORSPACE					0x3112
#define EGL_ALPHA_FORMAT_NONPRE			0x3113
#define EGL_ALPHA_FORMAT_PRE			0x3114
#define EGL_SINGLE_BUFFER				0x3115
#define EGL_COLORSPACE_sRGB				0x3116
#define EGL_COLORSPACE_LINEAR			0x3117

/* Config attribute and value */
#define EGL_NONE                       0x3038
/* 0x303D - 0x304F reserved for additional config attributes. */

/* Config values */
#define EGL_DONT_CARE                  ((EGLint) -1)
#define EGL_PBUFFER_BIT                0x01
#define EGL_PIXMAP_BIT                 0x02
#define EGL_WINDOW_BIT                 0x04
#define EGL_SLOW_CONFIG                0x3050
#define EGL_NON_CONFORMANT_CONFIG      0x3051
#define EGL_TRANSPARENT_RGB            0x3052
#define EGL_NO_TEXTURE                 0x305C
#define EGL_TEXTURE_RGB                0x305D
#define EGL_TEXTURE_RGBA               0x305E
#define EGL_TEXTURE_2D                 0x305F
#define EGL_OPENVG_BIT					0x01
/* \todo [sampo 19/May/05] verify the value of EGL_OPENGL_ES_BIT */
#define EGL_OPENGL_ES_BIT				0x02
#define EGL_RGB_BUFFER					0x01
#define EGL_LUMINANCE_BUFFER			0x02

#define EGL_DISPLAY_SCALING				10000

/* String names */
#define EGL_VENDOR                     0x3053
#define EGL_VERSION                    0x3054
#define EGL_EXTENSIONS                 0x3055
/* \todo [sampo 11/05/2005] verify the value of EGL_CLIENT_APIS */
#define EGL_CLIENT_APIS					0x3100

/* Surface attributes */
#define EGL_HEIGHT                     0x3056
#define EGL_WIDTH                      0x3057
#define EGL_LARGEST_PBUFFER            0x3058
#define EGL_TEXTURE_FORMAT             0x3080
#define EGL_TEXTURE_TARGET             0x3081
#define EGL_MIPMAP_TEXTURE             0x3082
#define EGL_MIPMAP_LEVEL               0x3083

/* BindTexImage/ReleaseTexImage buffer target */
#define EGL_BACK_BUFFER                0x3084

/* Current surfaces */
#define EGL_DRAW                       0x3059
#define EGL_READ                       0x305A

/* Engines */
#define EGL_CORE_NATIVE_ENGINE         0x305B

/* 0x305C-0x3FFFF reserved for future use */

/*------------------------------------------------------------------------
 * EGL Functions.
 *----------------------------------------------------------------------*/

#if defined (__cplusplus)
extern "C" {
#endif

EGL_APICALL EGLint      APIENTRY eglGetError				(void);

EGL_APICALL EGLDisplay  APIENTRY eglGetDisplay				(NativeDisplayType displayID);
EGL_APICALL EGLBoolean  APIENTRY eglInitialize				(EGLDisplay dpy, EGLint* major, EGLint* minor);
EGL_APICALL EGLBoolean  APIENTRY eglTerminate				(EGLDisplay dpy);

EGL_APICALL const char* APIENTRY eglQueryString				(EGLDisplay dpy, EGLint name);

EGL_APICALL EGLBoolean  APIENTRY eglGetConfigs				(EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config);
EGL_APICALL EGLBoolean  APIENTRY eglChooseConfig			(EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config);
EGL_APICALL EGLBoolean  APIENTRY eglGetConfigAttrib			(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value);

EGL_APICALL EGLSurface  APIENTRY eglCreateWindowSurface		(EGLDisplay dpy, EGLConfig config, NativeWindowType win, const EGLint* attrib_list);
EGL_APICALL EGLSurface  APIENTRY eglCreatePbufferSurface	(EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list);
EGL_APICALL EGLSurface  APIENTRY eglCreatePixmapSurface		(EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint* attrib_list);
EGL_APICALL EGLBoolean  APIENTRY eglDestroySurface			(EGLDisplay dpy, EGLSurface surface);
EGL_APICALL EGLBoolean  APIENTRY eglQuerySurface			(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value);

EGL_APICALL EGLBoolean	APIENTRY eglSurfaceAttrib			(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
EGL_APICALL EGLBoolean	APIENTRY eglBindTexImage			(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGL_APICALL EGLBoolean	APIENTRY eglReleaseTexImage			(EGLDisplay dpy, EGLSurface surface, EGLint buffer);

EGL_APICALL EGLBoolean  APIENTRY eglSwapInterval			(EGLDisplay dpy, EGLint interval);

EGL_APICALL EGLContext	APIENTRY eglCreateContext			(EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint* attrib_list);
EGL_APICALL EGLBoolean  APIENTRY eglDestroyContext			(EGLDisplay dpy, EGLContext ctx);
EGL_APICALL EGLBoolean  APIENTRY eglMakeCurrent				(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);

EGL_APICALL EGLContext  APIENTRY eglGetCurrentContext		(void);
EGL_APICALL EGLSurface  APIENTRY eglGetCurrentSurface		(EGLint readdraw);
EGL_APICALL EGLDisplay  APIENTRY eglGetCurrentDisplay		(void);
EGL_APICALL EGLBoolean  APIENTRY eglQueryContext			(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value);

EGL_APICALL EGLBoolean  APIENTRY eglWaitGL					(void);
EGL_APICALL EGLBoolean  APIENTRY eglWaitNative				(EGLint engine);
EGL_APICALL EGLBoolean  APIENTRY eglSwapBuffers				(EGLDisplay dpy, EGLSurface surface);
EGL_APICALL EGLBoolean  APIENTRY eglCopyBuffers				(EGLDisplay dpy, EGLSurface surface, NativePixmapType target);

EGL_APICALL void (* APIENTRY eglGetProcAddress(const char *procname)) (void);

EGL_APICALL EGLSurface	APIENTRY eglCreatePbufferFromClientBuffer (EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list);
EGL_APICALL EGLBoolean  APIENTRY eglWaitClient				(void);
EGL_APICALL	EGLBoolean	APIENTRY eglBindAPI					(EGLenum api);
EGL_APICALL	EGLenum		APIENTRY eglQueryAPI				(void);

EGL_APICALL	EGLBoolean	APIENTRY eglReleaseThread			(void);

/* functions for use in an OpenVG implementation */
EGL_APICALL void* eglGetCurrentVGContext(void);
EGL_APICALL int   eglIsInUse(void* image);
EGL_APICALL void  eglAcquireMutex(void);
EGL_APICALL void  eglReleaseMutex(void);

/* for OpenGLES extension */
NativeWindowType APIENTRY eglGetBuffers						(EGLDisplay dpy, EGLSurface surface);
NativeWindowType APIENTRY eglGetAlphaBuffers				(EGLDisplay dpy, EGLSurface surface);

#if defined (__cplusplus)
}
#endif

/*----------------------------------------------------------------------*/
#endif /* __egl_h_ */
