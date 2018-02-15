#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#define __gl_h_
	#include <OpenGL/glu.h>
	//#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	//#include <GL/freeglut.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "world.h"
#include "obj.h"

using namespace std;

const char* svtx_src_frag =
	"#version 330 core\n"
	"layout(location=0) in vec3 vpos;\n"
	"layout(location=1) in vec3 vnos;\n"
	"uniform mat4 view;\n"
	"out vec3 pos;\n"
	"out vec3 norm;\n"
	"void main()\n"
	"{\n"
	"   pos = vpos;\n"
	"   norm = vnos;\n"
	"	gl_Position = view*vec4(vpos,1.0);\n"
	"}\n";
GLuint svtx_frag;

const char* sfrg_src_frag =
	"#version 330 core\n"
	"out vec4 color;\n"
	"uniform vec3 light;\n"
	"uniform vec3 camera;\n"
	"in vec3 pos;\n"
	"in vec3 norm;\n"
	"void main()\n"
	"{\n"
	"	vec3 vec_light = normalize(light-pos);\n"
	"	vec3 vec_v = normalize(camera-pos);\n"
	"	float l_power = 1/pow(distance(light,pos),2);\n"
	"	float dot_l = dot(vec_light,normalize(norm));\n"
	"	vec3 difs = dot_l*l_power*vec3(1.0,1.0,1.0);\n" //Kd(N*L)Cd
	"	vec3 amb = vec3(1.0,1.0,1.0);\n" //ka Cd Cl
	"	vec3 spe = dot_l*pow(dot(reflect(-light,normalize(norm)),vec_v),3)*vec3(1.0,1.0,1.0)*l_power;\n" //ks (N*L)(R*V)Cs Cl
	"	color = vec4(0.2*difs+0.3*amb+0.5*spe,1.0);\n"
	"}\n";
GLuint sfrg_frag;

const char* svtx_src_vertex =
	"#version 330 core\n"
	"layout(location=0) in vec3 vpos;\n"
	"layout(location=1) in vec3 vnos;\n"
	"uniform mat4 view;\n"
	"uniform vec3 light;\n"
	"uniform vec3 camera;\n"
	"out vec3 f_color;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = view*vec4(vpos,1.0);\n"
	"	vec3 vec_light = normalize(light-vpos);\n"
	"	vec3 vec_v = normalize(camera-vpos);\n"
	"	float l_power = 1/pow(distance(light,vpos),2);\n"
	"	float dot_l = dot(vec_light,vnos);\n"
	"	vec3 difs = dot_l*l_power*vec3(1.0,1.0,1.0);\n" //Kd(N*L)Cd
	"	vec3 amb = vec3(1.0,1.0,1.0);\n" //ka Cd Cl
	"	vec3 spe = dot_l*pow(dot(reflect(-light,vnos),vec_v),3)*vec3(1.0,1.0,1.0)*l_power;\n" //ks (N*L)(R*V)Cs Cl
	"   f_color = 0.2*difs+0.3*amb+0.5*spe;\n"
	"}\n";
GLuint svtx_vertex;

const char* sfrg_src_vertex =
	"#version 330 core\n"
	"out vec4 color;\n"
	"in vec3 f_color;\n"
	"void main()\n"
	"{\n"
	"	color = vec4(f_color,1.0);\n"
	"}\n";
GLuint sfrg_vertex;

GLuint prog_frag;
GLuint prog_vertex;

GLuint vao;
GLuint vao_sz;

glm::mat4	view;
glm::vec3   light;
GLuint		view_loc_frag;
GLuint		view_loc_vertex;
GLuint light_loc_vertex;
GLuint light_loc_frag;
GLuint camera_loc_vertex;
GLuint camera_loc_frag;

void glcheck(const string& msg)
{
	GLenum err;
	err = glGetError();
	switch(err)
	{
		case GL_NO_ERROR:
		break;
		case GL_INVALID_ENUM:
		cout << msg << " error: Invalid enun" << endl;
		break;
		case GL_INVALID_VALUE:
		cout << msg << " error: Invalid ID value" << endl;
		break;
		case GL_INVALID_OPERATION:
		cout << msg << " error: Invalid operation" << endl;
		break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
		cout << msg << " error: Invalid framebuffer operation" << endl;
		break;
		case GL_OUT_OF_MEMORY:
		cout << msg << " error: Out of memory" << endl;
		break;
		default:
		cout << msg << " error: Undefined" << endl;
		break;
	}
}

