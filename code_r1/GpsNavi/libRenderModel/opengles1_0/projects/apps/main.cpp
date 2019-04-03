#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<X11/keysym.h>
#include<X11/Xlib.h>
#include<X11/extensions/XShm.h>
#include<X11/Xutil.h>

#include"GLES/gl.h"
#include"GLES/egl.h"

Display *display = NULL;
Window root = NULL;
Window win = NULL;
GC gc = NULL;
XImage *img = NULL;
int *screan_addr = NULL;

//buffer的地址
short *gRgb565Screen = NULL;
int gRgbScreenW = 800;
int gRgbScreenH = 480;

extern unsigned short gp2x_screen15[320 * 240];

void FlushScreen()
{
    XShmPutImage(display, win, gc, img, 
            0, 0, 0, 0, gRgbScreenW, gRgbScreenH, True/*False*/);
    //XSync(display, True);
    XFlush(display);
}

EGLDisplay           eglDisplay       = EGL_NO_DISPLAY;
EGLConfig            eglConfig        = 0;
EGLSurface           eglSurface       = EGL_NO_SURFACE;
EGLContext           eglContext       = EGL_NO_CONTEXT;

int InitOpenGL()
{
    EGLint cfg_attr_list[] = {EGL_BUFFER_SIZE, 16, EGL_NONE};
    NativeWindowType  eglWindow        = 0;
    NativeDisplayType eglNativeDisplay = EGL_DEFAULT_DISPLAY;
    EGLint               iErr             = 0;

    eglWindow = (NativeWindowType)win;
    eglNativeDisplay = (NativeDisplayType)display;
    eglDisplay = eglGetDisplay(eglNativeDisplay);

    EGLint iMajorVersion, iMinorVersion;
    if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
    {
        goto cleanup;
    }

    int iConfigs;
    if(!eglChooseConfig(eglDisplay, cfg_attr_list, &eglConfig, 1, &iConfigs) ||
            (iConfigs != 1))
    {
        goto cleanup;
    }


    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, NULL);

    if(eglSurface == EGL_NO_SURFACE)
    {
        goto cleanup;
    }

    eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);

    if( EGL_NO_CONTEXT == eglContext )
    {
        goto cleanup;
    }

    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    if((iErr = eglGetError()) != EGL_SUCCESS)
    {
        printf("eglMakeCurrent failed (%d).\n", iErr);
        goto cleanup;
    }

cleanup:;
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ;
        eglTerminate(eglDisplay);
        // More platform specific cleanup here
        return 0;
}

int main(int argc, char **argv)
{
    int x = 0, y = 0, w = 320, h = 240, depth = 24;
    Visual *visual;
    Visual *def_visual;

    int screen;

    display = XOpenDisplay(NULL);
    screen = DefaultScreen(display);


    XVisualInfo vi;

    Status st = XMatchVisualInfo(display, screen, depth, DirectColor, &vi);
    printf("XMatchVisualInfo st = %d\n", st);

    def_visual = DefaultVisual(display, screen);
    visual = vi.visual;// = def_visual;

    root = RootWindow(display, screen);

    Colormap xcolormap = XCreateColormap(display, root, visual, AllocNone);

    XSetWindowAttributes swa;
    swa.background_pixel = 0;
    swa.border_pixel = 0;
    swa.colormap = xcolormap;

    win = XCreateWindow(display, root,
            0, 0, w, h, 0, depth,
            InputOutput, visual,
            CWBackPixel | CWBorderPixel
            | CWColormap, &swa);

    XGCValues gcv;

    gcv.graphics_exposures = False;
    gc = XCreateGC(display, win,
            GCGraphicsExposures, &gcv);

    XSetWindowColormap(display, win, xcolormap);
    //XSetWindowColormap(GFX_Display, win, xcolormap);

    {
        Screen *xscreen;
        XSetWindowAttributes a;
        xscreen = ScreenOfDisplay(display, screen);
        a.backing_store = DoesBackingStore(xscreen);
    }

    XMapWindow(display, win);

    {
        //必须是在堆中 或者静态存储区的。。。
        static XShmSegmentInfo shminfo;
        shminfo.shmid = shmget(IPC_PRIVATE, w * h * 4,
                IPC_CREAT | 0777);
        shminfo.shmaddr = (char *)shmat(shminfo.shmid, 0, 0);
        shminfo.readOnly = False;
        int shm_error = False;

        XShmAttach(display, &shminfo);

        if (shm_error)
            shmdt(shminfo.shmaddr);

        shmctl(shminfo.shmid, IPC_RMID, NULL);

        img = XShmCreateImage(display, visual, depth, ZPixmap,
                shminfo.shmaddr, &shminfo, w, h);

        screan_addr = (int *)shminfo.shmaddr;
        gRgb565Screen = (short *)screan_addr;
    }


    //监听事件
    XSelectInput(display, win, /* ExposureMask|KeyPressMask|StructureNotifyMask| */
            ExposureMask|PointerMotionMask|ButtonPressMask|ButtonReleaseMask);

    memset(screan_addr, 0xff, w * h * 4);
    gRgbScreenW = w;
    gRgbScreenH = h;

# if 1 
    //for test
    InitOpenGL();

    while (1)
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        eglSwapBuffers(eglDisplay, eglSurface);
        memcpy(screan_addr, gp2x_screen15, sizeof (gp2x_screen15));
        FlushScreen();
        usleep(11111);
    }
#endif

# if 1
    //for test
    while (1) 
    {   
        int ret = XShmPutImage(display, win, gc, img, x, y, x, y, w, h, False);

        //int ret = XPutImage(display, win, gc, img, x, y, x, y, w, h);

        //XFlush(display);
        //XSync(display, False);
        FlushScreen();
        usleep(11111);
    }
#endif


    return 0;
}

