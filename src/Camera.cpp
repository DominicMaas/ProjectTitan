#include "Camera.h"
#include "core/Renderer.h"
#include "core/managers/PipelineManager.h"

Camera::Camera(glm::vec3 position) {
    // Initial position of the camera
    _position = position;

    // Create a uniform buffer for the view projection matrix for the camera
    VmaAllocationInfo uniformBufferAllocInfo = {};
    Renderer::Instance->createBuffer(_cameraUboBuffer, _cameraUboAllocation, uniformBufferAllocInfo,
                                     sizeof(SceneUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Allocate the descriptor set
    auto* pipeline = PipelineManager::getPipeline("basic");
    _cameraDescriptorSet = pipeline->createUBODescriptorSet();

    // Bind the uniform buffer
    vk::DescriptorBufferInfo bufferInfo = {
            .buffer = _cameraUboBuffer,
            .offset = 0,
            .range = sizeof(ModelUBO)
    };

    vk::WriteDescriptorSet descriptorWrite = {
            .dstSet = _cameraDescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo = &bufferInfo };

    Renderer::Instance->Device.updateDescriptorSets(descriptorWrite, nullptr);

    // Run an initial update to get the correct values
    update();
}

Camera::~Camera() {
    vmaDestroyBuffer(Renderer::Instance->Allocator, _cameraUboBuffer, _cameraUboAllocation);
}

glm::mat4 Camera::getProjectionMatrix() {
    return _projectionMatrix;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(_position, _position + _front, _up);
}

glm::vec3 Camera::getPosition() {
    return _position;
}

void Camera::update() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);

    // Also re-calculate the Right and Up vector
    _right = glm::normalize(glm::cross(_front, _worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up = glm::normalize(glm::cross(_right, _front));

    // Write updates for the camera
    SceneUBO ubo {};
    ubo.view = getViewMatrix();
    ubo.proj = getProjectionMatrix();
    ubo.proj[1][1] *= -1; // Adjust for Vulkan coords, vs OpenGL coords

    // Copy to the correct memory location
    void* mappedData;
    vmaMapMemory(Renderer::Instance->Allocator, _cameraUboAllocation, &mappedData);
    memcpy(mappedData, &ubo, sizeof(ubo));
    vmaUnmapMemory(Renderer::Instance->Allocator, _cameraUboAllocation);
}

void Camera::setProjectionMatrix(glm::mat4 projMatrix) {
    _projectionMatrix = projMatrix;

    // The projection matrix changed, run updates to ensure the camera is still positioned correctly
    update();
}

void Camera::processKeyboardInput(GLFWwindow *window, float deltaTime) {
    float speed = _speed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed *= 3.0f;

    float velocity = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        _position += _front * velocity;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        _position -= _front * velocity;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        _position -= _right * velocity;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        _position += _right * velocity;
}

void Camera::processMouseInput(float xPos, float yPos, bool constrainPitch) {
    if (_firstMouse) {
        _lastMouseX = xPos;
        _lastMouseY = yPos;
        _firstMouse = false;
    }

    float xOffset = xPos - _lastMouseX;
    float yOffset = _lastMouseY - yPos; // reversed since y-coordinates go from bottom to top

    _lastMouseX = xPos;
    _lastMouseY = yPos;

    xOffset *= _mouseSensitivity;
    yOffset *= _mouseSensitivity;

    _yaw += xOffset;
    _pitch += yOffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (_pitch > 89.0f)
            _pitch = 89.0f;
        if (_pitch < -89.0f)
            _pitch = -89.0f;
    }
}

void Camera::bind(vk::CommandBuffer &commandBuffer) {
    auto* pipeline = PipelineManager::getPipeline("basic");
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->getPipelineLayout(), 0, 1, &_cameraDescriptorSet, 0, nullptr);
}
