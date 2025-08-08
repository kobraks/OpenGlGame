#include "SceneContextView.h"
#include "SceneContext.h"
#include "SceneStateController.h"

Editor::SceneContextView::SceneContextView(SceneContext* context, SceneStateController* controller) : m_Context(context), m_Controller(controller) {
}

Engine::Ref<Engine::Scene> Editor::SceneContextView::GetEditorScene() const {
	return m_Context ? m_Context->GetEditorScene() : nullptr;
}

Engine::Ref<Engine::Scene> Editor::SceneContextView::GetActiveScene() const {
	return m_Context ? m_Context->GetActiveScene() : nullptr;
}

Editor::SceneState Editor::SceneContextView::GetState() const{
	return m_Controller ? m_Controller->GetState() : SceneState::Edit;
}

bool Editor::SceneContextView::IsPaused() const {
	return m_Context && m_Context->IsPaused();
}
