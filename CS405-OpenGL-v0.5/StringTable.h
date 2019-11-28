#ifndef STRINGTABLE_H
#define STRINGTABLE_H
#include <string>

// STRING TABLE
// -----------------------------
std::string WINDOW_TITLE = "Fish In the sea";

std::string KEY_SHADER_SKYBOX = "SKYBOX_SHADER";
std::string KEY_SHADER_OBJECT = "OBJECT_SHADER";
std::string KEY_TEXTURE_MARBLE = "TEXTURE_MARBLE";


std::string FILE_SHADER_FRAGMENT_SKYBOX = "./Resource/shaders/skybox.fs";
std::string FILE_SHADER_VERTEX_SKYBOX = "./Resource/shaders/skybox.vs";

std::string FILE_SHADER_FRAGMENT_STANDART_OBJECT = "./Resource/shaders/model_loading.fs";
std::string FILE_SHADER_VERTEX_STANDARD_OBJECT = "./Resource/shaders/model_loading.vs";


std::string FILE_OBJECT_HP = "./Resource/objects/Galp/Galp.obj";
std::string FILE_OBJECT_SCORE = "./Resource/objects/Score/Score.obj";
std::string FILE_OBJECT_HUNGER = "./Resource/objects/Hunger/Hunger.obj";

std::string FILE_OBJECT_CYBORG = "./Resource/objects/cyborg/cyborg.obj";
std::string FILE_OBJECT_NANOSUIT = "./Resource/objects/nanosuit/nanosuit.obj";
std::string FILE_OBJECT_FLATPLANE = "./Resource/objects/flat-plane/flat-plane.obj";
std::string FILE_OBJECT_COIN = "./Resource/objects/coin/coin.fbx";


std::string FILE_TEXTURE_MARBLE = "./Resource/textures/marble.jpg";

std::string FILE_TEXTURE_SKYBOX_RIGHT = "./Resource/textures/skybox/right.jpg";
std::string FILE_TEXTURE_SKYBOX_LEFT = "./Resource/textures/skybox/left.jpg";
std::string FILE_TEXTURE_SKYBOX_FRONT = "./Resource/textures/skybox/front.jpg";
std::string FILE_TEXTURE_SKYBOX_BACK = "./Resource/textures/skybox/back.jpg";
std::string FILE_TEXTURE_SKYBOX_TOP = "./Resource/textures/skybox/top.jpg";
std::string FILE_TEXTURE_SKYBOX_BOTTOM = "./Resource/textures/skybox/bottom.jpg";
// ------------------------------
#endif
