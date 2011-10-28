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

#include <GL/gl.h>
#include <GL/glu.h>

#include "main.h"
#include "hf_draw.h"
#include "hf_error.h"


// Global variables
GLuint hfNormals;
GLuint hfAxes;
GLuint hfObject;

int solidAndWire=0;

float HFdif[]={0.7,0.7,0.7};
float HFspec[]={.2,.2,.2};


void SetMaterial(GLfloat *color) 
{
	GLfloat ambient_color[4];
	GLfloat diffuse_color[4];

	ambient_color[0]=color[0]*0.1;
	ambient_color[1]=color[1]*0.1;
	ambient_color[2]=color[2]*0.1;
	ambient_color[3]=1.0;

	diffuse_color[0]=color[0];
	diffuse_color[1]=color[1];
	diffuse_color[2]=color[2];
	diffuse_color[3]=1.0;

	GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_color), ;);
	GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color), ;); 
	GL_CALL(glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10), ;);

	if (HF_LIGHT_TYPE == 2)
	{
		GLfloat specular_color[4];

		specular_color[0]=color[0]*0.5;
		specular_color[1]=color[1]*0.5;
		specular_color[2]=color[2]*0.5;
		specular_color[3]=1.0;

		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color), ;); 
		GL_CALL(glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10), ;);
	}

	if (HF_LIGHT_TYPE == 3)
	{
		GLfloat specular_color[4];
		GLfloat diffuse_color[4];

		diffuse_color[0]=color[0];
		diffuse_color[1]=color[1];
		diffuse_color[2]=color[2];
		diffuse_color[3]=1.0;

		specular_color[0]=1.0;
		specular_color[1]=1.0;
		specular_color[2]=1.0;
		specular_color[3]=1.0;

		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color), ;); 
		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color), ;); 
		GL_CALL(glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10), ;);
	}
}


void SetMeshProperties(){

	if(HF_FACE_LINE_FILL==0) 
	{ 
		GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL), ;); 
		solidAndWire = 0;  
	}
	else if(HF_FACE_LINE_FILL==1) 
	{
		GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE), ;);
		solidAndWire = 0;
	}
	else if(HF_FACE_LINE_FILL == 2) // we want to have wireframe and solid
		solidAndWire = 1;

	if(HF_FLAT_SMOOTH==1)
	{
		GL_CALL(glShadeModel(GL_FLAT), ;);
	}
	else  if(HF_FLAT_SMOOTH==0)
		GL_CALL(glShadeModel(GL_SMOOTH), ;);
}


void CreateAxes(){
	GLfloat cl[4]={0,0,0,1.0};

	GL_CALL(glLineWidth(2.0), ;);

	GL_CALL(glBegin(GL_LINES), ;);

	// Axis 1
	// line width & color
	cl[0]=1.0;
	if (HF_NORMAL)
	{
		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cl), ;);
		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cl), ;);
	}
	else
	{
		GL_CALL(glColor3f(cl[0], cl[1], cl[2]), ;);
	}
	// define line
	GL_CALL(glVertex3f(0,0,0), ;);
	GL_CALL(glVertex3f(1.5*HF_MAX[0],0,0), ;);


	// Axis 2
	// line color
	cl[0]=0.0;
	cl[1]=1.0;
	if (HF_NORMAL)
	{
		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cl), ;);
		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cl), ;);
	} 
	else
	{
		GL_CALL(glColor3f(cl[0], cl[1], cl[2]), ;);
	}
	// define line
	GL_CALL(glVertex3f(0,0,0), ;);
	GL_CALL(glVertex3f(0,1.5*HF_MAX[1],0), ;);


	// Axis 3
	// line color
	cl[1]=0.0;
	cl[2]=1.0;
	if (HF_NORMAL)
	{   
		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cl), ;);
		GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cl), ;);
	}
	else {
		GL_CALL(glColor3f(cl[0], cl[1], cl[2]), ;);
	}
	// define line
	GL_CALL(glVertex3f(0,0,0), ;);
	GL_CALL(glVertex3f(0,0,1.5*HF_MAX[2]), ;);

	GL_CALL(glEnd(), ;);
}


