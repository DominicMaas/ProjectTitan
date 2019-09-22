#include "Camera.h"

Camera::Camera(glm::vec3 position)
{
	_position = position;
	processUpdates();
}

glm::mat4 Camera::getProjectionMatrix()
{
	return _projectionMatrix;
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(_position, _position + _front, _up);
}

glm::vec3 Camera::getPosition()
{
	return _position;
}

void Camera::processUpdates()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_front = glm::normalize(front);

	// Also re-calculate the Right and Up vector
	_right = glm::normalize(glm::cross(_front, _worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	_up = glm::normalize(glm::cross(_right, _front));
}

void Camera::processKeyboardInput(GLFWwindow* window, float deltaTime)
{
	float velocity = _speed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		_position += _front * velocity;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		_position -= _front * velocity;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		_position -= _right * velocity;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		_position += _right * velocity;
}

void Camera::processMouseInput(float xPos, float yPos, GLboolean constrainPitch)
{
	if (_firstMouse)
	{
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
	if (constrainPitch)
	{
		if (_pitch > 89.0f)
			_pitch = 89.0f;
		if (_pitch < -89.0f)
			_pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	processUpdates();
}