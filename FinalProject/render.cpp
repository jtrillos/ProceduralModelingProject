#include "render.h"

// global variables
vector<modeler*> dataTree;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(10.0f, 35.0f, -15.0f));

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

	// build and compile shaders, objects and textures
	// -------------------------
	Shader ourShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
	Object planeObject("resources/objects/plane/plane.obj");
	Object cubeObject("resources/objects/cube/cube.obj");
	Object cylinderObject("resources/objects/cylinder/cylinder.obj");
	Object sofaObject("resources/objects/sofa/sofa.obj");
	Object tableObject("resources/objects/table/table.obj");
	Object cabinetObject("resources/objects/cabinet/cabinet.obj");
	Object chairObject("resources/objects/armchair/armchair.obj");
	Object toyObject("resources/objects/toy/toy.obj");
	map<string, unsigned int> textures = compileTextures();

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

		// render the loaded models
		for (int i = 0; i < dataTree.size(); i++) {
			modeler* mod = dataTree[i];
			vector3d& position = mod->getSymbolPosition();
			vector3d& size = mod->getSize();
			unsigned int tex;
			if (textures.find(mod->getTexture()) != textures.end()) {
				tex = textures.at(mod->getTexture());
			}
			switch (mod->getType()) {
			case CUBE:
				drawCube(position, size, ourShader, cubeObject, tex);
				break;
			case CYLINDER:
				drawCylinder(position, size, ourShader, cylinderObject, tex);
				break;
			case PLANE:
				drawCube(position, size, ourShader, cubeObject, tex);
				break;
			case SOFA:
				drawSofa(position, size, ourShader, sofaObject, tex);
				break;
			case TABLE:
				drawTable(position, size, ourShader, tableObject, tex);
				break;
			case CABINET:
				drawCabinet(position, size, ourShader, cabinetObject, tex);
				break;
			case CHAIR:
				drawChair(position, size, ourShader, chairObject, tex);
				break;
			case TOY:
				drawToy(position, size, ourShader, toyObject, tex);
				break;
			}
		}
		
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

void render::drawPlane(vector3d& position, vector3d& size, Shader ourShader, Object planeObject, unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	planeObject.Draw(ourShader);
}

void render::drawCube(vector3d& position, vector3d& size, Shader ourShader, Object cubeObject, unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	cubeObject.Draw(ourShader);
}

void render::drawCylinder(vector3d& position, vector3d& size, Shader ourShader, Object cylinderObject, unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	cylinderObject.Draw(ourShader);
}

void render::drawSofa(vector3d& position, vector3d& size, Shader ourShader, Object sofaObject, unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	sofaObject.Draw(ourShader);
}

void render::drawTable(vector3d& position, vector3d& size, Shader ourShader, Object tableObject, unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	tableObject.Draw(ourShader);
}

void render::drawCabinet(vector3d& position, vector3d& size, Shader ourShader, Object cabinetObject, unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	cabinetObject.Draw(ourShader);
}

void render::drawChair(vector3d& position, vector3d& size, Shader ourShader, Object chairObject, unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	chairObject.Draw(ourShader);
}

void render::drawToy(vector3d& position, vector3d& size, Shader ourShader, Object toyObject, unsigned int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.getX(), position.getY(), position.getZ())); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(size.getX(), size.getY(), size.getZ()));	// it's a bit too big for our scene, so scale it down
	ourShader.setMat4("model", model);
	toyObject.Draw(ourShader);
}

// load and create textures
map<string, unsigned int> render::compileTextures() {
	map<string, unsigned int> textures;
	textures.insert(std::pair<string, unsigned int>("armchair", TextureFromFile("armchair.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("awesomeface", TextureFromFile("awesomeface.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("door", TextureFromFile("door.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("floor", TextureFromFile("floor.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("metal", TextureFromFile("metal.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("picture", TextureFromFile("picture.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("rug", TextureFromFile("rug.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("sofa", TextureFromFile("sofa.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("table", TextureFromFile("table.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("tv", TextureFromFile("tv.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("wall", TextureFromFile("wall.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("wall1", TextureFromFile("wall1.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("wall2", TextureFromFile("wall2.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("wall3", TextureFromFile("wall3.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("window", TextureFromFile("window.bmp", "resources/textures", false)));
	textures.insert(std::pair<string, unsigned int>("wood", TextureFromFile("wood.bmp", "resources/textures", false)));
	return textures;
}