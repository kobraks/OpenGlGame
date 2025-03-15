#include "pch.h"
#include "Engine/Renderer/Renderer.h"

namespace Engine {
	Scope<Renderer::SceneData> Renderer::s_SceneData = MakeScope<Renderer::SceneData>();

	void Renderer::Init() {
		RendererCommand::Init();
	}

	void Renderer::Shutdown() {

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RendererCommand::SetViewPort(0, 0, width, height);
	}

	void Renderer::BeginScene(const Camera& camera) {
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene() {
	}

	void Renderer::Submit() {

	}
}
