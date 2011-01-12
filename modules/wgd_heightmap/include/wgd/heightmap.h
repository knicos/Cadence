#ifndef _WGD_HEIGHTMAP_REGION_
#define _WGD_HEIGHTMAP_REGION_

#include <wgd/dll.h>
#include <wgd/common.h>
#include <wgd/vector.h>
#include <wgd/vertex.h>
#include <wgd/material.h>


namespace wgd {
	
	//HMRegion needs camara data
	class Camera3D;
	
	// a single patch of a heightmap that will have level of detail applied to //
	struct Patch {
		int offset;				//vertex offset to get vertex data from region (worked out from position)
		unsigned int *indices;	//Index array of which vertices in the region get drawn
		int nIndices;			//size of index array
		int level;				//current mip level
		float *error;			//maximum error at each mip level for the patch
		unsigned int glue;		//used to seamlessly join patches together
		bool changed;			//does the patch need rebuilding
		int node;				//The quadtree node - used to get the AABB for collision
	};	
	
	/**
	 * The raw height data passed from HeightmapLoader to HMRegion.
	 * Heights are normalised between -1.0 and 1.0
	 * Material isthe material that is stretched over the entire heightmap
	 * Materials is an array of nMat material pointers for texture splatting
	 */
	struct HMData {
		int size;
		float *heights;
		
		int nMat;
		wgd::Material **materials;
		wgd::Material *material;
	};
	
	/**
	 * One extra height on all sides of the heightmap to correctly calculate the normals
	 * at the edge of the region Each array is of length size+1 where size is the power of two size of the region
	 * Any of the arrays can be null to just use the edge of the existing heightmap for the normals
	 */
	struct HMPadding {
		float *left;
		float *right;
		float *top;
		float *bottom;
	};
	
	/**
	 * Heightmap regions are uniform blocks of patches, They store the vertex data for the entire region
	 * that will be divided into the lod patches.
	 * The region must be square and a multiple of patchsize, + 1 (the +1 is an overlap to the next region)
	 * There is only one extra overlap column on the right, and row on the bottom from other regions.
	 * Each vertex must have x and z coordinates 1.0 apart and as a uniform grid.
	 * The heightmap instance will do any scaling required.
	 * All vertices are relative to the region position (so the top left vertex is always (0.0, y, 0.0)
	 * The size variable is the number of patches across the region (sqrt patch count)
	 */
	class HEIGHTMAPIMPORT HMRegion {
		public:
		
		struct HMNode {
			char clipFlags;	//Frustum clipping flags
			float min;		//minimum height
			float max;		//maximum height
			int splat;		//splat textures used in this node (per bit)
			float distance;	//distance to camera
		};
		
		/**
		 * Heightmap Region constructor
		 * @param data Structure containing the height data for this region
		 * @param padding Structure of surrounding heights to calculate normals, Can be null.
		 * @param size The number of vertices along an edge of the region - 1, this value must be a power of 2
		 */
		HMRegion(HMData *data, HMPadding *padding, int size);
		~HMRegion();
		
		/** Get the vertex data of the region */
		wgd::vertex *vertices() const { return m_vertices; }
		unsigned int vbo() const { return m_VBO; };
		
		/** Get a specific patch in this region, in patch coordinates relative to the patch */
		Patch* patch(int x, int y) const { return &m_patches[x + y * m_size/m_patchSize]; }
		
		/** Set up patches, This is be called once by IHeightmap before the region is used */
		void setup(int patchSize, const wgd::vector2d &splatScale);
		
		/** Query interpolated height of a point in the region */
		float height(float x, float y) const;
		
		/** Query interpolated normal of a point in the region */
		wgd::vector3d normal(float x, float y) const;
		
		/** Get the first intersection of a line segment with the heightmap */
		bool intersect(wgd::vector3d& point, const wgd::vector3d &start, const wgd::vector3d &end);
		
		/** Drawing stuff */
		void clip(Camera3D *cam, const wgd::vector3d &scale,const wgd::vector3d &position, int rx, int ry);
		void setLevels(Camera3D *cam, const wgd::vector3d &scale, float errorMetric);
		
		/* get the quadtree node */
		HMNode &node(int n) const { return m_tree[n]; }
		
		//Texture stuff
		wgd::Material *baseMaterial() const { return m_baseMaterial; };
		int splatCount() const { return m_splatCount; };
		wgd::Material *splatMaterial(int i) const { return m_splatMaterial[i]; }
		wgd::GLTexCoord *splatCoord() const { return m_spatCoord; };
		
		private:
		void calculateNormals( HMPadding *padding );
		
		/** Query height of a vertex in the region */
		float height(int x, int y) const;	
		wgd::vector3d normal(int x, int y) const;
		
		//get the intersecion between a line segment and the heightmap - uses vertex data
		//returns whether there is an intersection.
		bool intersectPolygons(wgd::vector3d &point, const wgd::vector3d &start, const wgd::vector3d &end);
		//Similar to the above function but uses the patch AABBs in stead of the vertex data
		//This function calls the intersect function to get the actual point if the patch is intersected.
		bool intersectPatches(wgd::vector3d &point, const wgd::vector3d &start, const wgd::vector3d &end);
		
		//vertex data
		wgd::vertex *m_vertices;
		unsigned int m_VBO;
		
		//Size of region
		int m_size;
		
		//regional data for drawing
		int m_rx, m_ry;			//region position
		wgd::vector3d m_pos;	//heightmap position
		wgd::vector3d m_scale;	//heightmap scale
		int m_patchSize;		//patch size
		float m_camFar;
		wgd::vector3d m_camPos;		
		
		//implicit complete quadtree of patches
		int nodeCount(int); //Calculate the number of quadtree in the region
		HMNode *m_tree;

		//recursive functions
		void setLevelsR(int node, int x, int y, int size);
		void clipR(Camera3D *cam, int node, int x, int y, int size, int parent);
		
		//calculate heights for AABBs (x=min, y=max)
		wgd::vector2d calcBoxR(int node, int x, int y, int size);
		
		//calculate maximum error for a patch for a specific mip level
		float calcError(int patchSize, int x, int y, int level);
		
		//error metric for viewport distortion
		float m_errorMetric;
		
		//patches
		Patch *m_patches;
		
		//The number of mip levels a patch has
		int levels(int patchSize); 
		int m_levels;
		
		
		//	TEXTURING	//
		wgd::Material* m_baseMaterial;
		wgd::GLTexCoord *m_spatCoord;
		wgd::Material**m_splatMaterial;
		int m_splatCount;
		
	};
};

#endif
