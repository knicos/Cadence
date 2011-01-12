#include <wgd/dll.h>
#include <cadence/doste/oid.h>
#include <wgd/scene3d.h>
#include <wgd/camera3d.h>
#include <wgd/iline.h>
#include <wgd/isprite3d.h>
#include <wgd/iprimitive.h>
#include <wgd/text3d.h>

extern "C" void BASE3DIMPORT initialise(const cadence::doste::OID &base) {
	cadence::Object::registerType<wgd::Scene3D>();
	cadence::Object::registerType<wgd::Camera3D>();
	cadence::Object::registerType<wgd::ILine3D>();
	cadence::Object::registerType<wgd::ISprite3D>();
	cadence::Object::registerType<wgd::IPrimitive3D>();
	cadence::Object::registerType<wgd::IText3D>();
}

extern "C" void BASE3DIMPORT finalise() {	
}
