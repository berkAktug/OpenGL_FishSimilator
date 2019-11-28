#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "model.h"
#include "Object.h"
#include "PhysicsEngine.h"


class GameEngine {
public:

private:
	GameEngine() : model("") {}

	Object cage;
	Model model;
	PhysicEngine physic;
};


#endif