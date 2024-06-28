#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <windows.h>

#define SDL_MAIN_HANDLED

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

//#include "imgui_impl_sdl2.h"
//#include "imgui_impl_sdlrenderer2.h"

#include "common.h"
#include "maths.cpp"

#include "GameplayExtern.cpp"
#include "Input.cpp"
#include "Renderer.cpp"
