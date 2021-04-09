#include "pch.h"
#include "TransformComponent.h"


namespace Game
{
	TransformComponent::TransformComponent() : Component("Transform Component") {}

	TransformComponent::TransformComponent(Entity *entity) : Component("Transform Component", entity) {}

	Pointer<Component> TransformComponent::Clone() const
	{
		return MakePointer<TransformComponent>(*this);
	}
}
