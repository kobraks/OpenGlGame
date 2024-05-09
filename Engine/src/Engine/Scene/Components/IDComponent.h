#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"

namespace Engine {
	struct IDComponent {
		UUID ID;

		IDComponent() = default;
		IDComponent(IDComponent&) = default;

		IDComponent(UUID& id) : ID(id) {}
	};
}