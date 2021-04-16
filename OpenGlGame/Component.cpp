#include "pch.h"
#include "Component.h"

#include "Defines.h"
#include "ModelLoader.h"
#include "Model.h"
#include "Renderer.h"

namespace Game
{
	void ModelComponent::LoadModel(const std::string &fileName)
	{
		Model = ModelLoader::Load(fileName, MODELS_PATH);
	}
	
	void ModelComponent::Draw(const glm::mat4 &transform) const
	{
		Renderer::Draw(Model, transform);
	}
}
