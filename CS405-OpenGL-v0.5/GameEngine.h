#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "model.h"
#include "Object.h"
#include "PhysicsEngine.h"


class GameEngine {
public:
	static GameEngine getInstance();
	
	void start();

	void update();

	void addEnemy(const std::string &filepath);

	void addPlayer(const std::string &filepath);

	void addCoin(const std::string &filepath);
	
private:
	GameEngine() {}


	//std::vector<Object*> cages;
	std::vector<Model*> enemyModels;
	Model *playerModel;
};

GameEngine GameEngine::getInstance()
{
	static GameEngine instance;
	return instance;
}

void GameEngine::start()
{

}

void GameEngine::update()
{

}

void GameEngine::addEnemy(const std::string &filepath)
{
	enemyModels.push_back(new Model(filepath));
}

void GameEngine::addPlayer(const std::string &filepath)
{
	playerModel = new Model(filepath);
}

void GameEngine::addCoin(const std::string &filepath)
{
	
}

#endif