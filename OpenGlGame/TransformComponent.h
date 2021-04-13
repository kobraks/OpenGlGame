#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "Component.h"

#include "Transformable.h"

namespace Game
{
	class TransformComponent: public Component, public Transformable
	{
	public:
		explicit TransformComponent(Entity* entity = nullptr);
	};
}
