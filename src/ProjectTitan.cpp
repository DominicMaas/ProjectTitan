// ProjectTitan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "effects/RenderEffect.h"
#include "effects/SSAO.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Chunk.h"
#include "World.h"
#include "TextRenderer.h"
#include "Mesh.h"
#include <reactphysics3d/reactphysics3d.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "core/ResourceManager.h"

Camera camera(glm::vec3(8, 40, 8));
World *currentWorld;

bool renderPhysics = false;
bool renderLines = false;

bool mouseCaptured = true;
bool guiHasMouse = false;

std::vector<RenderEffect> renderEffects;

glm::mat4 projectionMatrix;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

void onFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    projectionMatrix = glm::perspective(glm::radians(60.0f), (float) width / (float) height, 0.1f, 1000.0f);
}

void processMouseInput(GLFWwindow *window, double xPos, double yPos) {
    // Process camera inputs
    if (mouseCaptured) {
        camera.processMouseInput(xPos, yPos);
    }
}


void setMouseCapture(GLFWwindow *window, bool _mouseCapture) {
    if (guiHasMouse) {
        return; // Don't run this if the GUI has the mouse
    }

    mouseCaptured = _mouseCapture;

    if (mouseCaptured) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        setMouseCapture(window, true);
}

void processKeyboardInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        setMouseCapture(window, false);

    // Process camera inputs
    camera.processKeyboardInput(window, deltaTime);
}

int main(void) {
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
    GLFWwindow *window = glfwCreateWindow(width, height, "Project Titan", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Make the window's context current
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Set the view port
    glViewport(0, 0, width, height);

    // 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);

    // Other effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load in resources
    ResourceManager::loadTexture("block_map", "textures/block_map.png");
    ResourceManager::loadTexture("square", "textures/square.jpg");
    ResourceManager::loadTexture("test", "textures/test.png");

    // Capture the mouse input
    setMouseCapture(window, false);

    // Set the GLFW callbacks
    glfwSetFramebufferSizeCallback(window, onFramebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, processMouseInput);

    // Projection Matrix
    projectionMatrix = glm::perspective(glm::radians(60.0f), (float) width / (float) height, 0.1f, 1000.0f);

    // Physics engine for the game
    reactphysics3d::PhysicsCommon physicsCommon;



    // The world
    currentWorld = new World("Test World", &physicsCommon);
    currentWorld->getPhysicsWorld()->setIsDebugRenderingEnabled(renderPhysics);

    // Physics debugging
    reactphysics3d::DebugRenderer &debugRenderer = currentWorld->getPhysicsWorld()->getDebugRenderer();
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
    reactphysics3d::SphereShape *sphereShape = physicsCommon.createSphereShape(radius);
    reactphysics3d::RigidBody *body = currentWorld->getPhysicsWorld()->createRigidBody(transform);

    // Add the collider to the rigid body
    reactphysics3d::Collider *collider = body->addCollider(sphereShape, reactphysics3d::Transform::identity());
    body->setType(reactphysics3d::BodyType::STATIC);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
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

        // Update the mouse bool
        guiHasMouse = io.WantCaptureMouse;

        // Process keyboard inputs for the application
        if (!io.WantCaptureKeyboard) {
            processKeyboardInput(window);
        }

        // Clear screen for a new frame
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set the render mode based on the render lines boolean
        glPolygonMode(GL_FRONT_AND_BACK, renderLines ? GL_LINE : GL_FILL);

        // Start a new GUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        currentWorld->update(camera, projectionMatrix, deltaTime);

        // Render The current world
        if (!renderPhysics) {
            currentWorld->render(camera, projectionMatrix, deltaTime);

            // Render the world effects
            for (RenderEffect r : renderEffects) {
                r.render(&camera);
            }
        }

        // Physics debug rendering
        if (renderPhysics) {
            std::vector<glm::vec3> debugVertices;

            auto triangles = debugRenderer.getTriangles();
            for (auto const &i : triangles) {
                debugVertices.push_back(glm::vec3(i.point1.x, i.point1.y, i.point1.z));
                debugVertices.push_back(glm::vec3(i.point2.x, i.point2.y, i.point2.z));
                debugVertices.push_back(glm::vec3(i.point3.x, i.point3.y, i.point3.z));
            }

            Mesh m(debugVertices);

            debugShader.use();

            debugShader.setMat4("view", camera.getViewMatrix());
            debugShader.setVec3("viewPos", camera.getPosition());
            debugShader.setMat4("projection", projectionMatrix);
            debugShader.setMat4("model", glm::mat4(1));

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            m.render();
            glPolygonMode(GL_FRONT_AND_BACK, renderLines ? GL_LINE : GL_FILL);
        }

        // Debugging window
        {
            ImGui::Begin("Debug");

            ImGui::Text("Position: X: %f Y: %f Z: %f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
            ImGui::Text("Frame Time: %f ms", frameTime);
            ImGui::Text("FPS: %i", (int) fps);

            ImGui::Checkbox("Draw Physics Colliders", &renderPhysics);
            ImGui::Checkbox("Debug Renderer", &renderLines);

            if (ImGui::Button("Reset World")) {
                currentWorld->reset(true);
            }

            ImGui::End();
        }

        // Render the debug GUI
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete currentWorld;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Exit the program
    glfwTerminate();
    return 0;
}