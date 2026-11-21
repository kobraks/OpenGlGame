#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"

namespace Engine {
	struct IDComponent {
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;

		explicit IDComponent(UUID& id) : ID(id) {}
	};
}