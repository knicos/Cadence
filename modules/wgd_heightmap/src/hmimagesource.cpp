#include <wgd/hmimagesource.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

HMImageSource::HMImageSource(cadence::File* heights) : HeightmapSource(), m_image(0), m_default(0) {
	m_valid = true;
	file(heights);
}

HMImageSource::HMImageSource(const OID &o) : HeightmapSource(o), m_image(0), m_default(0) {
	m_valid = true;
}

HMImageSource::~HMImageSource() {
	if (m_image != 0) delete m_image;
	if (m_default != 0) delete m_default;
}

void HMImageSource::begin() {
	//buffer variable
	m_repeat = repeat();
	
	if (m_image == 0 && m_valid) {
		buildImage();
	}

	if (m_default == 0 && !m_repeat) {
		buildDefault();
	}
}

void HMImageSource::end() {

}

HMRegion *HMImageSource::region(int x, int y) {
	if(m_repeat) return m_image;
	else if (x == 0 && y == 0 && m_image != 0) return m_image;
	else return m_default;
}

void HMImageSource::buildDefault() {
	if (regionSize() == 0) {
		regionSize(128);
	}

	HMData data;
	data.size = (regionSize()+1) * (regionSize()+1);
	data.heights = new float[data.size];
	data.nMat = 0;
	data.materials = 0;
	data.material = get(ix::material);

	for (int i=0; i<data.size; i++) {
		data.heights[i] = defaultHeight();
	}

	m_default = new HMRegion(&data, 0, regionSize());
}

void HMImageSource::buildImage() {
	std::cout << "Building Heightmap\n";
	TextureLoader *l = Loader::create<TextureLoader>(file());
	if (l == 0) {
		m_valid = false;
		Error(0, "HMImageSource must have a valid texture file");
		return;
	}

	if (!l->load()) {
		m_valid = false;
		return;
	}

	if (l->width() != l->height()) {
		Error(0, "A heightmap image must be square.");
		m_valid = false;
		return;
	}

	if (countBits(l->width()) != 1) {
		Error(0, "A heightmap image must be a power of 2.");
		m_valid = false;
		return;
	}

	//Now have an array of bytes...
	HMData data;
	regionSize(l->width());
	data.size = (regionSize()+1) * (regionSize()+1);
	int size2 = regionSize() * regionSize();
	int j = 0;
	data.heights = new float[data.size];
	data.material = get(ix::material);

//	int index;
	int bpp = (l->format() == RGBA) ? 4 : 3;

	for (int i=0; i<size2; i++) {
		data.heights[j++] = convertData((char*)&l->data()[i*bpp]);
		//Leave a gap for extra row
		if((i % regionSize()) == (regionSize()-1)) j++;
	}

	//test - smoothing to reduce artefacts
	int rs = regionSize() + 1;
	for(int i=0; i<rs-1; i++) {
		for(int k=0; k<rs-1; k++) {
			float h = data.heights[i + k*rs];
			float h2 = h;
			if(i>0)    h+= data.heights[i-1 + k*rs]; else h+=h2;
			if(i<rs-2) h+= data.heights[i+1 + k*rs]; else h+=h2;
			if(k>0)    h+= data.heights[i + (k-1)*rs]; else h+=h2;
			if(k<rs-2) h+= data.heights[i + (k+1)*rs]; else h+=h2;
			data.heights[i + k*rs] = h / 5.0f;
		}
	}

	//Add extra row of default heights.
	int st1 = regionSize() * (regionSize()+1);
	for(int i=0; i<regionSize()+1; i++) {
		if(repeat()) {
			//if repeat is set, wrap the end values
			data.heights[i + st1] = data.heights[i];
			data.heights[(i+1) * (regionSize()+1) -1] = data.heights[i * (regionSize()+1)];
		} else {
			data.heights[i + st1] = defaultHeight();
			data.heights[(i+1) * (regionSize()+1) -1] = defaultHeight();
		
			//because this is the only region with height data, set the other two sides to defaultheight
			//to save the other regions needing to access this one to get the extra row and column of heights
			data.heights[i] = defaultHeight();
			data.heights[i * (regionSize()+1)] = defaultHeight();
		}
	}
	
	//Now build material array.
	std::vector<Material*> temp;
	int i=0;
	OID matid;
	OID mats = get(ix::materials);

	if (mats != Null) {
		while (true) {
			matid = mats.get(i++);
			if (matid == Null) break;
			temp.push_back(matid);
		}
	
		data.nMat = temp.size();
		data.materials = new Material*[temp.size()];
		for (i=0; i<data.nMat; i++) {
			data.materials[i] = temp[i];
		}
	} else {
		data.nMat=0;
	}

	m_image = new HMRegion(&data, 0, regionSize());
	//delete [] data.heights;
	//delete [] data.materials;
}

int HMImageSource::countBits(int num) {
	int count = 0;
	for (int i=0; i<31; i++) {
		count += (num >> i) & 1;
	}
	return count;
}

float HMImageSource::convertData(char *data) {
	//For now assume r component only
	return (float)(unsigned char)data[0] / 128.0f;
}

