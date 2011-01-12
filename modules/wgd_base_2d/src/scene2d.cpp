#include <wgd/scene2d.h>
#include <wgd/iterator.h>
#include <wgd/instance2d.h>

using namespace wgd;

Scene2D::Scene2D(const OID &id) : Scene(id) {
}
Scene2D::~Scene2D() {
}

void Scene2D::update() {
	//updates quadtree
}
void Scene2D::draw(Camera *cam) {
	
	Camera2D *camera = (Camera2D*)(OID)*cam;
	
	OID instances = get(ix::instances);
	for(Iterator<Instance2D> i = instances.begin(); i!=instances.end(); i++) {
		(*i)->draw(graph(), camera);
	}
	m_graph.draw();
}