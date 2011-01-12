#include <wgd/shader.h>
#include <wgd/vector.h>
#include <wgd/texture.h>
#include <wgd/index.h>
#include <wgd/extensions.h>

#ifdef WIN32
#include <windows.h>
#define GLAPIENTRY APIENTRY
#endif

#ifdef LINUX
#include <GL/glx.h>
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

typedef char GLchar;

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

bool Shader::s_available = true;
Shader *Shader::s_current=NULL;

namespace wgd {
	OnEvent(Shader, evt_reload) {
		if (s_available && m_ready) {
			if(m_vertexShader) 	WGDglDeleteShader(m_vertexShader);
			if(m_fragmentShader) 	WGDglDeleteShader(m_fragmentShader);
			if(m_program) 		WGDglDeleteProgram(m_program);
		}

		m_loaded = false;
		m_ready = false;
	}

	IMPLEMENT_EVENTS(Shader, Agent);
};
Shader::Shader() : m_ready(false), m_loaded(false), m_tangents(false) {
	m_vars = OID::create();
	registerEvents();
}
Shader::Shader(cadence::File &v, cadence::File &f) : m_ready(false), m_loaded(false), m_tangents(false) {
	vert(v);
	frag(f);
	m_vars = OID::create();
	registerEvents();
}
Shader::Shader(const char* v, const char* f) : m_ready(false), m_loaded(false), m_tangents(false) {
	vert(new LocalFile(v));
	frag(new LocalFile(f));
	m_vars = OID::create();
	registerEvents();
}
Shader::Shader(const OID &res)
	: Agent(res), m_ready(false), m_loaded(false), m_tangents(false) {
	m_vars = OID::create();
	registerEvents();
}

Shader::~Shader(){
	if (s_available) {
		if(m_vertexShader) 	WGDglDeleteShader(m_vertexShader);
		if(m_fragmentShader) 	WGDglDeleteShader(m_fragmentShader);
		if(m_program) 		WGDglDeleteProgram(m_program);
	}	
}

void Shader::current(Shader *s) {
	s_current = s;
}

Shader *Shader::current() { return s_current; }

void Shader::initialise() {
	//are shaders supported?
	s_available=(WGDglCreateShader!=0 && Extensions::hasShaders());
}

void Shader::enabled(bool v) {
}

bool Shader::enabled() {
	return true;
}

bool Shader::load(){
	//if(!s_available) wgd::cout << wgd::WARN << "Shaders not supported!\n";
	if (!enabled()) {
		m_ready = false;
		return false;
	}
	
	return loadShader();
}

bool Shader::loadShader(){
	if(!s_available) return false;

	char *vsBuffer, *fsBuffer;	
	
	//Read files
	Info(Info::LOADING, "Loading vertex shader " + vert()->filename());
	vsBuffer = readFile(vert());
	if(vsBuffer==NULL){ Error(0, "Vertex shader " + vert()->filename() + " failed to load\n"); return false; }
	
	
	Info(Info::LOADING, "Loading fragment shader " + frag()->filename());
	fsBuffer = readFile(frag());
	if(fsBuffer==NULL){ Error(0, "Fragment shader " + frag()->filename() + " failed to load\n"); return false; }
	
	//compile the shader
	bool r = make(vsBuffer, fsBuffer);
	
	if(vsBuffer!=NULL) delete [] vsBuffer;
	if(fsBuffer!=NULL) delete [] fsBuffer;
	
	return r;
}

