#include <wgd/widgets/widgets.h>
#include <wgd/widgets/wviewport.h>
#include <wgd/window.h>
#include <wgd/mouse.h>
#include <wgd/font.h>
#include <wgd/math.h>

#include <cadence/notation.h>
#include <algorithm>

#undef False

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

wgd::OID WidgetManager::s_focus;
bool WidgetManager::s_rebuild;
Widget *Widget::s_prevmouse = 0;

namespace wgd {
	OnEvent(WidgetManager, evt_mouse) {
		if (get(ix::root) == Null) return;
		int mx = wgd::mouse->x();
		int my = wgd::mouse->y();

		Widget *root = get(ix::root);
		if (root != 0) root->mouse(mx,my);
		
		set("over", s_focus, true);
	}
	
	OnEvent(WidgetManager, evt_focus) {
		if(get(ix::focus)!=Null) {
			Widget *cw = get(ix::focus);
			if(cw) cw->setfocus();
		}
	}
	
	IMPLEMENT_EVENTS(WidgetManager, Agent);
};

wgd::WidgetManager::WidgetManager(const OID &o) : Agent(o) {

	s_focus = Null;
	s_rebuild=true;

	Object::registerType<wgd::Widget>();
	Object::registerType<wgd::WViewport>();
	registerEvents();

	//std::cout << "Made widget manager\n";
}

wgd::WidgetManager::~WidgetManager() {
}

