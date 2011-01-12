/*
 * WGD Library
 *
 * Authors: 
 * Date: 25/9/2007
 *
 */

#ifndef _WGD_INDEX_
#define _WGD_INDEX_

#include <cadence/doste/oid.h>
#include <wgd/dll.h>

namespace wgd{
	namespace ix{
		extern BASEIMPORT cadence::doste::OID position;
		extern BASEIMPORT cadence::doste::OID orientation;
		extern BASEIMPORT cadence::doste::OID direction;
		extern BASEIMPORT cadence::doste::OID changed;
		extern BASEIMPORT cadence::doste::OID material;
		extern BASEIMPORT cadence::doste::OID materials;
		extern BASEIMPORT cadence::doste::OID model;
		extern BASEIMPORT cadence::doste::OID models;
		extern BASEIMPORT cadence::doste::OID animationset;
		extern BASEIMPORT cadence::doste::OID animations;
		extern BASEIMPORT cadence::doste::OID animation;
		extern BASEIMPORT cadence::doste::OID animated;
		extern BASEIMPORT cadence::doste::OID transformed;
		extern BASEIMPORT cadence::doste::OID transition;
		extern BASEIMPORT cadence::doste::OID meshes;
		extern BASEIMPORT cadence::doste::OID morphs;
		extern BASEIMPORT cadence::doste::OID morph;
		extern BASEIMPORT cadence::doste::OID root;
		extern BASEIMPORT cadence::doste::OID bones;
		extern BASEIMPORT cadence::doste::OID animset;
		extern BASEIMPORT cadence::doste::OID start;
		extern BASEIMPORT cadence::doste::OID end;
		extern BASEIMPORT cadence::doste::OID fps;
		extern BASEIMPORT cadence::doste::OID loop;
		extern BASEIMPORT cadence::doste::OID next;
		extern BASEIMPORT cadence::doste::OID solid;
		extern BASEIMPORT cadence::doste::OID mode;

		extern BASEIMPORT cadence::doste::OID debug;
		
		extern BASEIMPORT cadence::doste::OID x;
		extern BASEIMPORT cadence::doste::OID y;
		extern BASEIMPORT cadence::doste::OID z;
		extern BASEIMPORT cadence::doste::OID w;
		extern BASEIMPORT cadence::doste::OID r;
		extern BASEIMPORT cadence::doste::OID g;
		extern BASEIMPORT cadence::doste::OID b;
		extern BASEIMPORT cadence::doste::OID a;
		extern BASEIMPORT cadence::doste::OID sx;
		extern BASEIMPORT cadence::doste::OID sy;
		extern BASEIMPORT cadence::doste::OID sz;
		extern BASEIMPORT cadence::doste::OID pitch;
		extern BASEIMPORT cadence::doste::OID yaw;
		extern BASEIMPORT cadence::doste::OID roll;

		extern BASEIMPORT cadence::doste::OID console;
		extern BASEIMPORT cadence::doste::OID lines;
		extern BASEIMPORT cadence::doste::OID visible;
		extern BASEIMPORT cadence::doste::OID showdebug;
		extern BASEIMPORT cadence::doste::OID font;
		extern BASEIMPORT cadence::doste::OID bgcolour;
		extern BASEIMPORT cadence::doste::OID text;

		extern BASEIMPORT cadence::doste::OID ambient;
		extern BASEIMPORT cadence::doste::OID specular;
		extern BASEIMPORT cadence::doste::OID diffuse;
		extern BASEIMPORT cadence::doste::OID emission;
		extern BASEIMPORT cadence::doste::OID directional;
		extern BASEIMPORT cadence::doste::OID linear;
		extern BASEIMPORT cadence::doste::OID quadratic;
		extern BASEIMPORT cadence::doste::OID enabled;
		extern BASEIMPORT cadence::doste::OID lights;

		extern BASEIMPORT cadence::doste::OID size;
		extern BASEIMPORT cadence::doste::OID name;
		extern BASEIMPORT cadence::doste::OID bold;
		extern BASEIMPORT cadence::doste::OID fonts;

		extern BASEIMPORT cadence::doste::OID shininess;
		extern BASEIMPORT cadence::doste::OID shader;
		extern BASEIMPORT cadence::doste::OID texture;
		extern BASEIMPORT cadence::doste::OID textures;

