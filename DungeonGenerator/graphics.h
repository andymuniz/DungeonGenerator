//Include GLEW
#include <GL/glew.h>
//Include GLFW
#include <GLFW/glfw3.h>
//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

#include "Dungeon.h"

extern int screenWidth, screenHeight;

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static int draw(Dungeon* MyDungeon){
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	glEnable(GL_DEPTH_TEST);

	//Declare a window object
	GLFWwindow* window;

	//Create a window and create its OpenGL context
	window = glfwCreateWindow(screenWidth, screenHeight, "Dungeon Generator", NULL, NULL);

	//If the window couldn't be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);
	//Sets the key callback
	glfwSetKeyCallback(window, key_callback);
	//Initialize GLEW
	GLenum err = glewInit();
	//If GLEW hasn't initialized
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
								 //Set the camera perspective
	glLoadIdentity(); //reset the camera
	gluPerspective(45.0f,                      //camera angle
		(GLfloat)screenWidth / (GLfloat)screenHeight, //The width to height ratio
		1.0f,                          //The near z clipping coordinate
		200.0f);                       //The far z clipping coordinate

									   //Set a background color
	glClearColor(0.0f, 0.0f, 0.15f, 0.0f);
	//Main OpenGL Loop
	do
	{
		//Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
		glLoadIdentity(); //Reset the drawing perspective

						  //Draw every room
		for (auto& it : MyDungeon->getRooms()) {
			glPushMatrix();
			glTranslatef((float)it->getPosition()[0] - 50.f, (float)it->getPosition()[1] - 50.f, 0.f);
			glBegin(GL_QUADS); //Begin triangle coordinates

			AABB box = it->getAABB();
			float z = -150.0f;

			////Room Rectangle - draw counter-clockwise starting from LL corner.
			//glColor3f(1, 0, 0);
			glVertex3d(box.LL[0], box.LL[1], z);
			glVertex3d(box.LR[0], box.LR[1], z);
			glVertex3d(box.UR[0], box.UR[1], z);
			glVertex3d(box.UL[0], box.UL[1], z);

			glEnd(); //End triangle coordinates
			glPopMatrix();
		}

		//Swap buffers
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
	} //Check if the ESC key had been pressed or if the window had been closed
	while (!glfwWindowShouldClose(window));

	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(window);
	//Finalize and clean up GLFW
	glfwTerminate();

	exit(EXIT_SUCCESS);
}