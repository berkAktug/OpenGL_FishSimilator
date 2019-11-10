#include "Game.h"

// FALLOWING CODE ASSUMES TEXTURE IS SET BEFORE CALLING SHADER.

int main() 
{
	Game::getInstance().init();

	Game::getInstance().setTexture_DDS("uvmap.dds");
	Game::getInstance().setShader("StandardShading.vertexshader", "StandardShading.fragmentshader");

	Game::getInstance().addObject("suzanne.obj");
	Game::getInstance().addObject("cube.obj");
	Game::getInstance().addObject("cube.obj");

	Game::getInstance().run();

	Game::getInstance().cleanUp();

	return 0;
}