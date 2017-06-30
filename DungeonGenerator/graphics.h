#pragma once

//Include GLEW
#include <GL/glew.h>
//Include GLFW
#include <GLFW/glfw3.h>

#include "Dungeon.h"

namespace glENGINE {
	int draw(Dungeon* MyDungeon);

	//Define an error callback
	void error_callback(int error, const char* description);

	//Define the key input callback
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};