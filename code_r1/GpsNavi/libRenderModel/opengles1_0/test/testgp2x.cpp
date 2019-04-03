// test.cpp : Defines the entry point for the application.
//

#include "minimal.h"
#include <memory.h>
#include <stdio.h>

extern "C" bool InitOpenGL();
extern "C" void PaintProc();

extern "C" void gp2x_sound_frame(void *blah, void *bufferg, int samples) { }

static const unsigned char bmpheader[54] =
{
  66, 77, 54, 132, 3, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0,
  0, 0, 64, 1, 0, 0, 240, 0, 0, 0, 1, 0, 24, 0, 0, 0,
  0, 0, 0, 132, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0
};

void savebuffer(char *filename)
{
  FILE *fp;
  int x, y;
  unsigned short pixel;
  unsigned char r, g, b;

  fp = fopen(filename, "wb");
  if (!fp)
  {
    printf("Failed to open file '%s' for writing\n", filename);
    return;
  }
  fwrite(bmpheader, sizeof(bmpheader), 1, fp);

  for (y = 239; y >= 0; --y)
  {
    for (x = 0; x < 320; ++x)
    {
      pixel = gp2x_screen15[x + (y * 320)];
      r = ((pixel >> 11) & 0x1f) << 3;
      g = ((pixel >> 6) & 0x1f) << 3;
      b = (pixel & 0x1f) << 3;
      fwrite(&b, sizeof(b), 1, fp);
      fwrite(&g, sizeof(g), 1, fp);
      fwrite(&r, sizeof(r), 1, fp);
    }
  }
  fclose(fp);
  sync();
  printf("Screenshot written to file '%s'\n", filename);
}

int main(int argc, char *argv[])
{
  gp2x_init(1000, 16, 44100, 16, 1, 50);

  memset(gp2x_screen15, 0, 320*240*2);
  gp2x_video_flip();
  memset(gp2x_screen15, 0, 320*240*2);
  gp2x_video_flip();

  InitOpenGL();

	unsigned long timerRate = 1000;
	unsigned long frames = 0;
	unsigned long fps = 0;
	unsigned long timeStart = gp2x_timer_read();

  int running = 1;
  while (running)
  {
    unsigned long  pad=gp2x_joystick_read();
    if ((pad & GP2X_L) && (pad & GP2X_R))
      running = 0;

    extern float angle;
	  angle += 0.5f;

    PaintProc();

	++frames;
	 unsigned long timeCurrent = gp2x_timer_read();
	 if (timeCurrent - timeStart >= timerRate)
	 {
	   fps = frames;
	   timeStart = timeCurrent;
	   frames = 0;
	 }

	 printf("fps: %d\n", fps);
    if (pad & GP2X_B)
      savebuffer("screenshot.bmp");
  }

  return 0;
}
