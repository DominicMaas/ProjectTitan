// ProjectTitan.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define VMA_IMPLEMENTATION

#include <pch.h>
#include <reactphysics3d/reactphysics3d.h>
#include "Window.h"
#include "core/managers/ResourceManager.h"
#include "core/managers/PipelineManager.h"
#include "core/Scene.h"
#include "World.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

bool mouseCaptured = true;

void setMouseCapture(GLFWwindow *window, bool _mouseCapture) {
    mouseCaptured = _mouseCapture;

    if (mouseCaptured) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

/*

bool renderPhysics = false;
bool renderLines = false;
bool displayShadowMap = false;

bool mouseCaptured = true;
bool guiHasMouse = false;


void processKeyboardInput(GLFWwindow *window, long double delta) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        setMouseCapture(window, false);

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        // Create test entity
        const reactphysics3d::Vector3 halfExtents(1.0, 1.0, 1.0);
        auto* boxShape = currentWorld->getPhysicsCommon()->createBoxShape(halfExtents);
        currentWorld->addEntity(new Entity(currentWorld, ResourceManager::getModel("backpack"), boxShape, camera.getPosition(), glm::vec3()));
    }

    // Process camera inputs
    camera.processKeyboardInput(window, delta);
}*/

int main(void) {
    // Variables that we will need
    Camera* camera = nullptr;
    World* currentWorld = nullptr;

    // Create the window
    Window w("Project Titan [Vulkan]", 800, 600);

    // Initialise window and renderer resources
    if (!w.init()) {
        return -1;
    }

    // Load in shaders
    ResourceManager::loadShader("basic", "shaders/vulkan_test");
    // ResourceManager::loadShader("shadow_depth", "shaders/shadow_depth");
    // ResourceManager::loadShader("debug", "shaders/basic");
    // ResourceManager::loadShader("backpack_shader", "shaders/model");
    // ResourceManager::loadShader("skybox", "shaders/skybox_shader");
    // ResourceManager::loadShader("chunk", "shaders/chunk_shader");
    // ResourceManager::loadShader("debug_depth_quad", "shaders/debug_depth_quad");

    // The main pipeline used throughout the game, warning this is hard coded in some places
    PipelineManager::createPipeline("basic", { .shaderName = "basic" });

    // Textures must be loaded in before the basic pipeline
    ResourceManager::loadTexture("block_map", "textures/block_map.png");
    ResourceManager::loadTexture("square", "textures/square.jpg");
    ResourceManager::loadTexture("test", "textures/test.jpg");

    // Load in models
    ResourceManager::loadModel("backpack", "models/backpack.obj");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    w.createImGuiContext();

    // Start without mouse capture
    setMouseCapture(w.getGLFWWindow(), false);

    // Set the window callbacks
    w.onMouseMove = [&](double xPos, double yPos) {
        if (mouseCaptured) {
            camera->processMouseInput((float)xPos, (float)yPos);
        }
    };

    w.onMouseButton = [&](int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            setMouseCapture(w.getGLFWWindow(), true);
        }
    };

    w.onWindowResize = [&](int width, int height) {
        camera->setProjectionMatrix(glm::perspective(glm::radians(60.0f), (float) width / (float) height, 0.1f, 1000.0f));
    };

    w.onUpdate = [&](float deltaTime) {
        if (glfwGetKey(w.getGLFWWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            setMouseCapture(w.getGLFWWindow(), false);

        // Process camera inputs, this also updates the camera UBO for all objects
        camera->processKeyboardInput(w.getGLFWWindow(), deltaTime);
        camera->update();

        // Update the world
        currentWorld->update(deltaTime, *camera);
    };

    w.onRender = [&](vk::CommandBuffer& commandBuffer) {
        // Bind the descriptor set for the camera (position 0), all objects within
        // the scene will use this, so only bind at the start of the frame
        camera->bind(commandBuffer);

        // Render all chunks and entities within the world
        currentWorld->render(commandBuffer, *camera);

        // Start GUI frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render GUI

        // Debugging window
        {
            ImGui::Begin("Debug");

            ImGui::Text("Position: X: %f Y: %f Z: %f", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
            ImGui::Text("Frame Time: %f ms", w.getFrameTime());
            ImGui::Text("FPS: %i", w.getFPS());
            ImGui::Text("  ");
            ImGui::Text("Rendered Chunks: %i", currentWorld->ChunksRendered);
            ImGui::Text("  ");

            ImGui::Text("G: Set sun look-at to current position");

            ImGui::Text("  ");

            //ImGui::Checkbox("Debug Renderer", &renderLines);

            if (ImGui::Button("Reset World")) {
                currentWorld->reset(true);
            }

            ImGui::SliderFloat3("Light Position", (float*)&currentWorld->SunPosition, -1.0f, 1.0f);

            ImGui::End();
        }

        // Physics
        {
            ImGui::Begin("Physics");

            ImGui::Text("Rigid Bodies: %i", currentWorld->getPhysicsWorld()->getNbRigidBodies());
            ImGui::Text("Collision Bodies: %i", currentWorld->getPhysicsWorld()->getNbCollisionBodies());
            ImGui::Text("World Body Colliders: %i", currentWorld->getWorldBody()->getNbColliders());

            //if (ImGui::Checkbox("Draw Physics Colliders", &renderPhysics)) {
            //    currentWorld->getPhysicsWorld()->setIsDebugRenderingEnabled(true);
            //} else {
            //    currentWorld->getPhysicsWorld()->setIsDebugRenderingEnabled(false);
            //}

            ImGui::End();
        }

        // Finish GUI frame
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    };

    // This cleanup function is called after rendering is complete, but before
    // engine resources are destroyed. (Some cleanup functions need to access renderer
    // resources to correctly cleanup)
    w.onCleanUp = [&]() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        delete currentWorld;
        delete camera;
    };

    // Physics engine for the game
    reactphysics3d::PhysicsCommon physicsCommon;

    // Create the main camera and scene
    camera = new Camera(glm::vec3(8, 40, 8));
    camera->setProjectionMatrix(glm::perspective(glm::radians(60.0f), (float) 800 / (float) 600, 0.1f, 1000.0f));

    // The world
    currentWorld = new World("Test World", &physicsCommon);

    // Run the engine
    w.run();

    return 0;







    /*
    // Physics debugging
    Mesh physicsDebugMesh;

    reactphysics3d::DebugRenderer &debugRenderer = currentWorld->getPhysicsWorld()->getDebugRenderer();
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
    debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_POINT, true);


    //renderEffects.push_back(SSAO());
    ShadowMapping shadowMapping;

    // Create a rigid body in the world
    //reactphysics3d::Vector3 position(0, 100, 0);
    //reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
    //reactphysics3d::Transform transform(position, orientation);

    // Instantiate a sphere collision shape
    //float radius = 3.0f;

    // DEBUGGING!!!!
    //reactphysics3d::SphereShape *sphereShape = physicsCommon.createSphereShape(radius);
    //reactphysics3d::RigidBody *body = currentWorld->getPhysicsWorld()->createRigidBody(transform);
    //body->setType(reactphysics3d::BodyType::DYNAMIC);



    // Add the collider to the rigid body
    //reactphysics3d::Collider *collider = body->addCollider(sphereShape, reactphysics3d::Transform::identity());

    // Setup for per-frame time logic
    long double previousFrameTime = glfwGetTime();
    long double deltaTime = 0;
    long double deltaTimeAccum = 0;

    long double timeStep = 1.0/60.0; // Constant physics time step

    int frames = 0; // The framerate to display
    long double lastFramesTime = glfwGetTime();
    long double frameTime = 0;
    int fps;

    // Get the temp shaders that we need
    Shader* backpackShader = ResourceManager::getShader("backpack_shader");
    Model* backpackModel = ResourceManager::getModel("backpack");
    Shader* physicsShader = ResourceManager::getShader("debug");

    Shader* depthShader = ResourceManager::getShader("shadow_depth");
    Shader* worldShader = ResourceManager::getShader("chunk");


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // ---------- Per-frame time logic ---------- //
        long double currentFrameTime = glfwGetTime(); // Current system time
        deltaTime = currentFrameTime - previousFrameTime; // Time difference between frames
        previousFrameTime = currentFrameTime; // Update the previous time

        // Add the time difference in the accumulator
        deltaTimeAccum += deltaTime;

        // Calculate frames
        frames++;
        if ((currentFrameTime - lastFramesTime ) >= 1.0) {
            frameTime = 1000.0 / double(frames);
            fps = frames;
            frames = 0;
            lastFramesTime += 1.0f; // Reset timer
        }

        // ---------- Run update events ---------- //

        guiHasMouse = io.WantCaptureMouse;

        // Process keyboard inputs for the application
        if (!io.WantCaptureKeyboard) {
            processKeyboardInput(window, deltaTime);
        }

        currentWorld->update(camera, deltaTime);

        // ---------- Process Physics ---------- //

        // While there is enough accumulated time to take
        // one or several physics steps
        while (deltaTimeAccum >= timeStep) {
            // Update the physics world with a constant time step
            currentWorld->getPhysicsWorld()->update(timeStep);

            // Decrease the accumulated time
            deltaTimeAccum -= timeStep;
        }

        // Update all objects within the world
        currentWorld->updatePhysics(timeStep, deltaTimeAccum);

        // Clear for new frame
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // Render depth of scene to a texture
        depthShader->use();
        shadowMapping.renderToDepthMap();
        currentWorld->render(camera, *depthShader);

        // render scene as normal using the generated depth/shadow map
        worldShader->use();
        shadowMapping.finishRenderingToDepthMap(WIDTH, HEIGHT);
        currentWorld->render(camera, *worldShader);

        // Set the render mode based on the render lines boolean
        GLCall(glPolygonMode(GL_FRONT_AND_BACK, renderLines ? GL_LINE : GL_FILL));

        backpackShader->use();
        backpackShader->setMat4("view", camera.getViewMatrix());
        backpackShader->setMat4("projection", camera.getProjectionMatrix());

        // Render this model at the location of the sun for debugging purposes
        glm::mat4 pos(1.0f);
        pos = glm::translate(pos, currentWorld->SunPosition);
        backpackShader->setMat4("model", pos);
        backpackModel->render(*backpackShader);

        // Render depth map to a quad for visual debugging
        if (displayShadowMap) {
            shadowMapping.renderDebugQuad();
        }

        worldShader->use();
        currentWorld->postRender(camera, *worldShader);



        // ---------- Render ---------- //

        // Render The current world
        //if (!renderPhysics) {
            //currentWorld->render(camera);

            // Render the world effects
            //for (RenderEffect r : renderEffects) {
            //    r.render(&camera);
            //}

            //backpackShader->use();
            //backpackShader->setMat4("view", camera.getViewMatrix());
            //backpackShader->setMat4("projection", camera.getProjectionMatrix());

            //glm::mat4 pos(1.0f);
            //pos = glm::translate(pos, glm::vec3(0.0f, 40.0f, 0.0f));
            //backpackShader->setMat4("model", pos);

            //backpackModel->render(*backpackShader);
        //}

        // Physics debug rendering
        if (renderPhysics) {
            std::vector<Vertex> debugVertices;

            auto triangles = debugRenderer.getTriangles();
            for (auto const &i : triangles) {
                debugVertices.push_back(Vertex{glm::vec3(i.point1.x, i.point1.y, i.point1.z)});
                debugVertices.push_back(Vertex{glm::vec3(i.point2.x, i.point2.y, i.point2.z)});
                debugVertices.push_back(Vertex{glm::vec3(i.point3.x, i.point3.y, i.point3.z)});
            }

            physicsDebugMesh.rebuild(debugVertices, std::vector<unsigned int>(), std::vector<Texture>());

            physicsShader->use();
            physicsShader->setMat4("view", camera.getViewMatrix());
            physicsShader->setVec3("viewPos", camera.getPosition());
            physicsShader->setMat4("projection", camera.getProjectionMatrix());
            physicsShader->setMat4("model", glm::mat4(1));

            GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
            physicsDebugMesh.render(*physicsShader);
            GLCall(glPolygonMode(GL_FRONT_AND_BACK, renderLines ? GL_LINE : GL_FILL));
        }*/
}