bool Shader::make(const char *vert, const char *frag){
	if(!s_available) return false;
	if (WGDglCreateShader == 0) return false;

	m_loaded=true;	
	
	//Set up shaders 
	const char * vs = vert;
	const char * fs = frag;

	m_vertexShader = WGDglCreateShader(GL_VERTEX_SHADER);
	m_fragmentShader = WGDglCreateShader(GL_FRAGMENT_SHADER);
	
	WGDglShaderSource(m_vertexShader,  1, &vs, NULL);
	WGDglShaderSource(m_fragmentShader, 1, &fs, NULL);
	
	//compile shaders
	WGDglCompileShader(m_vertexShader);
	WGDglCompileShader(m_fragmentShader);
	
	//detect any compile errors
	if(logInfo(m_vertexShader, "Vertex shader")<0){
		Error(0, "The vertex shader failed to compile");
		return false;
	}
	if(logInfo(m_fragmentShader, "Fragment shader")<0){
		Error(0, "The fragment shader failed to compile");
		return false;
	}
	
	//Create the shader program
	m_program = WGDglCreateProgram();
	
	WGDglAttachShader(m_program, m_vertexShader);
	WGDglAttachShader(m_program, m_fragmentShader);
	
	WGDglLinkProgram(m_program);
	
	//test error?
	//GLenum err = glGetError();
	//if(err!=0) cout << wgd::ERR << "Shader : " << err << "\n";	
	
	
	int r = logInfo(m_program, "Shader Program");
	if(r<0) {
		if (r==-3) Error(0, "The shader is too large to run on your hardware");
		else Error(0, "The shaders failed to link");
		return false;
	}

	//bad things may happen
	if(m_program==0) return false;
	if(m_vertexShader==0) return false;
	if(m_fragmentShader==0) return false;
	
	//See if the shader uses the 'tangent' attribute variable
	GLint loc = WGDglGetAttribLocation(m_program, "tangent");	
	
	if(loc>=0){
		 m_tangents=true;
		 //if(Shader::debug()) wgd::cout << wgd::DEV << "Detected 'tangent' variable at " << loc << "\n";
		 //WGDglBindAttribLocation(m_program, loc, "tangent");
		 //does this fix it?
		 m_vars.set("tangent", (void*)(new ShaderVar(2, loc)));
	}

	//yay - the shader works!
	m_ready=true;
	
	return true;
}

char *Shader::readFile(File *f){

	if (f == 0 || !f->open(File::READ)) {
		return 0;
	}

	char *content = new char[f->size()+1];
	int count = f->read(content, f->size());
	content[count] = 0;
	
	f->close();
	return content;
}

int Shader::logInfo(GLuint s, const char *name){
    
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;
	int r=0;
	
	//get length of shader info log
	if(s==m_program) WGDglGetProgramiv(s, GL_INFO_LOG_LENGTH, &infologLength);
	else WGDglGetShaderiv(s, GL_INFO_LOG_LENGTH, &infologLength);
	
	//output the log if it exists
	if (infologLength > 0){
		//length 1 = success
		if(infologLength==1){
			//if(s==m_program && Shader::debug()) cout << DEV << name << ": OK\n";
			return r;
		}
		
		infoLog = (char *)malloc(infologLength);
		if(s==m_program) WGDglGetProgramInfoLog(s, infologLength, &charsWritten, infoLog);
		else WGDglGetShaderInfoLog(s, infologLength, &charsWritten, infoLog);
		
		//if(Shader::debug()) wgd::cout << wgd::DEV << name << ": " << infoLog << "\n";
		//Debug(0, dstring(name) + ": " + infoLog);
		std::cout << name << ": " << infoLog << "\n";
		//If the shader is too large to run in hardware
		if(strstr(infoLog, "software")) r = -3;
		//any linker errors
		if(strstr(infoLog, "Error")) r = -2;
		//compile errors
		if(strstr(infoLog, "ERROR")) r = -1;
		delete [] infoLog;
	}
	return r;
}

GLint Shader::addVariable(const char *name){
	if(!m_ready) return -1;
	//look in uniform, then attribute to find tha variable
	//and store the variable location for future reference
	GLint loc=0;
	
	//uniform...
	loc = WGDglGetUniformLocation(m_program, name);
	if(loc>=0){
		//if(Shader::debug()) wgd::cout << wgd::DBG << "Uniform Variable '" << name << "' at " << loc << "\n";
		m_vars.set(name, (void*)(new ShaderVar(1, loc)));
		return loc;	
	} else{
		//look in varying variables
		loc = WGDglGetAttribLocation(m_program, name);
		//wgd::cout << wgd::DEV << "program " << m_program <<  "   '" << name << "' = " << loc << "\n";
		if(loc>=0){
			m_vars.set(name, (void*)(new ShaderVar(2, loc)));
			//if(Shader::debug()) wgd::cout << wgd::DBG << "Varying Variable '" << name << "' at " << loc << "\n";
			return loc;	
		} else{
			//cant find the variable
			//wgd::cout << wgd::WARN << "Variable '" << name << "' does not exist " << glGetError() << "\n";
		}
	}	
	return -1;	
}

