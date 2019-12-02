#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager.h"

#include "camera.h"
#include "GameObject.h"

#include <iostream>

#include "StringTable.h"
#include "Enums.h"
#include "Point.h"

const float MAX_RENDER_DISTANCE = 10.0f;

class GameEngine {
public:
	static GameEngine &GetInstance();

	void Init();

	void NotifyObjectChanges();

	void StartGame();

	void FinishGame();

	void AddEnemy(const std::string &filepath, const glm::vec3 &scaleVec);
	void AddCoin(const std::string &filepath, const glm::vec3 &scaleVec);

	void SetPlayer(const std::string &filepath, const glm::vec3 &scaleVec);

	void SetScreenPanelHP(const std::string &filepath, const glm::vec3 & scaleVec);
	void SetScreenPanelScore(const std::string &filepath, const glm::vec3 & scaleVec);
	void SetScreenPanelHunger(const std::string &filepath, const glm::vec3 & scaleVec);

	void SetSkybox(const std::vector<std::string> & faces);

	void PrintObjects();

private:
	GameEngine() {} // Since we want only one instance of the engine-> We use an singleton pattern.
	
	/*  Skybox Data  */
	unsigned int _skyboxVAO, _skyboxVBO;
	unsigned int _skyboxTextureID;

	/*  Time Data  */
	float _deltaTime;
	float _lastTime;

	int _frameCounter;// = 0;

	/*  Game Objects */
	std::vector<GameObject*> _enemyObjects;
	std::vector<GameObject*> _coinObjects;

	/*  Player Object*/
	GameObject *_playerObject;

	/* On Screen Panel Objects */
	GameObject *_screenPanelHP;
	GameObject *_screenPanelScore;
	GameObject *_screenPanelHunger;

	/*  Game Window Data  */
	GLFWwindow* _window;
	unsigned int _windowSize[2];
	float _windowRatio;

	/*  Camera Matricies  */
	glm::mat4 _projectionMatrix;
	glm::mat4 _viewMatrix;

	// Debug Controls
	bool _isDebugMode;
	bool _debugPrinter;

	/*  Update Objects  */
	void _Update();

	bool _IsRenderable(GameObject * object);
	/*  Draw & Render Objects  */
	void _Render();
	void _UpdateScreenPanel();
	void _UpdateSkybox();

	/*  Collusion Functions  */
	void _DoBoundryCollusionWith(std::vector<GameObject*> objectList);
	void _DoBoundryCollusion();
	void _DoCollusionEnemy();
	void _DoCollusionCoin();
	void _DoCollusion();

	/*  Process User Input  */
	void _ProcessInput();

	/*  MoveCollider Player With User Input  */
	void _MovePlayer(Directions dir);

	/*  Init GLFW window  */
	void _InitGameWindow();

	/*  GLFW window callbacks  */
	static void _FramebufferSizeCallback(GLFWwindow * window, int width, int height);
	static void _MouseCallback(GLFWwindow * window, double xpos, double ypos);
	static void _ScrollCallback(GLFWwindow * window, double xoffset, double yoffset);
};

/*  Last X and Y positions for camera.  */
float lastX = (float)SCR_WIDTH / 2;
float lastY = (float)SCR_HEIGHT / 2;

/*  Game Camera  */
Camera camera = Camera(glm::vec3(0.0f, 2.0f, 3.0f));
bool isFirstMouse; //= true;

GameEngine& GameEngine::GetInstance()
{
	static GameEngine instance;
	return instance;
}

void GameEngine::Init()
{
	_windowSize[0] = SCR_WIDTH;
	_windowSize[1] = SCR_HEIGHT;

	_windowRatio = (float)_windowSize[0] / (float)_windowSize[1];

	_frameCounter = 0;

	_InitGameWindow();
}

inline void GameEngine::NotifyObjectChanges()
{
	for (int i = 0; i < 100; i++)
	{
		float current_frame = glfwGetTime();

		_deltaTime = current_frame - _lastTime;
		_lastTime = current_frame - _lastTime;

		_DoBoundryCollusion();

		_Update();
	}
	_deltaTime = 0.0f;
	_lastTime = 0.0f;
}

