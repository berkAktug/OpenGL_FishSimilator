#include <iostream>
#include "Game.h"

int main() {

	Game * game = Game::getGameInstance();

	game->setShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	game->setTexture_DDS("uvmap.dds");

	game->run();

	//game->cleanUp();

	return 0;
}