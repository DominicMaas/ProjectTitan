// ProjectTitan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Chunk.h"
#include "World.h"

Camera camera(glm::vec3(8, 40, 8));

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void onFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processMouseInput(GLFWwindow* window, double xPos, double yPos)
{
	// Process camera inputs
	camera.processMouseInput(xPos, yPos);
}

void processKeyboardInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Process camera inputs
	camera.processKeyboardInput(window, deltaTime);

	// Debugging
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
}

int main(void)
{
	int width = 1920;
	int height = 1080;

	// Initialize the library
	if (!glfwInit())
		return -1;

	// Setup OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, "Project Titan", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Set the view port
	glViewport(0, 0, width, height);

	// 3D
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// Capture the mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Change view port on window resize
	glfwSetFramebufferSizeCallback(window, onFramebufferSizeCallback);
	glfwSetCursorPosCallback(window, processMouseInput);

	// Projection Matrix
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);

	glfwSwapInterval(1);

	World* w = new World(4567573453, "Test World");

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		processKeyboardInput(window);

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render
		w->update(camera, proj, deltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Exit the program
	glfwTerminate();
	return 0;
}