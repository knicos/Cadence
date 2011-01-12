#include <wgd/texturetga.h>
#include <wgd/texture.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

bool TextureTGA::validate(File *f) {
	unsigned char TGAheaderN[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
	unsigned char TGAheaderC[12] = {0,0,10,0,0,0,0,0,0,0,0,0};
	unsigned char TGAcompare[12];
	f->read(TGAcompare);
	f->seek(0, File::BEG);

	if ((memcmp(TGAheaderC, TGAcompare, 12) == 0) || (memcmp(TGAheaderN, TGAcompare, 12) == 0)) {
		return true;
	} else {
		return false;
	}
}

bool TextureTGA::load() {
	unsigned char TGAheaderC[12] = {0,0,10,0,0,0,0,0,0,0,0,0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesperpixel;
	unsigned int imagesize;
	unsigned int temp;

	int mwidth;
	int mheight;
	int mcdepth;
	unsigned char *mdata;

	File *f = file();

	f->open(File::READ);
	f->read(TGAcompare);

	//Read the header and extract width and height
	f->read(header);
	mwidth = header[1] * 256 + header[0];
	mheight = header[3] * 256 + header[2];

	//Colour depth, bpp
	mcdepth = header[4];
	bytesperpixel = mcdepth / 8;
	imagesize = mwidth * mheight * bytesperpixel;
	
	//Must be either 24 or 32 bpp.
	if (header[4] != 24 && header[4] != 32 && header[4] != 8)
	{
		std::cout << "Invalid TGA file\n";
		f->close();
		return false;
	}

	mdata = new unsigned char[imagesize+4];

	if (memcmp(TGAheaderC, TGAcompare, 12) == 0) {
		//TGA is compressed with RLE coding.

		//Buffer to store entire file... faster
		int curpos = f->tell();
		int fsize = f->size() - curpos;
		f->seek(curpos, File::BEG);
		char *databuf = new char[fsize];
		f->read(databuf, fsize);
		curpos = 0;
		
		int pixcount = mwidth*mheight;
		int curpix = 0;
		int curbyte = 0;
		char colbuf[4];	//Max 4 bytes per pixel.

		unsigned char chunk;
		do {
			//f->read(chunk);
			chunk = databuf[curpos++];

			if (chunk < 128) {
				//This is a raw chunk.
				chunk++;
				//f->read((char*)&mdata[curbyte], bytesperpixel*chunk);
				memcpy((char*)&mdata[curbyte], &databuf[curpos], bytesperpixel*chunk);
				curpos += bytesperpixel*chunk;

				if (mcdepth >= 24) {
					for (int i=0; i<chunk; i++) {
						temp = mdata[curbyte];
						mdata[curbyte] = mdata[curbyte+2];
						mdata[curbyte+2] = temp;
						curbyte += bytesperpixel;
					}
				}
				curpix += chunk;
			} else {
				//Same colour for a number of pixels.
				chunk -= 127;

				//f->read((char*)colbuf, bytesperpixel);
				memcpy((char*)colbuf, &databuf[curpos], bytesperpixel);
				curpos += bytesperpixel;

				for (int i=0; i<chunk; i++) {

					if (bytesperpixel >= 3) {
						mdata[curbyte] = colbuf[2];
						mdata[curbyte+1] = colbuf[1];
						mdata[curbyte+2] = colbuf[0];
	
						if (bytesperpixel == 4) {
							mdata[curbyte+3] = colbuf[3];
						}
					} else {
						mdata[curbyte] = colbuf[0];
					}

					curbyte += bytesperpixel;
					curpix++;
				}
			}
		} while (curpix < pixcount);

		delete [] databuf;

	} else {
		//TGA is uncompressed so read raw data
		f->read((char*)mdata, imagesize);
	
		//Swap bgr to rgb
		if (bytesperpixel >= 3) {
			for (unsigned int i=0; i<imagesize; i+= bytesperpixel)
			{
				temp=mdata[i];
				mdata[i] = mdata[i+2];
				mdata[i+2] = temp;
			}
		}
	}

	f->close();

	data(mdata);
	width(mwidth);
	height(mheight);

	switch (mcdepth) {
	case 8:		format(ALPHA); break;
	case 24:	format(RGB); break;
	case 32:	format(RGBA); break;
	default: Error(0, "There be something wrong with this tga file.");
	}

	return true;
}
