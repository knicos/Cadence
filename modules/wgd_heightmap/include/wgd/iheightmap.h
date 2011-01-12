#ifndef _WGD_IHEIGHTMAP_
#define _WGD_IHEIGHTMAP_

#include <wgd/common.h>
#include <wgd/instance3d.h>
#include <wgd/heightmapsource.h>

namespace wgd {
	class HEIGHTMAPIMPORT IHeightmap : public wgd::Instance3D {
		public:
		
		IHeightmap(HeightmapSource* source);
		IHeightmap(const OID&);
		~IHeightmap();
		
		void draw(SceneGraph &graph, Camera3D *camera);
		
		OBJECT(Instance3D, IHeightmap);
		
		// Data source for heightmap data
		PROPERTY_RF(HeightmapSource, source, "source");
		PROPERTY_WF(HeightmapSource, source, "source");
		
		//LOD Patchsize - Must be a power of 2
		PROPERTY_RF(int, patchSize, "patchsize");
		PROPERTY_WF(int, patchSize, "patchsize");
		
		//scale of the detailed spat textures
		PROPERTY_RF(wgd::vector2d, splatScale, "splatscale");
		PROPERTY_WF(wgd::vector2d, splatScale, "splatscale");
		void splatScale(float s) { splatScale(vector2d(s,s)); };

		/** Draw the heightmap in wireframe mode */
		PROPERTY_RF(bool, wireframe, "wireframe");
		PROPERTY_WF(bool, wireframe, "wireframe");
		
		/** Splat fade distance.
		 *	If this is 0, no splat textured are drawn.
		 */
		PROPERTY_RF(float, fade, "fade");
		PROPERTY_WF(float, fade, "fade");
		
		/** Level of detail metric (higher number = less detail) */
		PROPERTY_RF(float, lod, "lod");
		PROPERTY_WF(float, lod, "lod");
		
		/** heightmap scale - defines how far apart the vertices are */
		PROPERTY_RF(vector3d, scale, ix::scale);
		PROPERTY_WF(vector3d, scale, ix::scale);
		void scale(float s) { scale(vector3d(s,s,s)); };
		
		/** 
		 *	Get the height at any point
		 *	@param v The point to query the height at. Note y is ignored
		 *	@return height of the point (y coordinate)
		 */
		float height(const wgd::vector3d &v);
		/** 
		 *	Get the height at any point
		 *	@param x The x coordinate of the point
		 *	@param z The z coordinate of the point
		 *	@return height of the point (y coordinate)
		 */		
		float height(float x, float z);
		/** 
		 *	Get the surface normal at any point 
		 *	@param v The point to query the height at. Note y is ignored
		 */
		wgd::vector3d normal(const wgd::vector3d &v);
		
		/**
		 *	Get the point where a line segment intersects the heightmap
		 *	@param result The calculated intersection point
		 *	@param start The start point of the line in world coordinates
		 *	@param end The end point of the line in world coordinates
		 *	@return Whether the line segment intersects the heightmap
		 */
		bool intersect(wgd::vector3d &result, const wgd::vector3d &start, const wgd::vector3d &end);
		/**
		 *	Get the point where a ray hits the heightmap
		 *	@param result The calculated intersection point
		 *	@param point The start point of the ray in world coordinates
		 *	@param direction The direction of the ray
		 *	@return Whether the ray hits the heightmap
		 */		
		bool ray(wgd::vector3d &result, const wgd::vector3d &point, const wgd::vector3d &direction);
		
		private:
		
		// Draw the region at said region coordinates
		void drawRegion(Camera3D *camera, int rx, int ry);
		
		//Get a patch (using patch coordinates)
		wgd::Patch *patch(int x, int y);
		
		//Calculate the glue values for a patch - takes absolute patch coordinates (not relative to region)
		unsigned int glue(wgd::Patch *p, int x, int y);
		
		// Calculate the error metric for getting mip levels from viewport distortion
		void errorMetric(float fovY, int viewHeight);
		
		//recursively draw patches
		void drawPatchR(HMRegion *region, int rx, int ry, int node, int x, int y, int size);
		void drawSplatR(HMRegion *region, int splat, int rx, int ry, int node, int x, int y, int size);

		//Index Array Generation
		void buildPatch (Patch *p, unsigned int glue);
		void acrossStrip(Patch *p, int start, int length, int step);
		void leftGlue   (Patch *p, int nextLevel); //nextLevel being the mip level of the next patch
		void rightGlue  (Patch *p, int nextLevel);
		void topGlue    (Patch *p, int nextLevel);
		void bottomGlue (Patch *p, int nextLevel);

		//some local data
		float m_errorMetric;	//Error metric to calculate mip level
		int m_regionSize;		//Size of region in vertices
		int m_patchSize;		//size of patch in vertices
		int m_patches;			//Size of region in patches
		float m_fade;			//splat fade distance (this is the cut-off distance)
		vector3d m_scale;		//heightmap scale
		
		HeightmapSource* m_source;
		wgd::vector2d m_splatScale;
		float m_farClip;
		
		//info
		int m_polys;
		float m_maxError;
		int m_count;
		
		//ray debug
		wgd::vector3d vs, ve;
		
	};
};

#endif