OBJ obj;

void world_init()
{
	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));

//	obj.load("../model/cube.obj");
//	obj.load("../model/teapot.obj",xf);
//	obj.load("../model/sphere.obj");
	obj.load("../model/venus.obj",xf);
//	obj.load("../model/bunny.obj",xf);
//	obj.load("../model/dragon.obj",xf);
//	obj.load("../model/armadillo.obj",xf);
//	obj.load("../model/tyra.obj",xf);
//	obj.load("../model/nefertiti.obj");

	cout << obj.faces().size()/3 << endl;

	vao = 0;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	vao_sz = obj.faces().size();
	// Vertices y triangulos
	GLuint vpbo = 0;
	glGenBuffers(1,&vpbo);
	glBindBuffer(GL_ARRAY_BUFFER,vpbo);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.faces().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);
	// Normales
	GLuint vnbo = 0;
	glGenBuffers(1,&vnbo);
	glBindBuffer(GL_ARRAY_BUFFER,vnbo);
	glBufferData(GL_ARRAY_BUFFER,vao_sz*sizeof(glm::vec3),obj.normals().data(),GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),NULL);

	// Shader en fragmento
	svtx_frag = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(svtx_frag,1,&svtx_src_frag,NULL);
	glCompileShader(svtx_frag);

	sfrg_frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sfrg_frag,1,&sfrg_src_frag,NULL);
	glCompileShader(sfrg_frag);

	prog_frag = glCreateProgram();
	glAttachShader(prog_frag,svtx_frag);
	glAttachShader(prog_frag,sfrg_frag);
	glLinkProgram(prog_frag);

	view_loc_frag = glGetUniformLocation(prog_frag,"view");
	light_loc_frag = glGetUniformLocation(prog_frag,"light");
	camera_loc_frag = glGetUniformLocation(prog_frag,"camera");

	// Shader en vertices
	svtx_vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(svtx_vertex,1,&svtx_src_vertex,NULL);
	glCompileShader(svtx_vertex);

	sfrg_vertex = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sfrg_vertex,1,&sfrg_src_vertex,NULL);
	glCompileShader(sfrg_vertex);

	prog_vertex = glCreateProgram();
	glAttachShader(prog_vertex,svtx_vertex);
	glAttachShader(prog_vertex,sfrg_vertex);
	glLinkProgram(prog_vertex);

	view_loc_vertex = glGetUniformLocation(prog_vertex,"view");
	light_loc_vertex = glGetUniformLocation(prog_vertex,"light");
	camera_loc_vertex = glGetUniformLocation(prog_vertex,"camera");

	light = {0.0,1.0,0.0};

	glClearColor(0,0,0,0);
}

void world_reshape(int w,int h)
{
//	cout << w << " x " << h << endl;
	glViewport(0,0,w,h);
}


float world_ph=0;
float world_th=0;
float world_ro=2;

bool world_fill=true;
bool world_shading_frag=true;

void world_display(int w,int h)
{
	if (h<=0) return;
	if (w<=0) return;

	float aspect = float(w)/float(h);

	glm::mat4 pers = glm::perspective(glm::radians(45.0f),aspect,0.01f,1000.0f);

	glm::vec3 axis;
	axis.x = cos(world_ph)*cos(world_th);
	axis.y = sin(world_ph)*cos(world_th);
	axis.z = sin(world_th);

	glm::vec3 to(0,0,0);
	glm::mat4 camera = glm::lookAt(to+world_ro*axis,to,glm::vec3(0,0,1));
	glm::vec3 cam = to+world_ro*axis;
	view = pers*camera;

	glPolygonMode(GL_FRONT_AND_BACK,(world_fill ? GL_FILL : GL_LINE));
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


	if (world_shading_frag){
		glUseProgram(prog_frag);
		glUniform3fv(camera_loc_frag,1, glm::value_ptr(cam));
		glUniform3fv(light_loc_frag,1, glm::value_ptr(cam));
		glUniformMatrix4fv(view_loc_frag,1,GL_FALSE,glm::value_ptr(view));
	} else {
		glUseProgram(prog_vertex);
		glUniform3fv(camera_loc_vertex,1, glm::value_ptr(cam));
		glUniform3fv(light_loc_vertex,1, glm::value_ptr(cam));
		glUniformMatrix4fv(view_loc_vertex,1,GL_FALSE,glm::value_ptr(view));
	}

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES,0,obj.faces().size());
}

void world_clean()
{
}