		extern BASEIMPORT cadence::doste::OID clamp;
		extern BASEIMPORT cadence::doste::OID filename;
		extern BASEIMPORT cadence::doste::OID mipmap;
		extern BASEIMPORT cadence::doste::OID objects;
		extern BASEIMPORT cadence::doste::OID world;
		extern BASEIMPORT cadence::doste::OID positionvar;
		extern BASEIMPORT cadence::doste::OID rate;
		extern BASEIMPORT cadence::doste::OID life;
		extern BASEIMPORT cadence::doste::OID particles;
		extern BASEIMPORT cadence::doste::OID primitive;
		extern BASEIMPORT cadence::doste::OID primitives;
		extern BASEIMPORT cadence::doste::OID width;
		extern BASEIMPORT cadence::doste::OID height;
		extern BASEIMPORT cadence::doste::OID depth;
		extern BASEIMPORT cadence::doste::OID radius;
		extern BASEIMPORT cadence::doste::OID pitch;
		extern BASEIMPORT cadence::doste::OID playing;
		extern BASEIMPORT cadence::doste::OID attenuation;
		extern BASEIMPORT cadence::doste::OID volume;
		extern BASEIMPORT cadence::doste::OID sounds;
		extern BASEIMPORT cadence::doste::OID playing;
		extern BASEIMPORT cadence::doste::OID sound;
		extern BASEIMPORT cadence::doste::OID velocity;
		extern BASEIMPORT cadence::doste::OID sprite;
		extern BASEIMPORT cadence::doste::OID sprites;
		extern BASEIMPORT cadence::doste::OID frame;
		extern BASEIMPORT cadence::doste::OID font;
		extern BASEIMPORT cadence::doste::OID text;
		extern BASEIMPORT cadence::doste::OID music;
		extern BASEIMPORT cadence::doste::OID number;
		extern BASEIMPORT cadence::doste::OID velocityvar;
		extern BASEIMPORT cadence::doste::OID growth;
		extern BASEIMPORT cadence::doste::OID growthvar;
		extern BASEIMPORT cadence::doste::OID endsize;
		extern BASEIMPORT cadence::doste::OID endsizevar;
		extern BASEIMPORT cadence::doste::OID startsize;
		extern BASEIMPORT cadence::doste::OID startsizevar;
		extern BASEIMPORT cadence::doste::OID lifevar;
		extern BASEIMPORT cadence::doste::OID fade;
		extern BASEIMPORT cadence::doste::OID fadevar;
		extern BASEIMPORT cadence::doste::OID startcolour;
		extern BASEIMPORT cadence::doste::OID startcolourvar;
		extern BASEIMPORT cadence::doste::OID endcolour;
		extern BASEIMPORT cadence::doste::OID endcolourvar;
		extern BASEIMPORT cadence::doste::OID acceleration;
		extern BASEIMPORT cadence::doste::OID accelerationvar;
		extern BASEIMPORT cadence::doste::OID resources;
		extern BASEIMPORT cadence::doste::OID vert;
		extern BASEIMPORT cadence::doste::OID frag;
		extern BASEIMPORT cadence::doste::OID shaders;
		extern BASEIMPORT cadence::doste::OID left;
		extern BASEIMPORT cadence::doste::OID right;
		extern BASEIMPORT cadence::doste::OID top;
		extern BASEIMPORT cadence::doste::OID bottom;
		extern BASEIMPORT cadence::doste::OID front;
		extern BASEIMPORT cadence::doste::OID back;
		extern BASEIMPORT cadence::doste::OID skybox;
		extern BASEIMPORT cadence::doste::OID skyboxes;
		extern BASEIMPORT cadence::doste::OID cur;
		extern BASEIMPORT cadence::doste::OID frames;
		extern BASEIMPORT cadence::doste::OID columns;
		extern BASEIMPORT cadence::doste::OID rows;
		extern BASEIMPORT cadence::doste::OID billboard;
		extern BASEIMPORT cadence::doste::OID distance;
		extern BASEIMPORT cadence::doste::OID atmosphere;
		extern BASEIMPORT cadence::doste::OID density;
		extern BASEIMPORT cadence::doste::OID camera;
		extern BASEIMPORT cadence::doste::OID fov;
		#undef far
		extern BASEIMPORT cadence::doste::OID far;
		#undef near
		extern BASEIMPORT cadence::doste::OID near;
		extern BASEIMPORT cadence::doste::OID zoom;
		extern BASEIMPORT cadence::doste::OID running;
		extern BASEIMPORT cadence::doste::OID flipped;
		extern BASEIMPORT cadence::doste::OID keyboard;
		extern BASEIMPORT cadence::doste::OID keys;
		extern BASEIMPORT cadence::doste::OID commands;
		extern BASEIMPORT cadence::doste::OID lighting;
		extern BASEIMPORT cadence::doste::OID smooth;
		extern BASEIMPORT cadence::doste::OID mouse;
		extern BASEIMPORT cadence::doste::OID deltax;
		extern BASEIMPORT cadence::doste::OID deltay;
		extern BASEIMPORT cadence::doste::OID buttons;
		extern BASEIMPORT cadence::doste::OID visible;
		extern BASEIMPORT cadence::doste::OID cursor;
		extern BASEIMPORT cadence::doste::OID colour;
		extern BASEIMPORT cadence::doste::OID window;
		extern BASEIMPORT cadence::doste::OID cube;
		extern BASEIMPORT cadence::doste::OID deltaz;
		extern BASEIMPORT cadence::doste::OID game;
		
