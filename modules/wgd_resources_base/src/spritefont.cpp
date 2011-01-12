#include <wgd/spritefont.h>

namespace wgd {
	OnEvent(SpriteFont, evt_build) {
		buildFont();
	}
	IMPLEMENT_EVENTS(SpriteFont, Font);
};

using namespace wgd;
using namespace cadence;

SpriteFont::SpriteFont() { registerEvents(); }
SpriteFont::SpriteFont(const OID &id) : Font(id) { registerEvents(); }
SpriteFont::SpriteFont(Sprite *spr) { sprite(spr); registerEvents(); }
SpriteFont::~SpriteFont() {}

void SpriteFont::buildFont() {
	Sprite *spr = sprite();
	dstring s = characters();
	char str[128];
	s.toString(str, 128);
	
	if(!spr || s.size()==0) return;
	memset(m_cdata, 0, 128 * sizeof(CData));
	
	int tw = spr->texture()->width();
	int th = spr->texture()->height();
	int rows = spr->rows();
	int cols = spr->columns();
	
	for(int i=0; i<s.size(); i++) {
		int c = (int)str[i];
		if(c<128) {
			m_cdata[c].x = i % cols * tw / cols;
			m_cdata[c].y = (rows - i / cols) * th / rows;
			m_cdata[c].width = tw / cols;
			m_cdata[c].height = -th / rows;
		}
	}
	
	//unfortunately this will be vertically flipped
	texture(spr->texture());
	
}