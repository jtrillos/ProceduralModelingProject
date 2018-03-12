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
	map<string, unsigned int> compileTextures();
	void drawPlane(vector3d& position, vector3d& size, Shader ourShader, Object planeObject, unsigned int texture);
	void drawCube(vector3d& position, vector3d& size, Shader ourShader, Object cubeObject, unsigned int texture);
	void drawCylinder(vector3d& position, vector3d& size, Shader ourShader, Object cylinderObject, unsigned int texture);
	void drawSofa(vector3d& position, vector3d& size, Shader ourShader, Object sofaObject, unsigned int texture);
	void drawTable(vector3d& position, vector3d& size, Shader ourShader, Object tableObject, unsigned int texture);
	void drawCabinet(vector3d& position, vector3d& size, Shader ourShader, Object cabinetObject, unsigned int texture);
	void drawChair(vector3d& position, vector3d& size, Shader ourShader, Object chairObject, unsigned int texture);
	void drawToy(vector3d& position, vector3d& size, Shader ourShader, Object toyObject, unsigned int texture);
};

#endif