		extern BASEIMPORT cadence::doste::OID scene;
		extern BASEIMPORT cadence::doste::OID levels;
		
		extern BASEIMPORT cadence::doste::OID instances;
		extern BASEIMPORT cadence::doste::OID instance2D;
		extern BASEIMPORT cadence::doste::OID instance3D;
		

		extern BASEIMPORT cadence::doste::OID resolutions;
		extern BASEIMPORT cadence::doste::OID clearcolour;
		extern BASEIMPORT cadence::doste::OID colourdepth;
		extern BASEIMPORT cadence::doste::OID depthbuffer;
		extern BASEIMPORT cadence::doste::OID title;
		extern BASEIMPORT cadence::doste::OID fullscreen;
		extern BASEIMPORT cadence::doste::OID movespeed;

		extern BASEIMPORT cadence::doste::OID paused;
		extern BASEIMPORT cadence::doste::OID blending;
		extern BASEIMPORT cadence::doste::OID resistance;
		extern BASEIMPORT cadence::doste::OID resistancevar;

		extern BASEIMPORT cadence::doste::OID axes;
		extern BASEIMPORT cadence::doste::OID compress;
		extern BASEIMPORT cadence::doste::OID controls;
		extern BASEIMPORT cadence::doste::OID scale;
		extern BASEIMPORT cadence::doste::OID value;
		extern BASEIMPORT cadence::doste::OID deadzone;
		extern BASEIMPORT cadence::doste::OID invert;

		extern BASEIMPORT cadence::doste::OID keep;
		extern BASEIMPORT cadence::doste::OID flipv;
		extern BASEIMPORT cadence::doste::OID fliph;
		extern BASEIMPORT cadence::doste::OID tint;
		extern BASEIMPORT cadence::doste::OID layer;
		extern BASEIMPORT cadence::doste::OID widgets;
		extern BASEIMPORT cadence::doste::OID caption;
		extern BASEIMPORT cadence::doste::OID grab;
		extern BASEIMPORT cadence::doste::OID focus;
		extern BASEIMPORT cadence::doste::OID parent;
		extern BASEIMPORT cadence::doste::OID screenx;
		extern BASEIMPORT cadence::doste::OID screeny;
		
		extern BASEIMPORT cadence::doste::OID volumes;
		extern BASEIMPORT cadence::doste::OID children;
		extern BASEIMPORT cadence::doste::OID type;
		extern BASEIMPORT cadence::doste::OID light;

		extern BASEIMPORT cadence::doste::OID glowfilename;
		extern BASEIMPORT cadence::doste::OID glowmax;
		extern BASEIMPORT cadence::doste::OID reload;
		extern BASEIMPORT cadence::doste::OID variables;
		extern BASEIMPORT cadence::doste::OID input;

		extern BASEIMPORT cadence::doste::OID source;
		extern BASEIMPORT cadence::doste::OID location;
		
		extern BASEIMPORT cadence::doste::OID time;
	};
};
#endif
