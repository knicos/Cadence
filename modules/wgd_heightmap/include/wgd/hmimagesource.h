#include <wgd/heightmapsource.h>
#include <wgd/texture.h>
#include <cadence/file.h>

namespace wgd {
	class HEIGHTMAPIMPORT HMImageSource : public HeightmapSource {
		public:
		OBJECT(HeightmapSource, HMImageSource);

		HMImageSource(cadence::File* heights);
		HMImageSource(const cadence::doste::OID &o);
		~HMImageSource();
		
		PROPERTY_RF(float, defaultHeight, "defaultheight");
		PROPERTY_WF(float, defaultHeight, "defaultheight");

		PROPERTY_RF(cadence::doste::OID, mode, "mode");
		PROPERTY_WF(cadence::doste::OID, mode, "mode");
		
		/** Heightmap is repeated */
		PROPERTY_RF(bool, repeat, "repeat");
		PROPERTY_WF(bool, repeat, "repeat");
		
		PROPERTY_RF(cadence::File, file, "file");
		PROPERTY_WF(cadence::File, file, "file");
		
		PROPERTY_RF(Material, material, ix::material);
		PROPERTY_WF(Material, material, ix::material);

		void begin();
		void end();
		HMRegion *region(int x, int y);

		//BEGIN_EVENTS(HeightmapSource);
		//EVENT(evt_height, (*this)("defaultheight"));
		//EVENT(evt_mode, (*this)("mode"));
		//EVENT(evt_repeat, (*this)("repeat"));
		//END_EVENTS;

		private:
		//Might use a hash table eventually.
		HMRegion *m_image;
		HMRegion *m_default;
		bool m_valid;
		
		//buffer repeat database variable
		bool m_repeat;
		
		void clearAll();
		void buildImage();
		void buildDefault();
		float convertData(char *data);
		int countBits(int);
	};
};
