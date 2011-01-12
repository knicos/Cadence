#ifndef _WGD_RENDER_H_
#define _WGD_RENDER_H_

#include <wgd/camera2d.h>
#include <wgd/camera3d.h>
#include <wgd/sprite.h>

namespace wgd {
	class matrix;

	class BASE3DIMPORT Render {
		public:

		static void begin(Camera3D *cam);
		static void being(Camera2D *cam);
		static void end();

		//static void sprite(Sprite *spr, int frame, const vector2d &pos);
		//static void sprite(Sprite *spr, int frame, const vector2d &pos, float orientation);
		static void sprite(Sprite *spr, int frame, const vector3d &pos);
		static void sprite(Sprite *spr, int frame, const vector3d &pos, const quaternion &orientation);
		static void sprite(Sprite *spr, int frame);
		//Sprite batch?

		//static void text(Font *fnt, const vector3d &pos, const char *str);

		//static void line(const vector3d &a, const vector3d &b) { line(a,b,colour(1.0,1.0,1.0,1.0),1); };
		//static void line(const vector3d &a, const vector3d &b, const colour &col) { line(a,b,col,1); };
		//static void line(const vector3d &a, const vector3d &b, const colour &col, int width);

		static void transform(const matrix &mat);
		static void untransform();

		private:
		static Camera3D *s_cam3d;
		static Camera2D *s_cam2d;
	};
};

#endif
