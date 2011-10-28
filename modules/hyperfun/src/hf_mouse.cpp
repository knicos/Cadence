/*==============================================================================

 Copyright 2003-2004 Pierre-Alain Fayolle, Benjamin Schmitt

 This Work or file is part of the greater total Work, software or group of
 files named HyperFun Polygonizer.

 HyperFun Polygonizer can be redistributed and/or modified under the terms 
 of the CGPL, The Common Good Public License as published by and at CGPL.org
 (http://CGPL.org).  It is released under version 1.0 Beta of the License
 until the 1.0 version is released after which either version 1.0 of the
 License, or (at your option) any later version can be applied.

 THIS WORK, OR SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED (See the
 CGPL, The Common Good Public License for more information.)

 You should have received a copy of the CGPL along with HyperFun Polygonizer;  
 if not, see -  http://CGPL.org to get a copy of the License.

==============================================================================*/



#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include <GL/glut.h>

#include "hf_mouse.h"
#include "trackball.h"


// GLOB VAR
int mousex,mousey;


// PROCEDURES  
  
void my_mouse(int button, int state, int x, int y) {
  mousex=x;mousey=y;
 
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
  // glutPostRedisplay();
}

void my_motion(int x, int y) {
  mousex=x;mousey=y;
  Trackball::getCurrent()->motion(x,y);
  // glutPostRedisplay();
}

void my_passive_motion(int x,int y) {
  mousex=x;mousey=y;
}
