#ifndef ENUMS_H
#define ENUMS_H

enum Directions {
	UP,
	DOWN,
	RIGHT,
	LEFT,
	FORWARD,
	BACKWARD
};

enum ObjectType {
	Player,
	Enemy,
	Coin,
	Envoirmental,
	OnScreenPanel
};

enum MovementType {
	Random,
	Normal,
	UpDown,
	NONE
};

#endif // !ENUMS_H
