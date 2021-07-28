#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <engine/engine.h>

Camera::Camera()
{
	CalculateTransformMatrix();
}

void Camera::CalculateTransformMatrix()
{
	transformMatrix = glm::inverse(glm::translate(glm::mat4x4(1), glm::vec3(position, 1.0f)));
	CalculateProjectionViewMatrix();
}

void Camera::CalculateProjectionViewMatrix()
{
	glm::vec2 half_size = Engine::window->GetSize() / 2.0f;
	glm::mat4x4 projection_matrix = glm::ortho(-half_size.x, half_size.x, -half_size.y, half_size.y);
	glm::mat4x4& view_matrix = transformMatrix;
	projectionViewMatrix = projection_matrix * view_matrix;
}

void Camera::CheckPositionChange()
{
	VT_PROFILER_SCOPE();

	events = CameraEvents_None;

	if (position != lastPosition)
	{
		events |= CameraEvents_PositionChanged;
		lastPosition = position;
	}
}

void Camera::Update()
{
	events = CameraEvents_None;

	if (position != lastPosition)
	{
		events |= CameraEvents_PositionChanged;
		lastPosition = position;
	}
	
	static float speed = 1500.0f;

	if (glfwGetKey(Engine::window->GetGLFWWindow(), GLFW_KEY_W))
	{
		AddPosition(glm::vec2(0, -1) * Engine::Time::GetDelta() * speed);
	}
	if (glfwGetKey(Engine::window->GetGLFWWindow(), GLFW_KEY_S))
	{
		AddPosition(glm::vec2(0, 1) * Engine::Time::GetDelta() * speed);
	}
	// if (glfwGetKey(Engine::window->GetGLFWWindow(), GLFW_KEY_D))
	{
		AddPosition(glm::vec2(1, 0) * Engine::Time::GetDelta() * speed);
	}
	if (glfwGetKey(Engine::window->GetGLFWWindow(), GLFW_KEY_A))
	{
		AddPosition(glm::vec2(-1, 0) * Engine::Time::GetDelta() * speed);
	}
}

CameraEvents_& Camera::GetEvents()
{
	return events;
}

const glm::mat4x4& Camera::GetProjectionViewMatrix() const
{
	return projectionViewMatrix;
}