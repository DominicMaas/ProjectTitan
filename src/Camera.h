#pragma once

#include <pch.h>

class Camera {
private:
    // Used for storing the camera ubo
    vk::Buffer _cameraUboBuffer;
    VmaAllocation _cameraUboAllocation;
    vk::DescriptorSet _cameraDescriptorSet;

    // Vectors
    glm::vec3 _position;

    // Matrices
    glm::mat4 _viewMatrix;
    glm::mat4 _projectionMatrix;

    // Extra
    glm::vec3 _front;
    glm::vec3 _up;
    glm::vec3 _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 _right;

    // Constants
    float _speed = 20.0f;
    float _mouseSensitivity = 0.1f;
    float _yaw = -90;
    float _pitch = 0;

    bool _firstMouse;
    float _lastMouseX;
    float _lastMouseY;

public:
    SceneUBO SceneUBO;

    Camera(glm::vec3 position);
    ~Camera();

    void update();

    void bind(vk::CommandBuffer &commandBuffer);

    glm::mat4 getProjectionMatrix();
    glm::mat4 getViewMatrix();
    glm::vec3 getPosition();

    void setProjectionMatrix(glm::mat4 projMatrix);

    void processKeyboardInput(GLFWwindow *window, float deltaTime);

    void processMouseInput(float xPos, float yPos, bool constrainPitch = true);
};
