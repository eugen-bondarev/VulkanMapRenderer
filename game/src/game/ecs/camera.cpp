#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::CalculateTransformMatrix()
{
	transformMatrix = glm::inverse(glm::translate(glm::mat4x4(1), glm::vec3(position, 1.0f)));
}

void Camera::CheckPositionChange()
{
	MW_PROFILER_SCOPE();

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