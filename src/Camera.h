#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
private:
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
    float _speed = 30.0f;
    float _mouseSensitivity = 0.1f;
    float _yaw = -90;
    float _pitch = 0;

    bool _firstMouse;
    float _lastMouseX;
    float _lastMouseY;

    void processUpdates();

public:
    Camera(glm::vec3 position);

    glm::mat4 getProjectionMatrix();

    glm::mat4 getViewMatrix();

    glm::vec3 getPosition();

    void processKeyboardInput(GLFWwindow *window, float deltaTime);

    void processMouseInput(float xPos, float yPos, GLboolean constrainPitch = true);
};
