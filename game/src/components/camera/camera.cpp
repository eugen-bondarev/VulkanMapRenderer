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

CameraEvents_& Camera::GetEvents()
{
	return events;
}

const glm::mat4x4& Camera::GetProjectionViewMatrix() const
{
	return projectionViewMatrix;
}