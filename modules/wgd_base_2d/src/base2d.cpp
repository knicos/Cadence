#include <wgd/dll.h>
#include <cadence/doste/oid.h>
#include <wgd/scene2d.h>
#include <wgd/camera2d.h>
#include <wgd/sprite2d.h>
#include <wgd/iprimitive.h>

extern "C" void BASE2DIMPORT initialise(const cadence::doste::OID &base) {
	cadence::Object::registerType<wgd::Scene2D>();
	cadence::Object::registerType<wgd::Camera2D>();
	cadence::Object::registerType<wgd::ISprite2D>();
	//cadence::Object::registerType<wgd::IPrimitive2D>();
	//cadence::Object::registerType<wgd::IText2D>();
}

extern "C" void BASE2DIMPORT finalise() {	
}