void GameEngine::StartGame()
{
	while (!glfwWindowShouldClose(_window))
	{
		// per-frame time logic
		// --------------------
		float current_frame = glfwGetTime();
		_deltaTime = current_frame - _lastTime;
		_lastTime = current_frame;

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ResourceManager::GetShader(KEY_SHADER_OBJECT).use();

		// Apply collusion to objects
		this->_DoCollusion();

		// view/projection transformations
		if (!_isDebugMode) {
			glm::vec3 camera_pos = _playerObject->model->GetModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			camera.setPosition(camera_pos + glm::vec3(0.0f, 2.0f, -3.0f));
		}


		this->_projectionMatrix = glm::perspective(glm::radians(camera.getZoom()), _windowRatio, 0.1f, 100.0f);		
		this->_viewMatrix = camera.GetViewMatrix();
		
		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("projection", _projectionMatrix);
		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("view", _viewMatrix);

		// Take user inputs
		// ----------------------
		this->_ProcessInput();

		// Update Objects
		// -----------------------
		_Update();

		// Render Objects
		// -----------------------
		_Render();

		// after game objects are rendered, render the on screen panels
		_UpdateScreenPanel();

		// Lastly render skybox.
		_UpdateSkybox();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
}

void GameEngine::FinishGame()
{
	glDeleteVertexArrays(1, &_skyboxVAO);
	//glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &_skyboxVAO);

	glfwTerminate();
}

void GameEngine::AddEnemy(const std::string &filepath, const glm::vec3 &scaleVec = glm::vec3(1.0f))
{
	auto enemy_object = new GameObject(filepath, ObjectType::Enemy);
	
	enemy_object->PlaceRandomly();

	enemy_object->ScaleObject(scaleVec);

	_enemyObjects.push_back(enemy_object);
}

void GameEngine::AddCoin(const std::string &filepath, const glm::vec3 &scaleVec = glm::vec3(1.0f))
{
	auto coin_object = new GameObject(filepath, ObjectType::Coin);
	
	coin_object->PlaceRandomly();
	
	coin_object->ScaleObject(scaleVec);

	_coinObjects.push_back(coin_object);
}

void GameEngine::SetPlayer(const std::string &filepath, const glm::vec3 &scaleVec = glm::vec3(1.0f))
{
	_playerObject = new GameObject(filepath, ObjectType::Player);
	_playerObject->ScaleObject(scaleVec);

	camera.setPosition(_playerObject->GetPosition());
}

void GameEngine::SetScreenPanelHP(const std::string &filepath, const glm::vec3 &scaleVec = glm::vec3(1.0f))
{
	_screenPanelHP = new GameObject(filepath, ObjectType::OnScreenPanel);
	_screenPanelHP->ScaleObject(scaleVec);
}

void GameEngine::SetScreenPanelScore(const std::string &filepath, const glm::vec3 &scaleVec = glm::vec3(1.0f))
{
	_screenPanelScore = new GameObject(filepath, ObjectType::OnScreenPanel);
	_screenPanelScore->ScaleObject(scaleVec);
}

void GameEngine::SetScreenPanelHunger(const std::string &filepath, const glm::vec3 &scaleVec = glm::vec3(1.0f))
{
	_screenPanelHunger = new GameObject(filepath, ObjectType::OnScreenPanel);
	_screenPanelHunger->ScaleObject(scaleVec);
}

void GameEngine::SetSkybox(const std::vector<std::string> & faces)
{
	float skybox_vertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// skybox VAO
	//unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &_skyboxVAO);
	glGenBuffers(1, &_skyboxVBO);
	glBindVertexArray(_skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glGenTextures(1, &_skyboxTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxTextureID);

	int width, height, number_of_channels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &number_of_channels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void GameEngine::PrintObjects()
{
	std::cout << "\nEnemy Objects\n~~~~~~~~~~~~~~~~~~~~~~\n";
	for (int i = 0; i < _enemyObjects.size(); i++)
	{
		_enemyObjects[i]->PrintObject();
	}

	std::cout << "Coin Objects\n~~~~~~~~~~~~~~~~~~~~~~\n";
	for (int i = 0; i < _coinObjects.size(); i++)
	{
		_coinObjects[i]->PrintObject();
	}
	
	std::cout << "Player Objects\n~~~~~~~~~~~~~~~~~~~~~~\n";
	if (_playerObject != nullptr)
		_playerObject->PrintObject();
}

void GameEngine::_Update()
{
	for (int i = 0; i < _enemyObjects.size(); i++)
	{
		_enemyObjects[i]->Update(_deltaTime);
	}

	_playerObject->Update(_deltaTime);

	for (int i = 0; i < _coinObjects.size(); i++)
	{
		_coinObjects[i]->Update(_deltaTime);
	}
}

bool GameEngine::_IsRenderable(GameObject *object)
{
	if (!object->ShouldRender())
	{
		return false;
	}
	
	auto dist = _playerObject->collider->GetDistance(object->collider);

	if (dist.x > MAX_RENDER_DISTANCE || dist.y > MAX_RENDER_DISTANCE || dist.z > MAX_RENDER_DISTANCE
		|| dist.x < -MAX_RENDER_DISTANCE || dist.y < -MAX_RENDER_DISTANCE || dist.z < - MAX_RENDER_DISTANCE)
	{
		return false;
	}
	return true;
}

void GameEngine::_Render()
{

	for (int i = 0; i < _enemyObjects.size(); i++)
	{
		if (_IsRenderable(_enemyObjects[i]))
		{
			_enemyObjects[i]->Draw(KEY_SHADER_OBJECT);
		}
	}

	_playerObject->Draw(KEY_SHADER_OBJECT);

	for (int i = 0; i < _coinObjects.size(); i++)
	{
		if (_IsRenderable(_coinObjects[i]))
		{
			_coinObjects[i]->Draw(KEY_SHADER_OBJECT);
		}
	}
}

void GameEngine::_UpdateScreenPanel()
{
	if (_frameCounter < 1000 && TOTAL_LIVES != 0)
	{
		// view/projection transformations
		glm::mat4 ortogonal_projection_matrix = glm::orthoLH(-10, 10, -10, 10, -10, 10);

		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("projection", glm::mat4x4(1));
		ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("view", glm::mat4x4(1));

		glDepthFunc(GL_ALWAYS);

		for (float i = 0; i < TOTAL_LIVES; i++) {
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																			 0.0f,1.0f,0.0f,0.0f,//y
																			 0.0f,0.0f,0.0f,0.0f,//z
																			 -7.50f,7.50f - i,0.0f,8.0f });
			_screenPanelHP->Update(_deltaTime);
			_screenPanelHP->Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		}

		int tmp_score = TOTAL_SCORE;
		for (int i = 0; 5 <= tmp_score; i++, tmp_score -= 5)
		{
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																			 0.0f,1.0f,0.0f,0.0f,//y
																			 0.0f,0.0f,0.0f,0.0f,//z
																			 6.0f - i,6.10f ,0.0f,6.5f });
			_screenPanelScore->Update(_deltaTime);
			_screenPanelScore->Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		}
		for (int j = 0; 0 < tmp_score; j++, tmp_score--)
		{
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 1.0f,0.0f,0.0f,0.0f,//x
																			 0.0f,1.0f,0.0f,0.0f,//y
																			 0.0f,0.0f,0.0f,0.0f,//z
																			 7.50f - j,6.40f ,0.0f,8.0f });
			_screenPanelScore->Update(_deltaTime);
			_screenPanelScore->Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		}
		if (VAR_HUNGER < 10) {
			ResourceManager::GetShader(KEY_SHADER_OBJECT).setMat4("model", { 10 - VAR_HUNGER,0.0f,0.0f,0.0f,//x
																			 0.0f,0.5f,0.0f,0.0f,//y
																			 0.0f,0.0f,0.0f,0.0f,//z
																			 0.0f,-7.50f,0.0f,8.0f });
			VAR_HUNGER += (0.000001*SCR_WIDTH);
			_screenPanelHunger->Update(_deltaTime);
			_screenPanelHunger->Draw(ResourceManager::GetShader(KEY_SHADER_OBJECT));
		}
		else if (TOTAL_LIVES > 0)
		{
			VAR_HUNGER = 0;
			TOTAL_LIVES--;
		}
	}
}

