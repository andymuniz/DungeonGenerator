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

int main(void)
{
	////*Dungeon Generator Logic*////

	Dungeon *MyDungeon = new Dungeon();
	MyDungeon->GenerateDungeon();

	////*End of Generator Logic*////
	
	/*OpenGL Rendering*/
	return draw(MyDungeon);

}