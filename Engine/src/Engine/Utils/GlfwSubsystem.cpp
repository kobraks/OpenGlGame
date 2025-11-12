#include "pch.h"
#include "GlfwSubsystem.h"

#include <GLFW/glfw3.h>

namespace Engine {
	std::atomic_uint32_t GLFWSubsystem::s_RefCount;
	std::once_flag GLFWSubsystem::s_InitializeFlag;


	void GLFWSubsystem::Acquire() {
		std::call_once(s_InitializeFlag, []() {
			glfwSetErrorCallback([](int code, const char* msg) {
				LOG_ENGINE_ERROR("GLFW Error ({}): \"{}\"", code, msg);
				});

			if (glfwInit() != GLFW_TRUE) {
				throw std::runtime_error("Failed to initialize GLFW");
			}
		});

		s_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void GLFWSubsystem::Release() {
		const auto prev = s_RefCount.fetch_sub(1, std::memory_order_acq_rel);

		ENGINE_ASSERT(prev > 0, "Mismatched GLFWSubsystem::Release() calls!");

		if (prev == 1) {
			glfwTerminate();
		}
	}
}
