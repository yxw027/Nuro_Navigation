// ==========================================================================
//
// gl.cpp	EGL Client API entry points
//
// --------------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
//
// --------------------------------------------------------------------------
//
// Copyright (c) 2004, Hans-Martin Will. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
//	 *  Redistributions of source code must retain the above copyright
// 		notice, this list of conditions and the following disclaimer. 
//   *	Redistributions in binary form must reproduce the above copyright
// 		notice, this list of conditions and the following disclaimer in the 
// 		documentation and/or other materials provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//
// ==========================================================================


#include "stdafx.h"
#include "GLES/gl.h"
#include "GLES/egl.h"
#include "Context.h"
#include "Rasterizer.h"
#include "Config.h"
#include "Surface.h"
#include <stdio.h>
//#include "minimal.h"

using namespace EGL;


// version numbers
#define EGL_VERSION_MAJOR 1
#define EGL_VERSION_MINOR 0

static EGLint savedError = EGL_SUCCESS;
static NativeWindowType currentWindow = 0;

static void eglRecordError(EGLint error) 
	// Save an error code for the current thread
	//
	// error		-		The error code to be recorded in thread local storage
{
//	extern DWORD s_TlsIndexError;

//	TlsSetValue(s_TlsIndexError, reinterpret_cast<void *>(error));
  savedError = error;
}


static NativeDisplayType GetNativeDisplay (EGLDisplay display) {
	return reinterpret_cast<NativeDisplayType>(display);
}

GLAPI EGLint APIENTRY eglGetError (void) {
//	extern DWORD s_TlsIndexError;

//	return reinterpret_cast<EGLint> (TlsGetValue(s_TlsIndexError));
  return savedError;
}

GLAPI EGLDisplay APIENTRY eglGetDisplay (NativeDisplayType display) {
	return reinterpret_cast<EGLDisplay>(display);
}