void wgd::WidgetManager::draw() {	
	//Prevent the changing of the depth buffer.
	glDepthMask(0);

	glDisable(GL_LIGHTING);
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glScissor(0,0,wgd::window->width(),wgd::window->height());
	glMatrixMode(GL_PROJECTION);						
	glPushMatrix();								// Store The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix
	glOrtho(0,wgd::window->width(),0,wgd::window->height(),-1,1);		// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	//create clip rect
	ClipRect r; 
	r.top=0; r.left=0;
	r.right = window->width();
	r.bottom = window->height();
	
	//Draw widgets
	if (get(ix::root) != Null) {
		Widget *root = getObject<Widget>("root");
		if (root != 0) {
			root->draw(r);
		}
	}
	
	glPopMatrix();	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();					// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);

	//Lighting::enabled(lightstate);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	
	//Allow depth buffer to be changed again
	glDepthMask(1);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

wgd::Widget::Widget(const cadence::doste::OID &id) : Agent(id) {
	m_remake = true;
	
	m_vertices = NULL;
	m_indices = NULL;
	m_nIndices=0;
	m_nVertices=0;
	
	//add screen position to database
	_DASM( .screenx is { if(.parent == null) { ..x } else { ..parent screenx + (..x) } } );
	_DASM( .screeny is { if(.parent == null) { ..y } else { ..parent screeny + (..y) } } );
	
	//mouseover
	_DASM( this mouseover is {this == (@root widgets over) } );
	
	//Register Triggers
	registerEvents();
}

wgd::Widget::~Widget() {
	if(m_vertices!=NULL) delete [] m_vertices;
	if(m_indices!=NULL) delete [] m_indices;
}
void wgd::Widget::event(const cadence::doste::OID& type){
	//cause any triggers to fire
	property(type, true); property(type, false);
}

void wgd::Widget::make(int x, int y, int w, int h){
	property(ix::x, x);
	property(ix::y, y);
	width(w);
	height(h);
}

extern WidgetManager *wm;

void wgd::Widget::update(){
}

namespace wgd {
	OnEvent(Widget, evt_changed) {
		m_remake = true;
	}
	
	OnEvent(Widget, evt_frame) {
		m_remake = true;
	}

	OnEvent(Widget, evt_text) {
		resizeText();
	}
	
	OnEvent(Widget, evt_child) {
		//Could mean that an object changed or
		//was deleted, but for now assume it was added
		//NOTE: THIS IS A BUG THAT CAN OCCUR IF A SCRIPT IS RUN MULTIPLE TIMES.
		
		Widget *child;
		OID children = get(ix::children);
		m_focus.clear();
		//Check for all existing children
		for (OID::iterator i=children.begin(); i!=children.end(); i++) {
			if ((*i) == This) continue;
			child = (Widget*) children.get(*i);
			if (child != 0){
				child->set(ix::parent, *this, true);
				
				int z = child->get(ix::z);
				addFocus(child);
				//m_focus.push_front(child);
			}
		}
	}

	IMPLEMENT_EVENTS(Widget, Agent);
};

void wgd::Widget::addFocus(Widget *w) {
	int z = w->get(ix::z);
	for (std::list<Widget*>::iterator i=m_focus.begin(); i!=m_focus.end(); i++) {
		int wz = (*i)->get(ix::z);
		if(z >= wz) {
			m_focus.insert(i, w);
			return;
		}
	}
	//stick it on the end if not added
	m_focus.push_back(w);
}

void wgd::Widget::resizeText() {
	if (get(ix::font) == Null) return;
	Font *fnt = font();
	if (fnt == 0) return;

	int twidth=0;
	int theight=0;
	cadence::dstring t = caption();
	int len = t.size();

	for(int i=0; i<len; i++){
		twidth+=fnt->CharWidth(t[i]);
		theight+=fnt->CharHeight(t[i]);
	}

	if (len > 0)
		theight/=len;

	set("textwidth", twidth, true);
	set("textheight", theight, true);
}

bool wgd::Widget::mouse(int mx, int my) {
	vector2d pos = screenPosition();

	if ((pos.x <= mx) && ((pos.x+width()) >= mx) && (pos.y <= my) && ((pos.y+height()) >= my)) {
		//OID children = get(ix::children);
		bool found = false;

		if (m_focus.size()) {
			Widget *child;
			//For each child.
			//for (OID::iterator i=children.begin(); i!=children.end(); i++) {
			for (std::list<Widget*>::iterator i=m_focus.begin(); i!=m_focus.end(); i++) {
				child = *i;
				if (child->visible()) {
					if (child->mouse(mx,my)) {
						found = true;
						break;
					}
				}
			}
		}

		if (!found) {
			//if (s_prevmouse != this) {
				//if (s_prevmouse != 0) s_prevmouse->set("mouseover", false);
				//std::cout << "Changing mouseover\n";
				//set("mouseover", true);
				//s_prevmouse = this;
				WidgetManager::s_focus = *this;
			//}
		}
		return true;
	} //else {
		//Set all children to mouseover false.
		//set("mouseover", false);
	//}
	return false;
}

void wgd::Widget::setfocus() {
	//recursively make focus
	if(get(ix::parent)!=Null){
		Widget *parent = get(ix::parent);
		if(parent) {
			//set this widget as the fucused widget
			parent->makefocus(this);
			
			//recursively focus parent widget
			parent->setfocus();
		}
	}
}

void wgd::Widget::makefocus(Widget *w) {
	if (m_focus.front() == w) return;
	
	for (std::list<Widget*>::iterator i=m_focus.begin(); i!=m_focus.end(); i++) {
		if ((*i) == w) {
			m_focus.erase(i);
			break;
		}
	}
	//bring as far forward as it can go
	addFocus(w);
}

void wgd::Widget::draw(const ClipRect &rect){

	int glerr;
	if ((glerr = glGetError()) != 0) {
		DMsg msg(DMsg::ERROR);
		msg << "OpenGL Error: " << glerr << "\n";
	}

	//build vertex array
	if(m_remake && get(ix::sprite) != Null){
		clearFrame();
		addFrame(0, 0, width(), height(), frame(), border());
	}
	
	if (get(ix::sprite) != Null)
		drawSprite();
	if (get(ix::caption) != Null && get(ix::font) != Null) {
		if(get("textcolour")!=Null){
			colour col = textColour();
			glColor4f(col.r,col.g,col.b,col.a);
		} else {
			glColor4f(0.0f,0.0f,0.0f,1.0f);
		}
		drawText(textX(), textY(), caption(), font());
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	
	if (m_remake) m_remake = false;

	drawChildren(rect);
	
}

void wgd::Widget::drawChildren(const ClipRect &rect) {
	OID children = get(ix::children);
	if (children == Null) return;
	Widget *child;
	
	ClipRect r, rchild;
	r.top = (int)get(ix::screeny);
	r.left = (int)get(ix::screenx);
	r.right = r.left + width();
	r.bottom = r.top + height();
	
	
	for (std::list<Widget*>::reverse_iterator i=m_focus.rbegin(); i!=m_focus.rend(); i++) {
		child = *i;
		if (child != 0 && child->visible()) {
			//scissor text to clip drawing to the widget
			//glScissor(x,wh-y,w,h);
			rchild = clip(rect, r);
			child->draw(rchild);
		}
	}
}

ClipRect wgd::Widget::clip(const ClipRect &base, const ClipRect &r) {
	ClipRect c;
	//Intersect rectangles
	c.top = max(base.top, r.top);
	c.left = max(base.left, r.left);
	c.right = min(base.right, r.right);
	c.bottom = min(base.bottom, r.bottom);
	
	//make sure width and height are positive
	if(c.right < c.left) c.right = c.left;
	if(c.bottom < c.top) c.bottom = c.top;
	
	
	
	//do scissor test
	glScissor(c.left, window->height() - c.bottom, c.right-c.left, c.bottom-c.top);
	return c;
}

void wgd::Widget::clearFrame(){
	if(m_vertices!=NULL) delete [] m_vertices;
	if(m_indices!=NULL) delete [] m_indices;
	m_vertices=NULL;
	m_indices=NULL;
	m_nVertices = 0;
	m_nIndices = 0;
}
void wgd::Widget::addFrame(int x, int y, int cw, int ch, int cframe, int cborder) {
	Sprite *spr = sprite();

	//Does the sprite need flipping?
	bool fx = false;
	bool fy = false;
	
	if(spr!=0){
		fx = spr->fliph();
		fy = !spr->flipv();

		//Get sprite texture coordinates
		//m_tw =  ((float)spr->width()  / (float)spr->texture()->width())  / (float)spr->columns();
		//m_th = -((float)spr->height() / (float)spr->texture()->height()) / (float)spr->rows();
		m_tw = 1.0f / (float)spr->columns();
		m_th = -1.0f / (float)spr->rows();
		m_tx = m_tw * (float)(cframe % spr->columns());
		m_ty = m_th * (float)(cframe / spr->columns());
	}
	

	//calculate offset
	float ox =   (float)x + (float)cw/2.0f;
	float oy = - (float)y - (float)ch/2.0f;
	float w = (float)cw/2.0f;
	float h = (float)ch/2.0f;
	
	//If w and h are not defined - take them from the sprite
	if(spr!=0){
		if(w<1.0) w = (float)spr->width() / 2;
		if(h<1.0) h = (float)spr->height() / 2;			
	}
	
	//Build vertex array
	if(cborder<=0 || spr==0 || spr->texture()==0){
		//Single polygon version

		//resize vertex array
		int j = m_nVertices;
		vertex* tmpv = new vertex[m_nVertices + 4];
		memcpy(tmpv, m_vertices, m_nVertices * sizeof(vertex));	
		if(m_vertices)delete [] m_vertices;
		m_vertices = tmpv;
		m_nVertices+=4;
		
		//add new vertices
		for(int i=0; i<4; i++){
			m_vertices[i+j].x = ox + (i<2? w: -w);
			m_vertices[i+j].y = oy + (i%2? h: -h);
			m_vertices[i+j].u = ((i<2 && !fx) || (!(i<2) && fx) )? m_tx+m_tw : m_tx;
			m_vertices[i+j].v = ((i%2 && !fy) || (!(i%2) && fy) )? m_ty+m_th : m_ty;
		}
		unsigned char tIndices[] = {0, 1, 3, 2};			
		
		//Resize Index array
		unsigned char* tmpi = new unsigned char[m_nIndices + 4];
		memcpy(tmpi, m_indices, m_nIndices);	
		if(m_indices)delete [] m_indices;
		m_indices = tmpi;
		//copy data
		for(int i=0; i<4; i++) m_indices[m_nIndices + i] = tIndices[i] + j;
		m_nIndices+=4;		

		
	} else {
		//edges are separate (texture must exist)
		float b = (float)cborder; 				//Border (in pixels)
		spr->texture()->load();
		float tbx = b / ((float)spr->texture()->width());	//Border (texture units u)
		float tby = -b / ((float)spr->texture()->height());	//Border (texture units v)
		
		//std::cout << "Adding frame: " << spr->texture()->width() << "\n";
		
		//resize vertex array
		int j = m_nVertices;
		vertex* tmpv = new vertex[m_nVertices + 16];
		memcpy(tmpv, m_vertices, m_nVertices * sizeof(vertex));	
		if(m_vertices)delete [] m_vertices;
		m_vertices = tmpv;
		m_nVertices+=16;
		
		//Add new vertices
		for(int i=0; i<4; i++){
			m_vertices[j+i   ].y = oy + h;
			m_vertices[j+i+ 4].y = oy + h-b;
			m_vertices[j+i+ 8].y = oy + b-h;
			m_vertices[j+i+12].y = oy +  -h;
			
			m_vertices[j+i*4  ].x = ox +  -w;
			m_vertices[j+i*4+1].x = ox + b-w;
			m_vertices[j+i*4+2].x = ox + w-b;
			m_vertices[j+i*4+3].x = ox + w;
			
			//UVs
			m_vertices[j+i   ].v = (fy ? m_ty 	  : m_ty+m_th);
			m_vertices[j+i+ 4].v = (fy ? m_ty+tby 	  : m_ty+m_th-tby);
			m_vertices[j+i+ 8].v = (fy ? m_ty+m_th-tby  : m_ty+tby);
			m_vertices[j+i+12].v = (fy ? m_ty+m_th 	  : m_ty);
			
			m_vertices[j+i*4  ].u = (!fx ? m_tx 	  : m_tx+m_tw);
			m_vertices[j+i*4+1].u = (!fx ? m_tx+tbx 	  : m_tx+m_tw-tbx);
			m_vertices[j+i*4+2].u = (!fx ? m_tx+m_tw-tbx : m_tx+tbx);
			m_vertices[j+i*4+3].u = (!fx ? m_tx+m_tw 	  : m_tx);
						
		}
		
		//Create indices
		unsigned char tIndices[] = {	0,4,5,1,	1,5,6,2,	2,6,7,3,   
						4,8,9,5,	5,9,10,6,	6,10,11,7,
						8,12,13,9,	9,13,14,10,	10,14,15,11};
					
		//Resize Index array
		unsigned char* tmpi = new unsigned char[m_nIndices + 9*4];
		memcpy(tmpi, m_indices, m_nIndices);	
		if(m_indices)delete [] m_indices;
		m_indices = tmpi;
		//copy data
		for(int i=0; i<9*4; i++) m_indices[m_nIndices + i] = tIndices[i] + j;
		m_nIndices+=9*4;	

	} 
}

void wgd::Widget::drawSprite(){
	Sprite *spr = sprite();
	if(spr!=0 && spr->texture() != 0) spr->texture()->bind();
	else return;
	
	//translate to position
	glLoadIdentity();
	glRasterPos2f(0.0,0.0);
	vector2d pos = screenPosition();
	pos.y = wgd::window->height() - pos.y;
	glTranslatef(pos.x, pos.y, 0.0);

	//std::cout << "Widget: " << pos.x << "," << pos.y << "\n";

	//Tint - if it exists
	if (get(ix::tint) != Null) {
		colour tin = tint();
		glColor4f(tin.r, tin.g, tin.b, tin.a);
	}

	//Draw the vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	float* pointer = &m_vertices[0].x;
	glVertexPointer  (2, GL_FLOAT, sizeof(vertex), pointer);
	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), pointer+6);
	
	glDrawElements(GL_QUADS, m_nIndices, GL_UNSIGNED_BYTE, m_indices);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	if(spr!=0) spr->texture()->unbind();
}

