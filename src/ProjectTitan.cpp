// ProjectTitan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include <iostream>
//#include "Shader.h"
//#include "effects/RenderEffect.h"
//#include "effects/SSAO.h"
//#include "Camera.h"
//#include "Chunk.h"
//#include "World.h"
//#include "TextRenderer.h"
//#include "Mesh.h"
//#include <reactphysics3d/reactphysics3d.h>
//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"
//#include "core/ResourceManager.h"
//#include "core/Model.h"
//#include "effects/ShadowMapping.h"
#define VMA_IMPLEMENTATION

#include <pch.h>
#include "Window.h"
#include "core/managers/ResourceManager.h"
#include "core/managers/BlockManager.h"
#include "core/managers/PipelineManager.h"
#include "core/Scene.h"

/*int WIDTH = 1920;
int HEIGHT = 1080;

Camera camera(glm::vec3(8, 40, 8));
World *currentWorld;

bool renderPhysics = false;
bool renderLines = false;
bool displayShadowMap = false;

bool mouseCaptured = true;
bool guiHasMouse = false;

std::vector<RenderEffect> renderEffects;

void setWindowSize(GLFWwindow *window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;

    GLCall(glViewport(0, 0, width, height));
    camera.setProjectionMatrix(glm::perspective(glm::radians(60.0f), (float) width / (float) height, 0.1f, 1000.0f));
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        setMouseCapture(window, true);
}

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


    Window w("Test Window", 800, 600);
    if (!w.init()) {
        return -1;
    }

    // Load in resources
    ResourceManager::loadShader("basic", "shaders/vulkan_test");
    PipelineManager::createPipeline("basic", { .shaderName = "basic" });

    ResourceManager::loadTexture("block_map", "textures/block_map.png");
    ResourceManager::loadTexture("square", "models/diffuse.jpg");
    ResourceManager::loadTexture("test", "textures/test.jpg");





    const std::vector<Vertex> vertices = {
            {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    const std::vector<unsigned short> indices = {
            0, 1, 3, 1, 2, 3
    };

    //ResourceManager::loadModel("backpack", "models/backpack.obj", w.getRenderableData());

    Mesh* mesh = new Mesh("basic", vertices, indices, std::vector<Texture>());

    auto* camera = new Camera(glm::vec3(0,0,0));
    auto* scene = new Scene(camera);
    scene->addRenderable("TestMesh", mesh, w.getRenderableData());
    //scene->addRenderable("TestModel", ResourceManager::getModel("backpack"), w.getRenderableData());

    w.setCurrentScene(scene);

    w.run();

    delete camera;

    return 0;

    /*// Initialize the library
    if (!glfwInit()) {
        spdlog::error("[Main] Failed to init GLFW");
        return -1;
    }

    // Setup OpenGL Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Project Titan", NULL, NULL);
    if (!window) {
        spdlog::error("[Main] Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Make the window's context current
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        spdlog::error("[Main] Failed to initialize GLAD");
        glfwTerminate();
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

    // 3D
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_MULTISAMPLE));
    GLCall(glEnable(GL_CULL_FACE));

    // Other effects
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // Load in resources
    ResourceManager::loadTexture("block_map", "textures/block_map.png", GL_CLAMP_TO_EDGE, GL_NEAREST);
    ResourceManager::loadTexture("square", "textures/square.jpg");
    ResourceManager::loadTexture("test", "textures/test.jpg");

    ResourceManager::loadShader("shadow_depth", "shaders/shadow_depth");
    ResourceManager::loadShader("debug", "shaders/basic");
    ResourceManager::loadShader("backpack_shader", "shaders/model");
    ResourceManager::loadShader("skybox", "shaders/skybox_shader");
    ResourceManager::loadShader("chunk", "shaders/chunk_shader");
    ResourceManager::loadShader("debug_depth_quad", "shaders/debug_depth_quad");

    ResourceManager::loadModel("backpack", "models/backpack.obj");

    // Capture the mouse input
    setMouseCapture(window, false);

    // Set the GLFW callbacks
    glfwSetFramebufferSizeCallback(window, setWindowSize);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, processMouseInput);

    // Set the initial window size
    setWindowSize(window, WIDTH, HEIGHT);

    // Physics engine for the game
    reactphysics3d::PhysicsCommon physicsCommon;

    // The world
    currentWorld = new World("Test World", &physicsCommon);

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

        // ---------- Prepare GUI for new frame ---------- //

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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
        }

        // Debugging window
        {
            ImGui::Begin("Debug");

            ImGui::Text("Position: X: %f Y: %f Z: %f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
            ImGui::Text("Frame Time: %Lf ms", frameTime);
            ImGui::Text("FPS: %i", fps);
            ImGui::Text("  ");
            ImGui::Text("Rendered Chunks: %i", currentWorld->ChunksRendered);
            ImGui::Text("  ");

            ImGui::Text("G: Set sun look-at to current position");

            ImGui::Text("  ");

            ImGui::Checkbox("Debug Renderer", &renderLines);

            ImGui::Checkbox("Show Shadow Depth Map", &displayShadowMap);

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

            if (ImGui::Checkbox("Draw Physics Colliders", &renderPhysics)) {
                currentWorld->getPhysicsWorld()->setIsDebugRenderingEnabled(true);
            } else {
                currentWorld->getPhysicsWorld()->setIsDebugRenderingEnabled(false);
            }

            ImGui::End();
        }

        // Render the debug GUI
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // ---------- End Frame ---------- //

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete currentWorld;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Exit the program
    glfwTerminate();
    return 0;*/
}