//set variables here
Shader::ShaderVar *Shader::getVar(const char *name){
	ShaderVar *v = (ShaderVar*)(void*)m_vars[name];
	if(v==NULL){
		//attempt to find it!
		addVariable(name);
		v = (ShaderVar*)(void*)m_vars[name];
	}
	return v;
}

void Shader::setVariable(const char *name, float v1){	
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL){
		if(v->type==1) WGDglUniform1f(v->location, v1); 
		if(v->type==2) WGDglVertexAttrib1f(v->location, v1); 
	}
	
	//GLenum err = glGetError();
	//if(err!=0) cout << wgd::ERR << "Shader : " << err << "\n";
}

void Shader::setVariable(const char *name, float v1, float v2){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL){
		if(v->type==1) WGDglUniform2f(v->location, v1, v2); 
		if(v->type==2) WGDglVertexAttrib2f(v->location, v1, v2); 
	}
}
void Shader::setVariable(const char *name, float v1, float v2, float v3){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL){
		if(v->type==1) WGDglUniform3f(v->location, v1, v2, v3); 
		if(v->type==2) WGDglVertexAttrib3f(v->location, v1, v2, v3); 
	}
}
void Shader::setVariable(const char *name, float v1, float v2, float v3, float v4){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL){
		if(v->type==1) WGDglUniform4f(v->location, v1, v2, v3, v4); 
		if(v->type==2) WGDglVertexAttrib4f(v->location, v1, v2, v3, v4); 
	}
}
void Shader::setVariable(const char *name, const wgd::vector3d &vec3){
	if(!m_ready) return;
	setVariable(name, vec3.x, vec3.y, vec3.z);
}


void Shader::setVariable(const char *name, int v1){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL){
		if(v->type==1) WGDglUniform1i(v->location, v1); 
		//if(v->type==2) wgd::cout << wgd::WARN << "Cant have integer Attribute variables.\n";
	}
}

void Shader::setVariable(const char *name, int size, int* data){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL){
		if(v->type==1) WGDglUniform1iv(v->location, size, data); 
		//if(v->type==2) wgd::cout << wgd::WARN << "Cant have integer Attribute variables.\n";
	}
}

void Shader::setVariable(const char *name, int size, float* data){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL){
		if(v->type==1) WGDglUniform1fv(v->location, size, data); 
		//if(v->type==2) wgd::cout << wgd::WARN << "Attribute arrays not implemented.\n";
	}
}

//Attribute Arrays
void Shader::enableAttribArray(const char *name){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL && v->type==2) WGDglEnableVertexAttribArray(v->location);
}
void Shader::disableAttribArray(const char *name){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL && v->type==2) WGDglDisableVertexAttribArray(v->location);	
}
void Shader::attribPointer(const char *name, GLint size, GLenum type, GLboolean normalised, GLsizei stride, const void *pointer){
	if(!m_ready) return;
	ShaderVar *v = getVar(name);
	if(v!=NULL){
		WGDglVertexAttribPointer(v->location, size, type, normalised, stride, pointer);	
	}
}


void Shader::bind(){

	if (!Shader::enabled()) {
		return;
	}

	if(!m_ready) { 
		//load and compile shader if not already tried
		if(s_available && !m_loaded) load();
	}

	if (m_ready) {
		WGDglUseProgram(m_program);

		//set current shader for outside reference
		s_current = this;
	}
		
}

void Shader::unbind(){
	if(s_available && enabled()){
		if (WGDglUseProgram != 0) WGDglUseProgram(0);
	}

	//no shaders bound.
	s_current=NULL;
}