void GameEngine::_UpdateSkybox()
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	ResourceManager::GetShader(KEY_SHADER_SKYBOX).use();
	_viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	ResourceManager::GetShader(KEY_SHADER_SKYBOX).setMat4("view", _viewMatrix);
	ResourceManager::GetShader(KEY_SHADER_SKYBOX).setMat4("projection", _projectionMatrix);
	// skybox cube
	glBindVertexArray(_skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}

void GameEngine::_DoBoundryCollusionWith(std::vector<GameObject*> objectList)
{
	for (int i = 0; i < objectList.size(); i++)
	{
		objectList[i]->DoBoundryCollusion();
	}
}

void GameEngine::_DoBoundryCollusion()
{
	_DoBoundryCollusionWith(_enemyObjects);
	_DoBoundryCollusionWith(_coinObjects);

	_playerObject->DoBoundryCollusion();
}

void GameEngine::_DoCollusionEnemy()
{
	for (int i = 0; i < _enemyObjects.size(); i++)
	{
		for (int j = i; j < _enemyObjects.size(); j++)
		{
			_enemyObjects[i]->DoCollusion(_enemyObjects[i]);
		}
		_playerObject->DoCollusion(_enemyObjects[i]);
	}
}

//#pragma optimize("", off)
void GameEngine::_DoCollusionCoin()
{
	for (int i = 0; i < _coinObjects.size(); i++)
	{
		for (int j = i; j < _coinObjects.size(); j++)
		{
			_coinObjects[i]->DoCollusion(_coinObjects[i]);
		}
		
		if (_playerObject->CheckCollusion(_coinObjects[i]))
		{
			if (_coinObjects[i]->ShouldRender())
			{
				_coinObjects[i]->DisableRender();
				std::cout << "yedi" << std::endl;
				_playerObject->PrintObject();
				_coinObjects[i]->PrintObject();
				std::cout << "bitti" << std::endl;

				TOTAL_SCORE += 1;

				VAR_HUNGER -= 0.5f;
			}
		}
	}
}
//#pragma optimize("", on)

