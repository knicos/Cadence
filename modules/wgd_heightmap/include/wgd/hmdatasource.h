#include <wgd/heightmapsource.h>
#include <wgd/texture.h>
#include <cadence/file.h>

namespace wgd {
	class HEIGHTMAPIMPORT HMDataSource : HeightmapSource {
		public:
		OBJECT(HeightmapSource, HMDataSource);

		HMDataSource();
		HMDataSource(const cadence::doste::OID &o);
		~HMDataSource();

		//PROPERTY_RF(float, defaultHeight, "defaultheight");
		//PROPERTY_WF(float, defaultHeight, "defaultheight");

		PROPERTY_RF(cadence::File, file, "file");
		PROPERTY_WF(cadence::File, file, "file");

		void begin();
		void end();
		HMRegion *region(int x, int y);

		//BEGIN_EVENTS(HeightmapSource);
		//EVENT(evt_height, (*this)("defaultheight"));
		//EVENT(evt_mode, (*this)("mode"));
		//END_EVENTS;

		private:
		
	};
}; 
