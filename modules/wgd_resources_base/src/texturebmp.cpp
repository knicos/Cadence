#include <wgd/texturebmp.h>
#include <wgd/texture.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

struct BitmapHeader {				/**** BMP file info structure ****/
	unsigned int	biSize;			/* Size of info header */
	int		biWidth;		/* Width of image */
	int		biHeight;		/* Height of image */
	unsigned short	biPlanes;		/* Number of color planes */
	unsigned short	biBitCount;		/* Number of bits per pixel */
	unsigned int	biCompression;		/* Type of compression to use */
	unsigned int	biSizeImage;		/* Size of image data */
	int		biXPelsPerMeter;	/* X pixels per meter */
	int		biYPelsPerMeter;	/* Y pixels per meter */
	unsigned int	biClrUsed;		/* Number of colors used */
	unsigned int	biClrImportant;		/* Number of important colors */
	char *data;
};

bool TextureBMP::validate(File *f) {
	unsigned short check;
	f->read(check);
	f->seek(0, File::BEG);
	return (check == 0x4D42);
}

bool TextureBMP::load() {
	char temp;
	long i;
	
	BitmapHeader infoheader;
	
	file()->open(File::READ);

	//Get the width
	file()->seek(18);
	file()->read(infoheader.biWidth);
	//Get the height
	file()->read(infoheader.biHeight);
	
	file()->read(infoheader.biPlanes);
	if (infoheader.biPlanes != 1) {
		file()->close();
		std::cout << "Bitmap must have only 1 plane\n";
		return false;
	}
	
	// read the bpp
	file()->read(infoheader.biBitCount);
	if (infoheader.biBitCount != 24) {
		file()->close();
		std::cout << "Bitmap does not have 24bit colour\n";
		return false;
	}
	
	file()->seek(24);	

	// read the data.
	infoheader.data = new char[infoheader.biWidth * infoheader.biHeight * 3];
	if ((i = file()->read(infoheader.data, infoheader.biWidth * infoheader.biHeight * 3)) != infoheader.biWidth*infoheader.biHeight*3) {
		std::cout << "Cannot read image data for texture\n";
		file()->close();
		return false;
	}
	
	// reverse all of the colors. (bgr -> rgb)
	for (i=0; i<(infoheader.biWidth * infoheader.biHeight * 3); i+=3) {
		temp = infoheader.data[i];
		infoheader.data[i] = infoheader.data[i+2];
		infoheader.data[i+2] = temp;
	}
	
	//Store the relevant data
	data((unsigned char*)infoheader.data);
	width(infoheader.biWidth);
	height(infoheader.biHeight);
	format(RGB);

	file()->close();
	
	return true;
}
