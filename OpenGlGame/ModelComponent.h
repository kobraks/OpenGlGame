#pragma once

#include <glm/mat4x4.hpp>

#include "Component.h"

namespace Game
{
	class Model;
	class Renderer;
	
	class ModelComponent : public Component
	{
		Pointer<Model> m_Model = nullptr;
		
	public:
		ModelComponent(Entity* entity = nullptr);

		Pointer<Model> GetModel() const { return m_Model; }

		void LoadModel(const std::string &fileName);
		
		void Draw(const glm::mat4 &transform = glm::mat4(1.f)) const;
	};
}