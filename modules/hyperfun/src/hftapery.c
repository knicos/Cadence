/*==============================================================================

 Copyright 1998, 1999 Valery Adzhiev, Alexander Pasko, Ken Yoshikawa 
 Copyright 2003-2004 Benjamin Schmitt

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


/*
Core HyperFun Library

Operation: Tapering along y-axis

Definition: inverse mapping
   y1<= y <= y2
            t = (y-y1)/(y2-y1)
            scale = (1-t)*s1 + t*s2
            z'=z/scale
            x'=x/scale
   y < y1   scale = s1
   y > y2   scale = s2

Call: hfTaperY(xt,y1,y2,s1,s2);

Parameters:
 xt - point coordinates array to be changed
 y1, y2 - end points of y-interval, y2 > y1
 s1, s2 - scaling factors for end points

Test file: taper.hpf

C-parameters:
	f_a[0]   corresponds to s2
	f_a[1]   corresponds to s1
	f_a[2]   corresponds to y2
	f_a[3]   corresponds to y1
	f_a[4]   corresponds to xt

Code by: Pasko, Adzhiev
Last revised: 11.8.98
*/

#include "general.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

extern int HF_Get_Runtime_Array(int index, double** array, int* size);
void HF_Fill_Error(PN_T* p_pn, int er_index, Error_Info_T* EI, int ic);

extern double EPS;

double hfTaperY(double* f_a, String_Array_T* str_param)
{

double s1,s2,scale,y1,y2,t;
double* xt;
int array_index, num;

s2=f_a[0];
s1=f_a[1];
y2=f_a[2];
y1=f_a[3];

array_index = (int)f_a[4];
HF_Get_Runtime_Array(array_index, &xt, &num);

if(xt[1]<y1) scale = s1;
 else{
      if(xt[1] > y2) scale = s2;
       else{
       t = (xt[1]-y1)/(y2-y1);
       scale = (1-t)*s1 + t*s2;
       }
     }
if(fabs(scale) < EPS) scale = 1.; /* !? discontinuity */
xt[2]=xt[2]/scale;
xt[0]=xt[0]/scale;

return 1.;
}

int R_hfTaperY(double* f_a,
            String_Array_T* str_param,
            Error_Info_T* error_info,
            PN_T* p_pn,
            int code_index)
{

	if((f_a[0] <= 0.) || (f_a[1] <= 0.))
	{
           HF_Fill_Error(p_pn, Blend_2_3_Error, error_info, code_index);
           return 0;
	}

	return 1;
}
