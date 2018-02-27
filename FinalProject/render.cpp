#include "render.h"

// global variables
vector<modeler*> dataTree;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(10.0f, 10.0f, 60.0f));

// array containing the pressed keys on the keyboard
// use this to pass the current state of pressed keys to the camera to enable movement
bool keys[1024];

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool render::rendering(vector<modeler*> tree) {
	dataTree = tree;
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Final Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	std::cout << "HOW TO USE IT: \n" << std::endl;
	std::cout << "- Navigation." << std::endl;
	std::cout << "	Arrow keys." << std::endl;
	std::cout << "	(W, A, S, D) keys." << std::endl;
	std::cout << "	Mouse." << std::endl;
	std::cout << "- Press ESC to close application." << std::endl;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

	// load models
	/*Object ourObject("resources/objects/plain/plain.obj");
	TextureFromFile("rug.bmp", "resources/textures", false);
	*/
	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		drawTree(ourShader);
		
		/*glm::mat4 model;
		model = glm::translate(model, glm::vec3(0, 0, 18)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(18, 18, 0));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		ourObject.Draw(ourShader);
		*/
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void render::drawTree(Shader ourShader) {
	for (int i = 0; i < dataTree.size(); i++) {
		modeler* mod = dataTree[i];
		vector3d& position = mod->getSymbolPosition();
		vector3d& size = mod->getSize();
		switch (mod->getType()) {
		case CUBE:
			drawCube(position, size,ourShader, mod->getTexture()+".bmp");
			break;
		case CYLINDER:
			drawCylinder(position, size, ourShader, mod->getTexture() + ".bmp");
			break;
		case PLAIN:
			drawCube(position, size, ourShader, mod->getTexture() + ".bmp");
			break;
		case SOFA:
			drawSofa(position, size, ourShader, mod->getTexture() + ".bmp");
			break;
		case TABLE:
			drawTable(position, size, ourShader, mod->getTexture() + ".bmp");
			break;
		case CABINET:
			drawCabinet(position, size, ourShader, mod->getTexture() + ".bmp");
			break;
		case CHAIR:
			drawChair(position, size, ourShader, mod->getTexture() + ".bmp");
			break;
		case TOY:
			drawToy(position, size, ourShader, mod->getTexture() + ".bmp");
			break;
		}
	}
}

void render::drawPlain(vector3d& position, vector3d& size, Shader ourShader, string texture) {
	const char * c = texture.c_str();
	Object ourObject("resources/objects/plain/plain.obj");
	TextureFromFile(c, "resources/textures", false);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	ourObject.Draw(ourShader);
}

void render::drawCube(vector3d& position, vector3d& size, Shader ourShader, string texture) {
	const char * c = texture.c_str();
	Object ourObject("resources/objects/cube/cube.obj");
	TextureFromFile(c, "resources/textures", false);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	ourObject.Draw(ourShader);
}

void render::drawCylinder(vector3d& position, vector3d& size, Shader ourShader, string texture) {
	const char * c = texture.c_str();
	Object ourObject("resources/objects/cylinder/cylinder.obj");
	TextureFromFile(c, "resources/textures", false);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	ourObject.Draw(ourShader);
}

void render::drawSofa(vector3d& position, vector3d& size, Shader ourShader, string texture) {
	const char * c = texture.c_str();
	Object ourObject("resources/objects/sofa/sofa.obj");
	TextureFromFile(c, "resources/textures", false);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	ourObject.Draw(ourShader);
}

void render::drawTable(vector3d& position, vector3d& size, Shader ourShader, string texture) {
	const char * c = texture.c_str();
	Object ourObject("resources/objects/table/table.obj");
	TextureFromFile(c, "resources/textures", false);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	ourObject.Draw(ourShader);
}

void render::drawCabinet(vector3d& position, vector3d& size, Shader ourShader, string texture) {
	const char * c = texture.c_str();
	Object ourObject("resources/objects/cabinet/cabinet.obj");
	TextureFromFile(c, "resources/textures", false);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	ourObject.Draw(ourShader);
}

void render::drawChair(vector3d& position, vector3d& size, Shader ourShader, string texture) {
	const char * c = texture.c_str();
	Object ourObject("resources/objects/armchair/armchair.obj");
	TextureFromFile(c, "resources/textures", false);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	ourObject.Draw(ourShader);
}

void render::drawToy(vector3d& position, vector3d& size, Shader ourShader, string texture) {
	const char * c = texture.c_str();
	Object ourObject("resources/objects/toy/toy.obj");
	TextureFromFile(c, "resources/textures", false);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	ourObject.Draw(ourShader);
}