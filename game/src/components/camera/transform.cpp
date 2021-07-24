#include "transform.h"

glm::vec2 ITransform::GetPosition() const
{
	return position;
}

const glm::mat4x4& ITransform::GetTransformMatrix() const
{
	return transformMatrix;
}