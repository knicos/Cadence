/*==============================================================================

Copyright 2003-2004 Pierre-Alain Fayolle, Benjamin Schmitt
Copyright 2007-2008 Oleg Fryazinov

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

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mainapp.h"
#include "hf_key.h"

#include "main.h"
#include "hf_draw.h"
#include "hf_error.h"


// GLOBAL VARIABLES
extern int wireAndSolid;



// PROCEDURES

void OutputString(int x, int y, char *string) {
	int len, i;
	glColor3f(0, 0, 0);
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	// for (i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]); GLUT_BITMAP_TIMES_ROMAN_10 
	for (i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
}


void displaytext(void) {
	glDisable(GL_LIGHTING); 
	glViewport(0, 0, width, 70);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, width, 0, 70);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// OutputString(5, 10, "F1      : Toggle help");
	OutputString(5, 10, "ESC     : Quit");
	OutputString(5, 20, "X,Y,Z   : Rotate around an axis");
	OutputString(5, 30, "S / s   : Zoom In/Out");
	OutputString(5, 40, "W       : Toggle wireframe/solid");
	OutputString(5, 50, "F       : Toggle flat/smooth");
	OutputString(5, 60,"N       : Show/Hide normals");
	OutputString(195, 60,"V       : Toggle vertex/face normals");
	OutputString(195, 50,"A       : Show/Hide axis");
	OutputString(195, 40,"Arrows  : Translations");
	OutputString(195, 30,"Mouse  button 1: Trackball");
	OutputString(195, 20,"Mouse  button 2: Translation");
	OutputString(195, 10,"Mouse  button 3: Zoom");
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
}


void keyboard(unsigned char key, int x, int y)
{
	int m=0;	//boolean for mesh update;
	int l=0;	//boolean for light update

	switch (key) {
		//**************Rotation of the Model***************
  case 'x':glRotatef(-15.,1.0,0.0,0.0);
	  break;
  case 'X':
	  glRotatef(15.,1.0,0.0,0.0);
	  break;
  case 'y':glRotatef(-15.,0.0,1.0,0.0);
	  break;
  case 'Y':
	  glRotatef(15.,0.0,1.0,0.0);
	  break;
  case 'z':  
	  glRotatef(-15.,0.0,0.0,1.0);
	  break;
  case 'Z':
	  glRotatef(15.,0.0,0.0,1.0);
	  break;

	  // for scaling the model
  case 's':  
	  glScalef(0.95,0.95,0.95);
	  break;
  case 'S':
	  glScalef(1.05,1.05,1.05);
	  break;

	  // Light
  case 'l':
  case 'L':
	  HF_LIGHT_TYPE=(1+ HF_LIGHT_TYPE)%2;l=1;break; 
	  break;

	  // Wireframe or surface or both 
  case 'w':
  case 'W':
	  HF_FACE_LINE_FILL=(1+HF_FACE_LINE_FILL)%3;
	  m=1;
	  break;

	  // shading
  case 'f':
  case 'F':
	  HF_FLAT_SMOOTH=(1+HF_FLAT_SMOOTH)%2;m=1;  //should become %3 for the offset
	  break;

	  // normal
  case 'n':
  case 'N':
	  HF_SHOW_NORMAL=(1+HF_SHOW_NORMAL)%2;
	  break;

  case 'v':
  case 'V':
	  HF_VERTEX_NORMAL = (1+HF_VERTEX_NORMAL)%2;
	  {
		  glDeleteLists(hfObject, 1);
		  hfObject = glGenLists (1);
		  GL_CALL(glNewList(hfObject, GL_COMPILE),;);
		  triangle();
		  GL_CALL(glEndList(),;);
		  m = 1;
	  }
	  break;

  case 'a':
  case 'A':
	  if(HF_SHOW_AXES){
		  glDeleteLists(hfAxes,3);
		  HF_SHOW_AXES=0;
	  }
	  else{
		  hfAxes = glGenLists (3);
		  glNewList(hfAxes, GL_COMPILE); 
		  CreateAxes();
		  glEndList();
		  HF_SHOW_AXES=1;
	  }
	  break;

	  // escape
  case 27:
	  exit(0); // quit
	  break;
	}

	if(m)	SetMeshProperties();
	if(l)	SetLight();

	// if we did compute the normals (HF_NORMAL)
	// and we want to show them
	// and they were not displayed
	if((HF_NORMAL==1)&&(HF_SHOW_NORMAL==1)&&(first_normal_show==0)){
		hfNormals = glGenLists (2);
		glNewList(hfNormals, GL_COMPILE); 
		DrawNormal();
		glEndList();
		first_normal_show=1;
	}
	glutPostRedisplay();
}


// for moving the object with the arrow keys
void SpecialKeyboard(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_LEFT: glTranslatef(-1.0,0.0,0.0); break;
	case GLUT_KEY_RIGHT: glTranslatef(1.0,0.0,0.0); break;
	case GLUT_KEY_UP: glTranslatef(0.0,1.0,0.0); break;
	case GLUT_KEY_DOWN: glTranslatef(0.0,-1.0,0.0); break;
	}
	glutPostRedisplay();
}
