#ifndef __APPGLOBAL_H__
#define __APPGLOBAL_H__

#include <GLES/egltypes.h>
#include <GLES/gl.h>

#define HARD_SIZE 3*732

struct TAppGlobal
{
    TAppGlobal(): angle(30.0f) {};

    // OpenGL variables
    EGLDisplay			g_display;				// EGL display
    EGLSurface			g_surface;				// EGL rendering surface
    EGLContext			g_context;				// EGL rendering context

    GLuint buffers[3];

    GLfixed egl_vertices[HARD_SIZE][3];
    GLfixed egl_normals[HARD_SIZE][3];
    GLfixed egl_textures[HARD_SIZE][2];

    float angle;
};

#endif