void wgd::Widget::drawText(int ox, int oy, const char* t, Font* fnt){

	if (fnt == 0) return;

	//Length of string to draw
	int len = strlen(t);
	if(len==0) return; //no point continuing with no string
	
	//This will start at the top left corner, and will truncate.
	glLoadIdentity();
	glRasterPos2f(0.0,0.0);
	
	vector2d pos = screenPosition();
	float posX = pos.x + ox;
	float posY = wgd::window->height() - pos.y - oy;
	glTranslatef((float)(int)posX, (float)(int)posY, 0);

	//std::cout << "Draw text: " << posX << "," << posY << "\n";
	
	fnt->bind();
	if (m_remake) resizeText();
	
	//get font
	//if(fnt==NULL) fnt = Resource::convert<Font>(WGD[ix::console][ix::font]);
	//if (fnt->texture() == 0) return;
		
	float tx,ty,tw,th;
	int cw,ch;
	int x = 0;
	float texw = (float)fnt->texture()->width();
	float texh = (float)fnt->texture()->height();
	//float texw = 256.0f;
	//float texh = 256.0f;

	for (int i=0; i<len; i++) {
		tx = (float)fnt->CharX(t[i]) / texw;
		ty = (float)fnt->CharY(t[i]) / texh;
		cw = fnt->CharWidth(t[i]);
		ch = fnt->CharHeight(t[i]);
		tw = (float)cw / texw;
		th = (float)ch / texh;
		
		//ch may be negative
		if(ch<0) ch = -ch;
		
		//clip text
		//if(!centered && x+cw > width() - ox) break;
		if(x>=0){
			glBegin(GL_QUADS);
				glTexCoord2f(tx, ty+th); 	glVertex2i(x, -ch);
				glTexCoord2f(tx+tw, ty+th); 	glVertex2i(x+cw, -ch);
				glTexCoord2f(tx+tw, ty); 	glVertex2i(x+cw, 0);
				glTexCoord2f(tx, ty); 		glVertex2i(x, 0);
			glEnd();
		}

		x += cw;
	}
	
	fnt->unbind();	
}

WidgetManager *wm;

extern "C" void WIDIMPORT initialise(const cadence::doste::OID &base) {
	Object::registerType<WidgetManager>();
	wm = base;
}

extern "C" void WIDIMPORT cadence_update() {
	wm->draw();
}

extern "C" void WIDIMPORT finalise() {
	//delete wm;
}

