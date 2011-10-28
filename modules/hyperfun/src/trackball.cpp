
#if _MSC_VER >= 1000
#pragma warning (disable : 4786)
#endif


#include <iostream>
#include <math.h>

#include <GL/glut.h>

#include <map>
#define TRACKBALL_CPP__
#include "trackball.h"


// dirty stuff! use of "hidden" static variable to animate the trackball!
static std::map<int,Trackball*> currentTrackballs;

static void
tbAnimate()
{
  static int save_id = glutGetWindow();
  // animate all the trackballs in every window
  std::map<int,Trackball*>::iterator p = currentTrackballs.begin();
  while(p != currentTrackballs.end()) {
#if defined(WIN32)
	int window_id = (*p).first;
#else
	int window_id = p->first;
#endif
	if (window_id<=0) continue;
#if defined(WIN32)
    Trackball* tb = (*p).second;
#else
    Trackball* tb = p->second;
#endif
	if (tb==NULL) continue;
    if (tb->_animate == GL_TRUE) {
      glutSetWindow(window_id);
      tb->stepRotation();
      glutPostRedisplay();
    }
    p++;
  }
  glutSetWindow(save_id);     
}

Trackball::Trackball(const int id,
		     const float _translSensitivity,
		     const float _trackballSz)
  : xfm(NULL), 
    translSensitivity(_translSensitivity), 
    _wantanim(GL_TRUE),
    _animate(GL_FALSE),
    _previousIdle(NULL),
    window_id(id)
{
  if (window_id<0) {
    window_id = glutGetWindow();
  }
  setSize(_trackballSz);
  currentTrackballs[window_id] = this;
}

Trackball::~Trackball()
{
  currentTrackballs.erase(window_id);
}

Trackball* 
Trackball::getCurrent(int id)
{
  if (id>0) {
    return currentTrackballs[id];
  } else {
    return currentTrackballs[glutGetWindow()];
  }
}

void Trackball::setCurrent()
{
  currentTrackballs[window_id] = this;
}

void
Trackball::rotMotion(int x, int y)
{
  buildQuaternion(lastquat,
		  (2.0 * beginx - _width) / _width,
		  (_height - 2.0 * beginy) / _height,
		  (2.0 * x - _width) / _width,
		  (_height - 2.0 * y) / _height
		  );
  _animate = 1;
  _lasttime = glutGet((GLenum)GLUT_ELAPSED_TIME);
  stepRotation();
}

void
Trackball::composedRotMotion(int x, int y)
{
  
  buildQuaternion(lastquat,
		  (2.0 * beginx - _width) / _width,
		  (_height - 2.0 * beginy) / _height,
		  (2.0 * x - _width) / _width,
		  (_height - 2.0 * y) / _height
		  );
  xfm->image(lastquat, lastquat); // transfo of rot. vector
  _animate = 1;
  _lasttime = glutGet((GLenum)GLUT_ELAPSED_TIME);
  stepRotation();
}

void
Trackball::translMotion(int x, int y)
{
  transl[0] += (x-beginx)/translSensitivity;
  transl[1] += (beginy-y)/translSensitivity;
}

void
Trackball::zoomMotion(int x, int y)
{
  transl[2] += (y-beginy)/translSensitivity;
}

void
Trackball::composedTranslMotion(int x, int y)
{
  vect3f tmp((x-beginx)/translSensitivity, 
	     (beginy-y)/translSensitivity,
	     0);
  xfm->image(tmp, tmp);
  transl += tmp;
}

void
Trackball::composedZoomMotion(int x, int y)
{
  vect3f tmp(0,0,(y - beginy)/translSensitivity);
  xfm->image(tmp, tmp);
  transl += tmp;
}

void
Trackball::stopMotion()
{
  if (_wantanim && 
      ((this->_currMotion == static_cast<void (Trackball::*)(int, int)>(&Trackball::rotMotion)) || 
       (this->_currMotion == static_cast<void (Trackball::*)(int, int)>(&Trackball::composedRotMotion)))) {
    if (glutGet((GLenum)GLUT_ELAPSED_TIME) == _lasttime) {
      _animate = GL_TRUE;
      glutIdleFunc(tbAnimate); // CB for the current window
    } else {
      _animate = GL_FALSE;
      glutIdleFunc(_previousIdle);
    }
  }
  _currMotion=NULL;
}

