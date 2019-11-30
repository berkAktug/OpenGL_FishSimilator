#ifndef ENUMS_H
#define ENUMS_H

enum Directions {
	UP,
	DOWN,
	LEFT,
	RIGHT,
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

//enum CageType {
//	CageBox,
//	CageSphere
//};

enum MovementType {
	Random,
	Normal,
	UpDown,
	NONE
};

//enum CollusionDetectionType {
//	CollusionSimple,
//	CollusionAABB,
//	CollusionAABBCoin
//};


#endif // !ENUMS_H
