#include "pch.h"
#include "Model.h"

namespace Engine {
	void Model::RemoveMesh(uint32_t index) {
		GAME_ASSERT(index < m_Meshes.size(), "Index out of bounds");
		if (index >= m_Meshes.size())
			throw std::out_of_range("Index out of bounds");

		m_Meshes.erase(m_Meshes.begin() + index);
	}

	void Model::AddMesh(Ref<Mesh> mesh) {
		if (!mesh) {
			LOG_ENGINE_ERROR("Cannot add a null mesh to the model");
			return;
		}
		m_Meshes.push_back(mesh);
		LOG_ENGINE_INFO("Mesh added to model, total meshes: {0}", m_Meshes.size());
	}
}
