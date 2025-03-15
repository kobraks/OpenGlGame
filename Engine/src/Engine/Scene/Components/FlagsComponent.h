#include "Engine/Core/Base.h"

namespace Engine {
	struct FlagsComponent {
		uint32_t Flags;

		FlagsComponent() = default;
		~FlagsComponent() = default;

		FlagsComponent(uint32_t flags) : Flags(flags) {}
	}; 
}