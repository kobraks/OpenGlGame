#pragma once
#include "Component.h"

namespace Game
{
	class TagComponent : public Component
	{
	public:
		TagComponent(Entity* entity = nullptr) : Component("TagComponent", entity)
		{}

		std::string Tag;
	};
}