#include <wgd/texture.h>
#include <wgd/sprite.h>
#include <wgd/font.h>
#include <wgd/spritefont.h>
#include <cadence/doste/doste.h>
#include <cadence/object.h>
#include <wgd/material.h>
#include <wgd/shader.h>
#include <wgd/rendertarget.h>

using namespace cadence;
using namespace cadence::doste;

extern "C" void RESIMPORT initialise(const OID &base) {
	Object::registerType<wgd::Texture>();
	Object::registerType<wgd::Sprite>();
	Object::registerType<wgd::Font>();
	Object::registerType<wgd::SpriteFont>();
	Object::registerType<wgd::Material>();
	Object::registerType<wgd::Shader>();
	Object::registerType<wgd::RenderTarget>();

	wgd::Texture::initialise();
	wgd::Material::initialise();
}

extern "C" void RESIMPORT finalise() {

} 
