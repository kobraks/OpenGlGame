#include "pch.h"
#include "ModelComponent.h"


#include "Defines.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Renderer.h"

namespace Game
{
	ModelComponent::ModelComponent(Entity *entity) : Component("ModelComponent", entity) {}

	void ModelComponent::LoadModel(const std::string &fileName)
	{
		m_Model = ModelLoader::Load(fileName, MODELS_PATH);
	}
	
	void ModelComponent::Draw(const glm::mat4 &transform ) const
	{
		Renderer::Draw(m_Model, transform);
	}
}
