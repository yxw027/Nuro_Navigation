#ifndef __glu_h__
#define __glu_h__

#include <GLES/egl.h>

extern "C"

{
void gluPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f);

void gluPerspectivex(GLfixed fovy, GLfixed aspect, GLfixed n, GLfixed f);

void gluLookAtf(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
      GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz); 

void gluLookAtx(GLfixed eyex, GLfixed eyey, GLfixed eyez, GLfixed centerx,
      GLfixed centery, GLfixed centerz, GLfixed upx, GLfixed upy, GLfixed upz);
}
      
#endif // __glu_h__