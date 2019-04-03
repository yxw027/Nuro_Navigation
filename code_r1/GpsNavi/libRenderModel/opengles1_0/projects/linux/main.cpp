#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#define USING_X11
#ifdef USING_X11
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#endif

#include <stdbool.h>
#include "GLES/gl.h"
#include "GLES/egl.h"

#define SW 800 //320
#define SH 480 //240

#ifdef USING_X11
Display *display     = NULL;
Window  root         = NULL;
Window  win          = NULL;
GC gc                = NULL;
XImage  *img         = NULL;
int     *screan_addr = NULL;
#endif
//buffer的地址
short *gRgb565Screen = NULL;
int gRgbScreenW = SW;
int gRgbScreenH = SH;
unsigned short screen15_1[SW * SH];

#ifdef USING_X11
void InitX11(void) 
{
    int x = 0, y = 0, w = SW, h = SH, depth = 24;
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

    return;
}
#endif



//add by dengxu
#define COLOR_BIT_FMT_RGB_565 16
#define COLOR_BIT_FMT_RGB_888 24
//add by dengxu
#define GET_R565_2_888(c888, c565) do {\
    c888 |= (((c565 << 8) & 0xF80000) | ((c565 << 3) & 0x070000));\
} while (0)

//add by dengxu
#define GET_G565_2_888(c888, c565) do {\
    c888 |= (((c565 << 5) & 0xFC00) | ((c565 >> 1) & 0x0300));\
} while (0)

//add by dengxu
#define GET_B565_2_888(c888, c565) do {\
    c888 |= (((c565 << 3) & 0xF8) | ((c565 >> 2) & 0x07));\
} while (0)

//add by dengxu
void bmp_bit_copy_to888(void *dec, int dec_x, int dec_y, int dec_w, int dec_h,
        void *src, int src_x, int src_y, int src_w, int src_h, 
        int src_fmt, int w, int h)
{
    int n888;
    unsigned short n565;
    unsigned short *pix16 = (unsigned short *)src;
    int *pix24 = (int *)dec;
    int x, y;

    //fprintf(stderr, "dec_x = %d   dec_y = %d   dec_w = %d   dec_h = %d  src_w = %d   src_h = %d\n",
    //        dec_x, dec_y, dec_w, dec_h, src_w, src_h);

    if (src_fmt == COLOR_BIT_FMT_RGB_565)
    {
        //FILE *fp16 = fopen("/home/dx/map.16", "w+");
        //FILE *fp24 = fopen("/home/dx/map.24", "w+");
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                n888 = 0;
                n565 = pix16[(y + src_y) * src_w + (x + src_x)];
                GET_R565_2_888(n888, n565);
                GET_G565_2_888(n888, n565);
                GET_B565_2_888(n888, n565);
                //printf("%#x  ... %#x...\n", n565, n888);
                pix24[(y + dec_y) * dec_w + (x + dec_x)] = n888;

                //fwrite(&n888, 1, 4, fp24);
                //fwrite(&n565, 1, 2, fp16);

            }
        }
        //fclose(fp16);
        //fclose(fp24);

    }
}



void FlushScreen()
{
#ifdef USING_X11
    XShmPutImage(display, win, gc, img, 
            0, 0, 0, 0, gRgbScreenW, gRgbScreenH, True/*False*/);
    //XSync(display, True);
    XFlush(display);
#endif
}

EGLDisplay           eglDisplay       = EGL_NO_DISPLAY;
EGLConfig            eglConfig        = 0;
EGLSurface           eglSurface       = EGL_NO_SURFACE;
EGLContext           eglContext       = EGL_NO_CONTEXT;

int InitOpenGL()
{
    EGLint cfg_attr_list[] = {EGL_BUFFER_SIZE, 32, EGL_NONE};
    NativeWindowType  eglWindow        = 0;
    NativeDisplayType eglNativeDisplay = EGL_DEFAULT_DISPLAY;
    EGLint               iErr             = 0;

#ifdef USING_X11
    eglWindow = (NativeWindowType)win;
    eglNativeDisplay = (NativeDisplayType)display;
    eglDisplay = eglGetDisplay(eglNativeDisplay);
#endif

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
    
    glViewport(0, 0, SW, SH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumf(
            -10.0f, 10.0f, -10.0f, 
            10.0, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //void default3DEnv()
    {
        glEnable(GL_DITHER);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDepthFunc(GL_LEQUAL);
        glShadeModel(GL_SMOOTH);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glFrontFace(GL_CW);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);
        //glEnable(GL_TEXTURE_2D);

        
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }  
    
    return 0;



cleanup:
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ;
    eglTerminate(eglDisplay);
    // More platform specific cleanup here
    return 0;
}

void LocalFlush(void)
{
    glReadPixels(0, 0, 800, 480, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, screen15_1);
    //eglSwapBuffers(eglDisplay, eglSurface);

    bmp_bit_copy_to888(screan_addr, 0, 0, SW, SH,
            screen15_1, 0, 0, SW, SH, 
            COLOR_BIT_FMT_RGB_565, SW, SH);

    FlushScreen();
    return;
}

///////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
#ifdef USING_X11
    InitX11();
#endif
    InitOpenGL();

#if 1 
    float v[12] = {
        -10.0, 10.0, -20.0,
        1.0, -10.0, -20.0,
        10.0, 1.0, -20.0,
    };

    float v1[12] = {
        -10.0 + 30, 10.0, -20.0,
        1.0 + 30, -10.0, -20.0,
        10.0 + 30, 1.0, -20.0,
    };
    
    while (1)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glColor4f(1.0, 1.0, 1.0, 1.0);

        //draw
        glVertexPointer(3, GL_FLOAT, 0, v);
        glDrawArrays(GL_TRIANGLES, 0, 3); 
        
        glColor4f(0.0, 1.0, 1.0, 1.0);
        glVertexPointer(3, GL_FLOAT, 0, v1);
        glDrawArrays(GL_TRIANGLES, 0, 3); 
        LocalFlush();
        usleep(1000000);
    }
#endif

    return 0;
}