void DrawNormal(){
	GLfloat cl[3]={0,0,0};

	GL_CALL(glLineWidth(1.0), ;);
	GL_CALL(glBegin(GL_LINES), ;);
	SetMaterial(cl);
	edi->CreateNormal();
	GL_CALL(glEnd(), ;);
}


void triangle(void)
{
	if (!HAS_ATTRIBUTES){
		GL_CALL(glBegin (GL_TRIANGLES), ;);

		if(HF_NORMAL) {  
			GL_CALL(SetMaterial(HF_FACE_COLOR),;);
			edi->createGLTriangle(true, HF_VERTEX_NORMAL ,false);
		}
		else{
			GL_CALL(glColor3f(HF_LINE_COLOR[0],HF_LINE_COLOR[1],HF_LINE_COLOR[2]), ;);
			edi->createGLTriangle(false, HF_VERTEX_NORMAL, false);
		}
		glEnd();
	}
	else{
		float amb[4];
		amb[3] = 1.0;
		GL_CALL(glBegin (GL_TRIANGLES), ;);
		if(HF_NORMAL) {
			edi->createGLTriangle(true, HF_VERTEX_NORMAL, true);
		}
		else{
			//glEnable GL_COLOR_MATERIAL
			edi->createGLTriangle(false, HF_VERTEX_NORMAL, false);
		}
		GL_CALL(glEnd(), ;);
	}
}


void SetLight(){
	GLfloat lmodel_twoside[1] = {GL_TRUE};

	GLfloat ambient[4] = {0.1, 0.1, 0.1, 1.0};
	GLfloat diffuse[4] = {1, 1, 1, 1};
	GLfloat specular[4] = {0.3, 0.3, 0.3, 1};

	GLfloat lmodel_ambient[4] = {1.0, 1.0, 1.0, 1.0};


	// position of lights

	GLfloat pos1[4]={5*(HF_MAX[0]*0.9),5*(HF_MAX[1]*0.9),5*HF_MAX[2],0},
		pos2[4]={0,0,5*HF_MIN[2],0};
	GLfloat direction1[3]={5*HF_MIN[0],5*HF_MIN[1],5*HF_MIN[2]},
		direction2[3]={5*HF_MIN[0],5*HF_MAX[1],5*HF_MAX[2]};

	GL_CALL(glLightfv(GL_LIGHT0, GL_POSITION, pos1), ;);
	GL_CALL(glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction1),;);
	GL_CALL(glLightfv(GL_LIGHT0, GL_AMBIENT, ambient),;);

	if(HF_LIGHT_TYPE==1)
	{
		GL_CALL(glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse),;);
	}
	if (HF_LIGHT_TYPE==2)
	{
		GL_CALL(glLightfv(GL_LIGHT0, GL_SPECULAR, specular),;);
	}
	if (HF_LIGHT_TYPE==3)
	{
		GL_CALL(glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse),;);
		GL_CALL(glLightfv(GL_LIGHT0, GL_SPECULAR, specular),;);
	}

	GL_CALL(glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient),;);

	GL_CALL(glEnable(GL_LIGHTING),;);
	GL_CALL(glEnable(GL_LIGHT0),;);
}


void init_render(void)
{
	hfObject = glGenLists (1);
	GL_CALL(glNewList(hfObject, GL_COMPILE),;);
	triangle();
	GL_CALL(glEndList(),;);

	hfAxes = glGenLists (3);
	GL_CALL(glNewList(hfAxes, GL_COMPILE),;); 
	CreateAxes();
	GL_CALL(glEndList(),;);

	GL_CALL(glClearColor(0.5, 0.5, 0.5, 0.0),;);

	GL_CALL(glFrontFace(GL_CCW),;);
	GL_CALL(glEnable(GL_NORMALIZE),;);
	GL_CALL(glEnable(GL_DEPTH_TEST),;);
	GL_CALL(glDepthFunc(GL_LESS),;);

	if (HF_NORMAL) { SetLight(); }

	SetMeshProperties();
}


void init_help(void)
{
	GL_CALL(glClearColor(0.5, 0.5, 0.5, 0.0),;);
}
