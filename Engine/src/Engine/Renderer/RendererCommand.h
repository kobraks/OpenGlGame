#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"

namespace Engine {
	class RendererCommand {
	public:
		static void Init();

		static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetClearColor(const Color &color);

		static void Clear();
	};
}