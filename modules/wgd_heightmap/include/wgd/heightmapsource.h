#ifndef _WGD_HEIGHTMAP_SOURCE_
#define _WGD_HEIGHTMAP_SOURCE_

#include <wgd/heightmap.h>
#include <cadence/object.h>


namespace wgd {
	/**
	 * This class is where the heightmap data comes from, be it from a texture, streamed from
	 * a file or procedurally generated. It must create HMRegion objects to store the heightmap data.
	 */
	class HEIGHTMAPIMPORT HeightmapSource : public cadence::Agent {
		public:
		VOBJECT(Agent, HeightmapSource);

		HeightmapSource() : Agent() {}
		HeightmapSource(const cadence::doste::OID &o) : Agent(o) {}
		virtual ~HeightmapSource() {};

		/**
		 * number of vertices along a side of the region This does not include the extra overlap vertices
		 * and therefore must be a multiple of patchsize.
		 */
		int regionSize() const { return m_size; }
		
		
		/** begin and end drawing of the height map so you know when to delete unused regions */
		virtual void begin() {};
		virtual void end() {};
		
		/** 
		 * Get the region at region coordinates (x, y). 
		 * This will create the region asynchronously if it does not exist
		 * and return either null, or a default flat region until it is loaded.
		 * 
		 */
		virtual HMRegion *region(int x, int y) = 0;
		
		protected:
		void regionSize(int s) { m_size = s; }

		private:
		int m_size;
	};
};

#endif
