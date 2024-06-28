#pragma once
#include <string>

struct Scene;

void LoadRoomFromFile(Scene *scene, std::string room);
void SaveRoomToFile(Scene *scene, std::string room);