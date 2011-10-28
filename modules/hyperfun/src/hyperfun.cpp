
#include <wgd/instance3d.h>
#include <wgd/drawable.h>
#include <wgd/material.h>
#include <wgd/vertex.h>
#include <wgd/index.h>
#include <cadence/file.h>
#include <cadence/agent.h>

#include "main.h"
#include "hf_draw.h"
#include "hf_error.h"

using namespace wgd;
using namespace cadence;

class BASE3DIMPORT IHyperFun : public wgd::Instance3D {
		public:
		//IHyperFun();
		IHyperFun(const cadence::doste::OID &o) : Instance3D(o) { registerEvents(); }
		~IHyperFun() {}
		
		/**
		 * Draw this primative.
		 */
		virtual void draw(SceneGraph &graph, Camera3D *camera);

		PROPERTY_RW(cadence::File, file);

		OBJECT(Instance3D, IHyperFun);
		
		BEGIN_EVENTS(Instance3D);
		EVENT(evt_file, (*this)("file"));
		END_EVENTS;
		
		private:
	};

extern CMainApp *edi;

extern int vn_t;
void init_var (CMainApp *mainapp);
	
	
OnEvent(IHyperFun,evt_file) {
	char *argv[2];
	argv[0] = "";
	argv[1] = new char[1000];
	strcpy(argv[1], file()->filename());
	int argc = 2;
	edi = new CMainApp (argc, (char**)argv);

	vn_t = edi->init();
	if (vn_t <= 0){
		if(edi!=NULL){
			delete(edi);
			edi=NULL;
		}
		printf ("Isosurface not generated...\n");
		exit (1);
	}
	init_var(edi);
	delete argv[1];
	
	init_render();
}

IMPLEMENT_EVENTS(IHyperFun,Instance3D);
	
void IHyperFun::draw(SceneGraph &graph, Camera3D *camera) {
	Instance3D::draw(graph, camera);
	localMatrix().apply();
	
	glDisable(GL_CULL_FACE);
	GL_CALL(glCallList(hfObject), ;);
	glEnable(GL_CULL_FACE);
	
	localMatrix().unapply();
}

extern "C" void BASE3DIMPORT initialise(const cadence::doste::OID &base) {
	cadence::Object::registerType<IHyperFun>();
}

extern "C" void BASE3DIMPORT finalise() {	
}

