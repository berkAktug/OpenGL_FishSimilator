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

	void addEnemy();

	void addPlayer();

	void addCoin();
	
private:
	GameEngine() {}


	std::map<Object*, std::string> cages;
	std::map<Model*, std::string> models;
	//std::vector<Object*> cages;
	//std::vector<Model*> model;
	//std::vector<PhysicEngine*> physic;
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

void GameEngine::addEnemy()
{

}

void GameEngine::addPlayer()
{

}

void GameEngine::addCoin()
{

}

#endif