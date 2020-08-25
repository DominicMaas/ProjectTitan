// ProjectTitan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "effects/RenderEffect.h"
#include "effects/SSAO.h"
#include "effects/ShadowMapping.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Chunk.h"
#include "World.h"
#include "TextRenderer.h"
#include "Mesh.h"
#include <reactphysics3d/reactphysics3d.h>

Camera camera(glm::vec3(8, 40, 8));
World* currentWorld;
TextRenderer* textRenderer;
bool debugRender = true;

std::vector<RenderEffect> renderEffects;

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

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        debugRender = !debugRender;
        currentWorld->getPhysicsWorld()->setIsDebugRenderingEnabled(debugRender);
    }

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		currentWorld->reset(true);
	}
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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
	glEnable(GL_CULL_FACE);

	// Other effects
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Capture the mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Change view port on window resize
	glfwSetFramebufferSizeCallback(window, onFramebufferSizeCallback);
	glfwSetCursorPosCallback(window, processMouseInput);

	// Projection Matrix
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 1000.0f);
	textRenderer = new TextRenderer(glm::ortho(0.0f, (float)width, 0.0f, (float)height));

	glfwSwapInterval(1);

    // Physics engine for the game
    reactphysics3d::PhysicsCommon physicsCommon;

    // The world
    currentWorld = new World("Test World", &physicsCommon);

    // Physics debugging
    reactphysics3d::DebugRenderer& debugRenderer = currentWorld->getPhysicsWorld()->getDebugRenderer();
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_POINT, true);

    currentWorld->getPhysicsWorld()->setIsDebugRenderingEnabled(true);

    renderEffects.push_back(SSAO());
	//renderEffects.push_back(ShadowMapping(width, height));

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	float fps = 0;
	float frameTime = 0;

	Shader debugShader("shaders/basic.vert", "shaders/basic.frag");

    // Create a rigid body in the world
    reactphysics3d::Vector3 position(0, 20, 0);
    reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
    reactphysics3d::Transform transform(position, orientation);

    // Instantiate a sphere collision shape
    float radius = 3.0f;

    // DEBUGGING!!!!
    reactphysics3d::SphereShape* sphereShape = physicsCommon.createSphereShape(radius);
    reactphysics3d::RigidBody* body = currentWorld->getPhysicsWorld()->createRigidBody(transform);

    // Add the collider to the rigid body
    reactphysics3d::Collider* collider = body->addCollider(sphereShape, reactphysics3d::Transform::identity());
    body->setType(reactphysics3d::BodyType::STATIC);

    // Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
			frameTime = 1000.0 / double(nbFrames);
			fps = nbFrames;
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Input
		processKeyboardInput(window);

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render
		currentWorld->update(camera, proj, deltaTime);

		// Render world effects
		for (RenderEffect r : renderEffects)
		{
			r.render(&camera);
		}

		// Debug
		if (debugRender) {
		    std::vector<glm::vec3> debugVertices;

		    auto tC = debugRenderer.getNbTriangles();
            auto triangles = debugRenderer.getTriangles();
            for (auto const& i : triangles) {
                debugVertices.push_back(glm::vec3(i.point1.x, i.point1.y, i.point1.z));
                debugVertices.push_back(glm::vec3(i.point2.x, i.point2.y, i.point2.z));
                debugVertices.push_back(glm::vec3(i.point3.x, i.point3.y, i.point3.z));
            }

            Mesh m(debugVertices);

            debugShader.use();

            debugShader.setMat4("view", camera.getViewMatrix());
            debugShader.setVec3("viewPos", camera.getPosition());
            debugShader.setMat4("projection", proj);
            debugShader.setMat4("model", glm::mat4(1));

            m.render();
		}

		textRenderer->renderText("Position: " + std::to_string(camera.getPosition().x) + " / " + std::to_string(camera.getPosition().y) + " / " + std::to_string(camera.getPosition().z), glm::vec2(25.0f, height - 50.0f), 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
		textRenderer->renderText("Frame Time: " + std::to_string((int)frameTime) + "ms", glm::vec2(25.0f, height - 80.0f), 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
		textRenderer->renderText("FPS: " + std::to_string((int)fps), glm::vec2(25.0f, height - 110.0f), 0.5f, glm::vec3(0.5, 0.8f, 0.2f));

        // Get the updated position of the body
        const reactphysics3d::Transform& transform = body->getTransform();
        const reactphysics3d::Vector3& position = transform.getPosition();

        // Display the position of the body
        std::cout << "Body Position: (" << position.x << ", " <<
                  position.y << ", " << position.z << ")" << std::endl;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete textRenderer;
	delete currentWorld;

	// Exit the program
	glfwTerminate();
	return 0;
}