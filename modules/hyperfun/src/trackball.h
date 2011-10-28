


#ifndef _TRACKBALL_H_
#define _TRACKBALL_H_

/* 
 *  Simple trackball-like motion adapted (ripped off) from projtex.c
 *  (written by David Yu and David Blythe).  See the SIGGRAPH '96
 *  Advanced OpenGL course notes.
 *
 *
 *  Usage:
 *  
 *  o  call reshape() from the reshape callback
 *  o  call matrix() to get the trackball matrix rotation+tranlation
 *  
 *  o  call startRotation() to begin trackball rotational movememt
 *  o  call startTranslation() to begin trackball translational movememt
 *  o  call startZoom() to begin trackball zoom movememt (e.g. z transl)
 *  o  call stopMotion() to stop trackball movememt
 *
 *  o  call motion() from the motion callback
 *  o  call animate(GL_TRUE) if you want the trackball to continue 
 *     spinning after a stopRotation call
 *  o  call animate(GL_FALSE) if you want the trackball to stop 
 *     spinning after a stopRotation call
 *
 *  Typical setup:
 *
 *
    Trackball* tb = NULL;

    void
    init()
    {
      int main_window_id = glut
      glutReshapeFunc(reshape);
      glutDisplayFunc(display);
      glutMouseFunc(mouse);
      glutMotionFunc(motion);

      Trackball* tb = new Trackball(main_window_id);
      tb->animate(GL_TRUE);

      Trackball* tb2 = new Trackball(main_window_id);
      tb2->compose(&*tb->curquat));
      tb2->animate(GL_TRUE);

      tb->setCurrent();
      . . .
    }

    void
    reshape(int width, int height)
    {
      tb->reshape(width, height);
      . . .
    }

    void
    drawObject()
    {
      glPushMatrix();
      tb2->matrix();
      . . . draw the object in the scene . . .
      glPopMatrix();
    }

    void
    display()
    {
      glPushMatrix();
      tb->matrix();
      . . . draw the scene . . .

      drawObject();      

      glPopMatrix();
    }

    void
    mouse(int button, int state, int x, int y)
    {
      if (state==GLUT_DOWN) {
        if (button==GLUT_LEFT_BUTTON) {
	  Trackball::getCurrent()->startRotation(x, y);
	}
        if (button==GLUT_MIDDLE_BUTTON) {
	  Trackball::getCurrent()->startTranslation(x, y);
	}
        if (button==GLUT_RIGHT_BUTTON) {
	  Trackball::getCurrent()->startZoom(x, y);
	}
      } else {
        Trackball::getCurrent()->stopMotion();
      }
      . . .
      glutPostRedisplay();
    }

    void
    motion(int x, int y)
    {
      Trackball::getCurrent()->motion(x, y);
      . . .
      glutPostRedisplay();
    }

    int
    main(int argc, char** argv)
    {
      . . .
      init();
      . . .
    }


 *
 *
 history: 
   08/98 : EF creation from glut/progs/demos/bounce/trackball
   09/98 : EF add translations(+"zoom") handling
   10/98 : Xadec bug fix + extension (windowId and reshape handling)
           + reInit() func
   02/99 : EF remove "static" reshape
   03/99 : Xadec add winList to handle multiple windows
   03/99 : EF add default window_id
 *
 * */

#include "utils.h"

#include <GL/glut.h>

#ifndef TRACKBALL_CPP__
extern void tbAnimate(void);
#endif
#ifdef TRACKBALL_CPP__
static void tbAnimate(void);
#endif

#if _MSC_VER >= 1000
#      pragma warning( disable : 4786 )
#endif

class Trackball {
public:
  // pb because GLUT_LEFT=0, GLUT_MIDDLE=1, GLUT_RIGHT=2 -> peut pas "|"
  Trackball(const int window_id = -1,
	    const float _translSensitivity=4.,
	    const float _trackballSz=0.8);
  virtual ~Trackball();
  void   setCurrent();
  void setGlutIdle(void (*myIdle)(void)) { _previousIdle = myIdle; }
  static Trackball* getCurrent(int window_id = -1);

  void setSize(const float _trackballSz);
  void reInit();
  void reshape(int width, int height);

  void matrix();
  void inv_matrix();

  void startRotation(int x, int y);
  void startTranslation(int x, int y);
  void startZoom(int x, int y);
  void stopMotion();
  void motion(int x, int y);

  void animate(GLboolean animate);

  void compose(quaternion *);
  void worldToLocal(float* src, float* dest) const; 
  void localToWorld(float* src, float* dest) const; 
  // gives a quaternion to transform the transl/zoom
  quaternion curquat;
  vect3f transl;
  quaternion* xfm;
  float translSensitivity;

  GLfloat m[4][4];
private:
  void  buildQuaternion(quaternion& q, float p1x, float p1y,
		       float p2x, float p2y);
  float projectToSphere(float x, float y);

  float tbSz, tbSz2;
  float minSz, maxSz;
  int beginx, beginy;