GLAPI EGLBoolean APIENTRY eglInitialize (EGLDisplay dpy, EGLint *major, EGLint *minor) {

	if (major != 0) {
		*major = EGL_VERSION_MAJOR;
	}

	if (minor != 0) {
		*minor = EGL_VERSION_MINOR;
	}

	eglRecordError(EGL_SUCCESS);

	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglTerminate (EGLDisplay dpy) {
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI const char * APIENTRY eglQueryString (EGLDisplay dpy, EGLint name) {

	eglRecordError(EGL_SUCCESS);

	switch (name) {
	case EGL_VENDOR:
		return EGL_CONFIG_VENDOR;

	case EGL_VERSION:
		return EGL_VERSION_NUMBER;

	case EGL_EXTENSIONS:
		return "";

	default:
		eglRecordError(EGL_BAD_PARAMETER);
		return 0;
	}
}

GLAPI EGLBoolean APIENTRY eglSaveSurfaceHM(EGLSurface surface, const char * filename) {
	return surface->Save(filename);
}

#define FunctionEntry(s) { #s, s }

static const struct {
	const char * name;
	void * ptr;

} FunctionTable[] = {
	/* OES_query_matrix */
	FunctionEntry((void *)glQueryMatrixxOES),

	/* OES_point_size_array */
	FunctionEntry((void *)glPointSizePointerOES),

	FunctionEntry((void *)eglSaveSurfaceHM)
};


GLAPI void (* APIENTRY eglGetProcAddress (const char *procname))() {

	eglRecordError(EGL_SUCCESS);

	if (!procname) {
		return 0;
	}

	size_t functions = sizeof(FunctionTable) / sizeof(FunctionTable[0]);

	for (size_t index = 0; index < functions; ++index) {
		if (!strcmp(FunctionTable[index].name, procname))
			return (void (APIENTRY *)(void)) FunctionTable[index].ptr;
	}

	return 0;
}


GLAPI EGLBoolean APIENTRY eglGetConfigs (EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config) {
	eglRecordError(EGL_SUCCESS);
	return Config::GetConfigs(configs, config_size, num_config);
}

GLAPI EGLBoolean APIENTRY eglChooseConfig (EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config) {
	eglRecordError(EGL_SUCCESS);
	return Config::ChooseConfig(attrib_list, configs, config_size, num_config);
}

GLAPI EGLBoolean APIENTRY eglGetConfigAttrib (EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value) {
	*value = config->GetConfigAttrib(attribute);
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}


GLAPI EGLSurface APIENTRY eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list, EGLint width, EGLint height) {
/*
	RECT rect;

	if (!GetClientRect(window, &rect)) {
		eglRecordError(EGL_BAD_NATIVE_WINDOW);
		return EGL_NO_SURFACE;
	}
	
	Config surfaceConfig(*config);
	surfaceConfig.SetConfigAttrib(EGL_SURFACE_TYPE, EGL_WINDOW_BIT);
	surfaceConfig.SetConfigAttrib(EGL_WIDTH, rect.right - rect.left);
	surfaceConfig.SetConfigAttrib(EGL_HEIGHT, rect.bottom - rect.top);
	
	HDC dc = GetWindowDC(window);
	if (!dc) {
		eglRecordError(EGL_BAD_NATIVE_WINDOW);
		return EGL_NO_SURFACE;
	}

	EGLSurface new_surface = new EGL::Surface(surfaceConfig, dc);
	ReleaseDC(window, dc);
*/
	Config surfaceConfig(*config);
	surfaceConfig.SetConfigAttrib(EGL_SURFACE_TYPE, EGL_WINDOW_BIT);
	//surfaceConfig.SetConfigAttrib(EGL_WIDTH, 320);
	//surfaceConfig.SetConfigAttrib(EGL_HEIGHT, 240);
	
    	surfaceConfig.SetConfigAttrib(EGL_WIDTH, width);
	surfaceConfig.SetConfigAttrib(EGL_HEIGHT, height);

	EGLSurface new_surface = new EGL::Surface(surfaceConfig);

    currentWindow = window;

	eglRecordError(EGL_SUCCESS);

	return (new_surface); 
}

GLAPI EGLSurface APIENTRY eglCreatePixmapSurface (EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint *attrib_list) {
	// Cannot support rendering to arbitrary native surfaces; use pbuffer surface and eglCopySurfaces instead
	eglRecordError(EGL_BAD_MATCH);
	return EGL_NO_SURFACE;
}

GLAPI EGLSurface APIENTRY eglCreatePbufferSurface (EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list) {
	static const EGLint validAttributes[] = {
		EGL_WIDTH,
		EGL_HEIGHT,
		EGL_NONE
	};

	Config surfaceConfig(*config, attrib_list, validAttributes);
	surfaceConfig.SetConfigAttrib(EGL_SURFACE_TYPE, EGL_PBUFFER_BIT);
	eglRecordError(EGL_SUCCESS);
	return new EGL::Surface(surfaceConfig/*, GetNativeDisplay(dpy)*/);
}

GLAPI EGLBoolean APIENTRY eglDestroySurface (EGLDisplay dpy, EGLSurface surface) {
	surface->Dispose();
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglQuerySurface (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value) {
	*value = surface->GetConfig()->GetConfigAttrib(attribute);
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLContext APIENTRY eglCreateContext (EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint *attrib_list) {
	eglRecordError(EGL_SUCCESS);
	return new Context(*config);
}

GLAPI EGLBoolean APIENTRY eglDestroyContext (EGLDisplay dpy, EGLContext ctx) {
	ctx->Dispose();
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglMakeCurrent (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) {

	Context::SetCurrentContext(ctx);

	if (ctx) {
		ctx->SetDrawSurface(draw);
		ctx->SetReadSurface(read);
	}

	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;

}

GLAPI EGLContext APIENTRY eglGetCurrentContext (void) {
	eglRecordError(EGL_SUCCESS);
	return Context::GetCurrentContext();
}

GLAPI EGLSurface APIENTRY eglGetCurrentSurface (EGLint readdraw) {
	EGLContext currentContext = eglGetCurrentContext();
	eglRecordError(EGL_SUCCESS);

	if (currentContext != 0) {
		switch (readdraw) {
		case EGL_DRAW:
			return currentContext->GetDrawSurface();

		case EGL_READ:
			return currentContext->GetReadSurface();

		default: 
			return 0;
		}
	} else {
		return 0;
	}
}

GLAPI EGLDisplay APIENTRY eglGetCurrentDisplay (void) {
	eglRecordError(EGL_SUCCESS);
	return 0;
}

GLAPI EGLBoolean APIENTRY eglQueryContext (EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value) {
	*value = ctx->GetConfig()->GetConfigAttrib(attribute);
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglWaitGL (void) {
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglWaitNative (EGLint engine) {
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

//unsigned short gp2x_screen15[800 * 480 * sizeof(U16)];

GLAPI EGLBoolean APIENTRY eglSwapBuffers (EGLDisplay dpy, EGLSurface draw) {

	Context::GetCurrentContext()->Flush();

    //memcpy(gp2x_screen15, draw->GetColorBuffer(), 800 * 480 * sizeof(U16));
    
    //memset(gp2x_screen15, 0xcc, 320 * 240 * sizeof(U16));
    //
    //gp2x_video_flip();
    //printf("func:%s,at %s:%d\n", __func__, __FILE__, __LINE__);

	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglCopyBuffers (EGLDisplay dpy, EGLSurface surface, NativePixmapType target) {

	if (!target) {
		return EGL_BAD_NATIVE_PIXMAP;
	}

	Context::GetCurrentContext()->Flush();
/*
	HDC nativeDisplay = GetNativeDisplay(dpy);
	HDC memoryDC = surface->GetMemoryDC();
	HDC targetDC = CreateCompatibleDC(nativeDisplay);;

	if (memoryDC == INVALID_HANDLE_VALUE) {
		memoryDC = CreateCompatibleDC(nativeDisplay);
	}

	HBITMAP old_memmap = (HBITMAP) SelectObject(memoryDC, surface->GetBitmap());
	HBITMAP old_targetmap = (HBITMAP)SelectObject(targetDC, target);

	BOOL result = BitBlt(targetDC, 0, 0, surface->GetWidth(), surface->GetHeight(), memoryDC, 0, 0, SRCCOPY);

	SelectObject(memoryDC, old_memmap);
	SelectObject(targetDC, old_targetmap);

	if (! result)
		return EGL_BAD_MATCH;

	if (memoryDC != surface->GetMemoryDC()) {
		DeleteDC(memoryDC);
	}

	DeleteDC(targetDC);
*/
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglSurfaceAttrib (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value) {
	return EGL_FALSE;
}

GLAPI EGLBoolean APIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer) {
	return EGL_FALSE;
}

GLAPI EGLBoolean APIENTRY eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer) {
	return EGL_FALSE;
}

GLAPI EGLBoolean APIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval) {
	return EGL_FALSE;
}

