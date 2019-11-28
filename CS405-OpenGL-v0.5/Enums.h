#ifndef ENUMS_H
#define ENUMS_H

enum Directions {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD
	//UP_LEFT,
	//UP_RIGHT,
	//UP_FORWARD,
	//UP_BACKWARD,
	//DOWN_LEFT,
	//DOWN_RIGHT,
	//DOWN_FORWARD,
	//DOWN_BACKWARD,
	//LEFT_FORWARD,
	//LEFT_BACKWARD,
	//RIGHT_FORWARD,
	//RIGHT_BACKWARD
};

enum ObjectType {
	Player,
	Enemy,
	Coin,
	Envoirmental
};

enum CageType {
	CageBox,
	CageSphere
};

enum MovementType {
	Random,
	Normal,
	BackWard,
	Funky,
	Possessed
};

enum CollusionDetectionType {
	CollusionSimple,
	CollusionAABB,
	CollusionAABBCoin
};


#endif // !ENUMS_H
