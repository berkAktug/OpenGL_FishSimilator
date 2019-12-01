#include "GameEngine.h"

int main()
{
	GameEngine engine = GameEngine::GetInstance();

	engine.Init();

	std::vector<std::string> faces
	{
		FILE_TEXTURE_SKYBOX_RIGHT,
		FILE_TEXTURE_SKYBOX_LEFT,
		FILE_TEXTURE_SKYBOX_TOP,
		FILE_TEXTURE_SKYBOX_BOTTOM,
		FILE_TEXTURE_SKYBOX_FRONT,
		FILE_TEXTURE_SKYBOX_BACK
	};

	// Initialize Skybox
	// --------------------------------------
	engine.SetSkybox(faces);

	// build and compile shaders
	// -------------------------
	ResourceManager::LoadShader(FILE_SHADER_VERTEX_SKYBOX.c_str(),
		FILE_SHADER_FRAGMENT_SKYBOX.c_str(), nullptr, KEY_SHADER_SKYBOX);

	ResourceManager::LoadShader(FILE_SHADER_VERTEX_STANDARD_OBJECT.c_str(),
		FILE_SHADER_FRAGMENT_STANDART_OBJECT.c_str(), nullptr, KEY_SHADER_OBJECT);


	// shader configuration
	// --------------------
	ResourceManager::GetShader(KEY_SHADER_OBJECT).use();

	ResourceManager::GetShader(KEY_SHADER_SKYBOX).use().setInt("skybox", 0);

	// Scale models 
	// -------------------
	auto scaleCyborg = glm::vec3(0.5f, 0.5f, 0.5f);
	auto scaleSoldier = glm::vec3(0.2f, 0.2f, 0.2f);
	auto scaleCoin = glm::vec3(0.01f, 0.01f, 0.01f);
	
	auto scaleHpPanel = glm::vec3(0.1f, 0.9f, 0.2f);
	auto scaleScore = glm::vec3(0.1f, 0.9f, 0.2f);
	auto scaleHunger = glm::vec3(0.1f, 0.9f, 0.2f);
		
	// Create && and scale Game Objects.
	// ---------------------------------------
	engine.SetPlayer(FILE_OBJECT_CYBORG, scaleCyborg);

	engine.AddEnemy(FILE_OBJECT_NANOSUIT, scaleSoldier);
	engine.AddEnemy(FILE_OBJECT_NANOSUIT, scaleSoldier);
		  
	engine.AddCoin(FILE_OBJECT_COIN, scaleCoin);
	engine.AddCoin(FILE_OBJECT_COIN, scaleCoin);
	engine.AddCoin(FILE_OBJECT_COIN, scaleCoin);
	engine.AddCoin(FILE_OBJECT_COIN, scaleCoin);
	engine.AddCoin(FILE_OBJECT_COIN, scaleCoin);
	engine.AddCoin(FILE_OBJECT_COIN, scaleCoin);
	engine.AddCoin(FILE_OBJECT_COIN, scaleCoin);
	engine.AddCoin(FILE_OBJECT_COIN, scaleCoin);
		  
	engine.SetScreenPanelHP(FILE_OBJECT_HP, scaleHpPanel);
	engine.SetScreenPanelScore(FILE_OBJECT_SCORE, scaleScore);
	engine.SetScreenPanelHunger(FILE_OBJECT_HUNGER, scaleHunger);


	//engine.NotifyObjectChanges();

	engine.PrintObjects();

	// Start the game
	// ----------------
	engine.StartGame();


	// Cleanup the workSpace
	// ---------------------
	engine.FinishGame();

	return 0;
}
