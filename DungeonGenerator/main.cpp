//Include GLEW
#include <GL/glew.h>
//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

//Include my headers
#include "Dungeon.h"
#include "graphics.h"

//Globals
bool debug_flag = false;
int screenWidth = 640;
int screenHeight = 468;

////Define an error callback
//static void error_callback(int error, const char* description)
//{
//	fputs(description, stderr);
//	_fgetchar();
//}
//
////Define the key input callback
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//}

int main(void)
{
	////*Dungeon Generator Logic*////

	Dungeon *MyDungeon = new Dungeon();
	MyDungeon->GenerateDungeon();

	////*End of Generator Logic*////
	
	/*OpenGL Rendering*/
	return draw(MyDungeon);

}