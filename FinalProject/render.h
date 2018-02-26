#ifndef __RENDER_H
#define __RENDER_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "object.h"
#include "common.h"
#include "modeler.h"

#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


class render
{
public:
	bool rendering(vector<modeler*> tree);
	void drawTree(Shader ourShader);
	void drawPlain(vector3d& position, vector3d& size, Shader ourShader, string texture);
	void drawCube(vector3d& position, vector3d& size, Shader ourShader, string texture);
	void drawCylinder(vector3d& position, vector3d& size, Shader ourShader, string texture);
	void drawSofa(vector3d& position, vector3d& size, Shader ourShader, string texture);
	void drawTable(vector3d& position, vector3d& size, Shader ourShader, string texture);
	void drawCabinet(vector3d& position, vector3d& size, Shader ourShader, string texture);
	void drawChair(vector3d& position, vector3d& size, Shader ourShader, string texture);
	void drawToy(vector3d& position, vector3d& size, Shader ourShader, string texture);
};

#endif