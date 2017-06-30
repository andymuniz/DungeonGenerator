#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") //this prevents the console window from opening

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

//Include my headers
#include "Graphics.h"

//Globals
bool debug_flag = false;

int main(void)
{
	////*Dungeon Generator Logic*////

	Dungeon *MyDungeon = new Dungeon();
	MyDungeon->GenerateDungeon();

	////*End of Generator Logic*////
	
	/*OpenGL Rendering*/
	glENGINE::draw(MyDungeon);

	return 0;
}