#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/RendererCommand.h"

#include "glm/mat4x4.hpp"

namespace Engine {
	class Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void Submit();
	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope<SceneData> s_SceneData;
	};
}
