#include "world.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <cmath>

void ogl_info(GLFWwindow* win);
void ogl_init(GLFWwindow* win);
void ogl_reshape(GLFWwindow* win,int width,int height);
void ogl_display(GLFWwindow* win);

void keyboard(GLFWwindow* win,int key,int s,int act,int mod);
void mouse(GLFWwindow* win,int but,int act,int mod);
void motion(GLFWwindow* win,double x,double y);
void scroll(GLFWwindow* win,double x,double y);

bool rotate_o=false;

using namespace std;

int main(int argc,char* argv[])
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API,GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);

	GLFWwindow* win;
	vector<int> vers = { 46,45,44,43,42,41,40,33 };
	for (auto v : vers)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,v/10);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,v%10);
		win = glfwCreateWindow(780,650,"OpenGL",NULL,NULL);
		if (win!=nullptr)
			break;
	}
	if (win==nullptr)
		return 0;
	ogl_info(win);

	ogl_init(win);

	glfwSetFramebufferSizeCallback(win,ogl_reshape);
	glfwSetWindowRefreshCallback(win,ogl_display);

	glfwSetKeyCallback(win,keyboard);
	glfwSetMouseButtonCallback(win,mouse);
	glfwSetScrollCallback(win,scroll);

	while (!glfwWindowShouldClose(win))
	{
		if(!rotate_o){
			glfwWaitEvents();
		}
		else {
			glfwWaitEventsTimeout(0.016667); // 60 FPS
			world_ph += 0.01;
			ogl_display(win);
		}
	}

	glfwTerminate();

	return 0;
}

void ogl_info(GLFWwindow* win)
{
	glfwMakeContextCurrent(win);

	cout << "GL: " << glGetString(GL_VERSION) << endl;
	cout << "SL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	cout << flush;
}

void ogl_init(GLFWwindow* win)
{
	glfwMakeContextCurrent(win);

	world_init();
}

void ogl_reshape(GLFWwindow* win,int w,int h)
{
	glfwMakeContextCurrent(win);

	world_reshape(w,h);
}

void ogl_display(GLFWwindow* win)
{
	int w,h;
	glfwGetWindowSize(win,&w,&h);

	glfwMakeContextCurrent(win);

	world_display(w,h);

	glfwSwapBuffers(win);
}

void keyboard(GLFWwindow* win,int key,int s,int act,int mod)
{
	if (act==GLFW_RELEASE)
		return;

	switch (key)
	{
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(win,GL_TRUE);
			break;
		case GLFW_KEY_LEFT:
			world_ph += 0.1;
			break;
		case GLFW_KEY_RIGHT:
			world_ph -= 0.1;
			break;
		case GLFW_KEY_DOWN:
			world_th += 0.1;
			if (world_th>0.49*M_PI)
				world_th = 0.49*M_PI;
			break;
		case GLFW_KEY_UP:
			world_th -= 0.1;
			if (world_th<-0.49*M_PI)
				world_th = -0.49*M_PI;
			break;
		case GLFW_KEY_R:
			world_ph = 0;
			world_th = 0;
			break;
		case GLFW_KEY_F:
			world_fill = !world_fill;
			break;
		case GLFW_KEY_P:
			rotate_o = !rotate_o;
			break;
		case GLFW_KEY_S:
			world_shading_frag = !world_shading_frag;
			break;
		default:
			cout << "key " << key << "<" << char(key) << ">" << endl;
			break;
	}

	ogl_display(win);
}

double lx,ly;

void mouse(GLFWwindow* win,int but,int act,int mod)
{
	switch(act)
	{
		case GLFW_PRESS:
			glfwGetCursorPos(win,&lx,&ly);
//			glfwSetInputMode(win,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(win,motion);
			break;
		case GLFW_RELEASE:
			glfwSetCursorPosCallback(win,0);
//			glfwSetInputMode(win,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
			break;
	}

	ogl_display(win);
}

void motion(GLFWwindow* win,double x,double y)
{
	double dx = x - lx;
	double dy = y - ly;
	lx = x;
	ly = y;
	world_ph -= 0.02*dx;
	world_th += 0.02*dy;
	if (world_th<-0.49*M_PI)
		world_th = -0.49*M_PI;
	if (world_th>0.49*M_PI)
		world_th = 0.49*M_PI;

	ogl_display(win);
}

void scroll(GLFWwindow* win,double,double z)
{
	if (z<0)
		world_ro *= 0.90;
	else
		world_ro *= 1.10;

	ogl_display(win);
}