  quaternion lastquat;
  GLuint    _lasttime;
  GLboolean _wantanim;
  GLboolean _animate;
  void (*_previousIdle)(void);

  GLuint    _width;
  GLuint    _height;
  int       window_id;

  void stepRotation();
  void startMotion(int,int);
  virtual void composedTranslMotion(int,int);
  virtual void composedZoomMotion(int,int);
  virtual void composedRotMotion(int,int);
  virtual void rotMotion(int,int);
  virtual void translMotion(int,int);
  virtual void zoomMotion(int,int);
 
  void (Trackball::*_currMotion)(int,int);

  friend void tbAnimate();
};


inline void 
Trackball::compose(quaternion * q)
{
  xfm = q;
}

inline void
Trackball::worldToLocal(float* src, float* dest) const 
{
  vect3f* d = (vect3f*) dest;
  if (src!=dest) {
    *d = src;
  }
  *d -= transl;
  curquat.image(dest,dest);
}

inline void
Trackball::localToWorld(float* src, float* dest) const
{
  vect3f* d = (vect3f*) dest;
  if (src!=dest) {
    *d = src;
  }
  curquat.source(dest,dest);
  *d += transl;
}

inline void
Trackball::matrix()
{
  curquat.rotmatrix(m);
  m[3][0] = transl[0];
  m[3][1] = transl[1];
  m[3][2] = transl[2];
  glMultMatrixf(&m[0][0]);
}


inline void
Trackball::inv_matrix()
{
  quaternion tmp(curquat);
  tmp[3] = -tmp[3];
  tmp.rotmatrix(m);
  vect3f tmpv;
  tmp.image(transl, tmpv);
  m[3][0] = -tmpv[0];
  m[3][1] = -tmpv[1];
  m[3][2] = -tmpv[2];
  glMultMatrixf(&m[0][0]);
}
  

inline void
Trackball::stepRotation()
{
  curquat += lastquat;
}

inline float
Trackball::projectToSphere(float x, float y)
{
  float d = x*x + y*y;
  if (d < minSz) {
    // Inside sphere
    return sqrt(tbSz2 - d);
  } else {
    return maxSz/sqrt(d);
  }
}

inline void
Trackball::buildQuaternion(quaternion& q, 
			   float p1x, float p1y,
			   float p2x, float p2y)
{
  if ((p1x==p2x) && (p1y==p2y)) {
    q.nullRot();
    return;
  }
  // z-coords of P1 and P2 on the deformed sphere
  vect3f p1(p1x, p1y, projectToSphere(p1x, p1y));
  vect3f p2(p2x, p2y, projectToSphere(p2x, p2y));
  
  // Axis of rotation
  vect3f axe; 
  cross(p2,p1, axe); 
  
  // sinus of angle of rotation
  p1 -= p2;
  float sina = length(p1) / (2.0 * tbSz);
  
  // avoid pb with "out-of-control" value
  if (sina>1.0) {
    sina = 1.0;
  } else if (sina<-1.0) {
    sina = -1.0;
  }
  q.fromAxis(axe, sina, sqrt(1-sina*sina));
}

inline void
Trackball::motion(int x, int y)
{
  if (_currMotion!=NULL) {
    (this->*_currMotion)(x,y);
    beginx = x;
    beginy = y;
  }
}

inline  void
Trackball::reshape(int width, int height)
{
  _width  = width;
  _height = height;
}

inline void
Trackball::startMotion(int x, int y)
{
  _animate = GL_FALSE;
  beginx = x;
  beginy = y;
  reshape(glutGet((GLenum)GLUT_WINDOW_WIDTH),glutGet((GLenum)GLUT_WINDOW_HEIGHT));
}

inline void
Trackball::startRotation(int x, int y)
{  
  if (xfm==NULL) {
    _currMotion = &Trackball::rotMotion;
  } else {
    _currMotion = &Trackball::composedRotMotion;
  }
  _lasttime = glutGet((GLenum)GLUT_ELAPSED_TIME);
  startMotion(x,y);
}

inline void
Trackball::startTranslation(int x, int y)
{
  if (xfm==NULL) {
    _currMotion = &Trackball::translMotion;
  } else {
    _currMotion = &Trackball::composedTranslMotion;
  }
  startMotion(x,y);
}

inline void 
Trackball::startZoom(int x, int y)
{
  if (xfm==NULL) {
    _currMotion = &Trackball::zoomMotion;
  } else {
    _currMotion = &Trackball::composedZoomMotion;
  }
  startMotion(x,y);
}

inline void
Trackball::animate(GLboolean animate)
{
  _wantanim = animate;
}

inline void
Trackball::setSize(const float _trackballSz)
{
  tbSz = _trackballSz;
  tbSz2 = tbSz*tbSz;
  maxSz = minSz = tbSz2/2.0;
}

inline void 
Trackball::reInit()
{
  curquat.nullRot();
  transl = 0;
}

#endif

