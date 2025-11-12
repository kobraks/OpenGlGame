#pragma once
#include "Engine/Core/Base.h"

#include <atomic>

namespace Engine {
	class GLFWSubsystem {
	public:
		static void Acquire();
		static void Release();

	private:
		static std::once_flag s_InitializeFlag;
		static std::atomic_uint32_t s_RefCount;
	};
}