void GameEngine::_DoCollusion()
{
	_DoBoundryCollusion();

	_DoCollusionEnemy();
	_DoCollusionCoin();
}

void GameEngine::_ProcessInput()
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(_window, true);
	}
	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		_MovePlayer(Directions::FORWARD);
	}
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		_MovePlayer(Directions::BACKWARD);
	}
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		_MovePlayer(Directions::RIGHT);
	}
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		_MovePlayer(Directions::LEFT);
	}
	if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		_MovePlayer(Directions::UP);
	}
	if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		_MovePlayer(Directions::DOWN);
	}
	if (glfwGetKey(_window, GLFW_KEY_G) == GLFW_PRESS)
	{
		_isDebugMode = true;
	}
	if (glfwGetKey(_window, GLFW_KEY_H) == GLFW_PRESS)
	{
		_isDebugMode = false;
	}
	if (glfwGetKey(_window, GLFW_KEY_I) == GLFW_PRESS)
	{
		_debugPrinter = true;
	}
	if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		_playerObject->AccelerateTowards(Directions::FORWARD);
	}
	if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		_playerObject->AccelerateTowards(Directions::BACKWARD);
	}
	if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		_playerObject->AccelerateTowards(Directions::RIGHT);
	}
	if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		_playerObject->AccelerateTowards(Directions::LEFT);
	}
	if (glfwGetKey(_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		_playerObject->AccelerateTowards(Directions::DOWN);
	}
	if (glfwGetKey(_window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		_playerObject->AccelerateTowards(Directions::UP);
	}

	if (glfwGetKey(_window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (_debugPrinter)
		{
			PrintObjects();
			_debugPrinter = false;
		}
	}
	if (glfwGetKey(_window, GLFW_KEY_0) == GLFW_PRESS)
	{
		// Clear the console.
		std::cout << "\x1B[2J\x1B[H";
	}
}

void GameEngine::_MovePlayer(Directions dir)
{
	camera.ProcessKeyboard(dir, _deltaTime);
	if (!_isDebugMode)
		_playerObject->AccelerateTowards(dir);
}

void GameEngine::_InitGameWindow()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	this->_window = glfwCreateWindow(_windowSize[0], _windowSize[1], WINDOW_TITLE.c_str(), NULL, NULL);
	if (_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(_window);
	glfwSetFramebufferSizeCallback(_window, _FramebufferSizeCallback);
	glfwSetCursorPosCallback(_window, _MouseCallback);
	glfwSetScrollCallback(_window, _ScrollCallback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void GameEngine::_FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void GameEngine::_MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	//auto engine = GameEngine::GetInstance();
	if (isFirstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		isFirstMouse = false;
	}

	float x_offset = xpos - lastX;
	float y_offset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(x_offset, y_offset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void GameEngine::_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

#endif