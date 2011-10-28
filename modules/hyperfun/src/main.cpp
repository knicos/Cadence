/*==============================================================================

Copyright 2003-2004 Pierre-Alain Fayolle, Benjamin Schmitt
Copyright 2007-2008 Oleg Fryazinov, Denis Kravtsov

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



/////////////////////////////////////
// INCLUDE 
/////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "main.h"
#include "hf_draw.h"
#include "hf_error.h"

////////////////////////////////////
// Global variables
////////////////////////////////////
//Trackball * globalTrackball = NULL;

int width;
int height;



CMainApp *edi;

int vn_t;

double itsCPos[3];
double itsCLookAt[3];

int HF_FACE_LINE_FILL;  // 0: WIREFRAME; 1: SOLID; 2: WIRE & SOLID
int HF_FLAT_SMOOTH;     // 1: FLAT; 0: SMOOTH
int HF_NORMAL;          // 0: NO NORMALS COMPUTED; 1: NORMALS COMPUTED
int HF_SHOW_NORMAL;     // 0: DO NOT SHOW  NORMALS; 1: SHOW NORMALS
int HF_VERTEX_NORMAL;     // 0: FACE NORMALS; 1: VERTEX NORMALS
int first_normal_show;
int HF_SHOW_AXES;
int HF_LIGHT_TYPE;

GLfloat HF_FACE_COLOR[3];
GLfloat HF_LINE_COLOR[3];

GLfloat HF_MIN[3],HF_MAX[3]; // boundng box

int window_main, sub_render;
int sub_help;

bool HAS_ATTRIBUTES = 1;

extern int solidAndWire;




//////////////////////
// Methods
//////////////////////

// should be called after a CMainApp object has been instanciated
// get value of command line thanks to the CMainApp object
void
init_var (CMainApp *mainapp)
{
	width = mainapp->getWindowSizeX();
	height = mainapp->getWindowSizeY();

	// gives to HF_FACE_LINE_FILL a 0 or 1 value
	// meaning wireframe or solid 
	HF_FACE_LINE_FILL = mainapp->getFaceLineFill();

	// if it has beeen required WireAndSurface from the command line
	// then we should give a value of 2 
	if (mainapp->getWireAndSurface() == 1)
		HF_FACE_LINE_FILL = 2; 

	HF_NORMAL = mainapp->getNormal();
	if (HF_NORMAL)
	{   
		HF_SHOW_NORMAL = mainapp->getNormalDisplay ();
		HF_FLAT_SMOOTH = mainapp->getFlat();
	}
	else // we did not compute the normals so we can not show them
	{
		HF_SHOW_NORMAL = 0;
		HF_FLAT_SMOOTH = 1; // defaut is flat
	}

	first_normal_show = 0;
	HF_SHOW_AXES = 1;

	HF_VERTEX_NORMAL = 1;

	HF_LIGHT_TYPE = mainapp->getLightType (); // 1 diffuse; 2 specular; 
	// 3: diffuse & specular.

	mainapp->getFaceColor(HF_FACE_COLOR);
	mainapp->getLineColor(HF_LINE_COLOR);
	mainapp->getBoundingBox(HF_MIN, HF_MAX);
	mainapp->getViewPos(itsCPos, itsCLookAt);

	//BEN
	HAS_ATTRIBUTES = edi->getAttribute();



}

/*

void 
idle_func () 
{
	glutPostRedisplay ();
}


void 
icon (int State) 
{
	switch (State)
	{
	case GLUT_VISIBLE:
		glutIdleFunc (idle_func);
		break;

	case GLUT_NOT_VISIBLE:
		glutIdleFunc (NULL);
		break;

	default:
		break;
	}
}


void 
display_help(void)
{
	GL_CALL(glClear (GL_COLOR_BUFFER_BIT), ;);
	displaytext ();
	GL_CALL(glutSwapBuffers (), ;);
}


void 
display_render(void) 
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GL_CALL(glPushMatrix (), ;);
	if (globalTrackball)
		globalTrackball->matrix ();

	if (solidAndWire == 0) 
	{
		GL_CALL(glCallList(hfObject), ;);
	}
	else
	{
		// we want wireframe surimposing surface
		GL_CALL(glCallList(hfObject), ;);

		GL_CALL(glDisable(GL_LIGHTING),;);
		GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE),;);
		GL_CALL(glEnable(GL_POLYGON_OFFSET_LINE),;);
		GL_CALL(glPolygonOffset(-1.0f, -1.0f),;);

		GL_CALL(glLineWidth(0.4),;);
		GL_CALL(glColor3f(HF_LINE_COLOR[0],HF_LINE_COLOR[1],HF_LINE_COLOR[2]),;);
		GL_CALL(glCallList(hfObject),;);

		GL_CALL(glDisable(GL_POLYGON_OFFSET_LINE),;); 
		GL_CALL(glPolygonMode(GL_FRONT_AND_BACK,GL_FILL),;); 
		GL_CALL(glEnable(GL_LIGHTING),;); 
		// end of modification
	}

	if (HF_SHOW_NORMAL == 1 && HF_NORMAL)
	{
		GL_CALL(glCallList (hfNormals),;);
	}

	if (HF_SHOW_AXES == 1)
	{
		GL_CALL(glLineWidth(2.0),;);
		GL_CALL(glCallList (hfAxes),;);
		GL_CALL(glLineWidth(1.0),;);
	}
	GL_CALL(glCallList(hfObject),;);
	GL_CALL(glPopMatrix (),;);

	GL_CALL(glutSwapBuffers (),;);
}


void
display_main(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
}


void 
reshape_render(int w, int h) 
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (globalTrackball)
		globalTrackball->reshape (w, h);

	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (30, (GLfloat) w / (GLfloat) h, 1.0, 200.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	gluLookAt (itsCPos[0], itsCPos[1], itsCPos[2], 
		itsCLookAt[0], itsCLookAt[1], itsCLookAt[2], 
		0, 1, 0);
}


void
reshape_help(int w, int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,w,h,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void
reshape_main(int w, int h)
{
	int sub_w=w, sub_h=h-70;

	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,w,h,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutSetWindow(sub_render);
	glutPositionWindow(0,0);
	glutReshapeWindow(sub_w, sub_h);

	glutSetWindow(sub_help);
	glutPositionWindow(0, sub_h);
	glutReshapeWindow(sub_w, 70);
}


void 
freeRessources (void) 
{
	if(edi!=NULL){
		delete (edi);
		edi=NULL;
	}

	if (! (vn_t<=0) ) 
		if(globalTrackball!=NULL){
			delete (globalTrackball);
			globalTrackball=NULL;
		}

		glutDestroyWindow(sub_help);
		glutDestroyWindow(sub_render);
		glutDestroyWindow(window_main);
}




int 
main (int argc, char **argv) 
{
	edi = new CMainApp (argc, argv);

	vn_t = edi->init();
	if (vn_t <= 0){
		if(edi!=NULL){
			delete(edi);
			edi=NULL;
		}
		printf ("Isosurface not generated...\n");
		exit (1);
	}
	init_var(edi);

	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize (width, height);
	glutInitWindowPosition (0, 0);

	window_main = glutCreateWindow (argv[0]);


	atexit (freeRessources);





	glutDisplayFunc (display_main);
	glutReshapeFunc (reshape_main);
	glutIdleFunc (idle_func);

	glutVisibilityFunc (icon);

	sub_render = glutCreateSubWindow(window_main, 0, 0, width, height-70);

	globalTrackball = new Trackball (sub_render);
	globalTrackball->animate (GL_FALSE);
	globalTrackball->setCurrent ();


	glutDisplayFunc (display_render);
	glutReshapeFunc(reshape_render);
	glutKeyboardFunc (keyboard);
	glutMouseFunc (my_mouse);
	glutMotionFunc (my_motion);
	glutPassiveMotionFunc (my_passive_motion);
	glutSpecialFunc (SpecialKeyboard);
	init_render ();

	sub_help = glutCreateSubWindow(window_main, 0, height-70, width, 70);
	glutDisplayFunc (display_help);
	glutReshapeFunc(reshape_help);
	init_help();

	glutMainLoop ();
	return 0;
}
*/