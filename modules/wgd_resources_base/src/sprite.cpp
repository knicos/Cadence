/*
 * Warwick Game Design Library
 */

#include <wgd/sprite.h>

using namespace cadence;
using namespace cadence::doste;

wgd::Sprite::Sprite() : Agent() {
	setup();
 }
wgd::Sprite::Sprite(const char* filename) {
	texture(new Texture(new LocalFile(filename)));
	setup();
}
wgd::Sprite::Sprite(File *file) : Agent() {
	texture(new Texture(file));
	setup();
}
wgd::Sprite::Sprite(const OID &res) : Agent(res) {
	setup();
}

void wgd::Sprite::setup() {
	//Initialise defaults
	if (columns() == 0)
		columns(1);		//One column
	if (rows() == 0)
		rows(1);		//One row
	if (frames() == 0)
		frames(1);		//One frame

	if (get(ix::texture) != Null) {		//If there is a texture
		texture()->load();	//Load from file to get width/height data
		if (width() == 0) {
			width(texture()->width());
		}
		if (height() == 0) {
			height(texture()->height());
		}
	}
}

wgd::Sprite::~Sprite() {

}

void wgd::Sprite::initialise() {

}

void wgd::Sprite::finalise() {

}
