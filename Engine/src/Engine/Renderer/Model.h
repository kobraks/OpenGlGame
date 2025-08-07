#pragma once
#include "Engine/Core/Base.h"

#include <vector>
#include <filesystem>

namespace Engine {
	class Mesh;

	class Model {
	public:
		using MeshContainerType = std::vector<Ref<Mesh>>;

		static Ref<Model> Load(std::filesystem::path path);

		const std::vector<Ref<Mesh>>& GetMeshes() const { return m_Meshes; }
		Ref<Mesh> GetMesh(uint32_t index) const {
			GAME_ASSERT(index < m_Meshes.size(), "Index out of bounds");
			if (index >= m_Meshes.size())
				throw std::out_of_range("Index out of bounds");

			return m_Meshes[index];
		}

		void RemoveMesh(uint32_t index);
		void AddMesh(Ref<Mesh> mesh);

		bool HasMeshes() const { return !m_Meshes.empty(); }
		bool HasMesh(uint32_t index) const {
			return index < m_Meshes.size() && m_Meshes[index];
		}

		MeshContainerType::iterator begin() { return m_Meshes.begin(); }
		MeshContainerType::iterator end() { return m_Meshes.end(); }

		MeshContainerType::const_iterator begin() const { return m_Meshes.begin(); }
		MeshContainerType::const_iterator end() const { return m_Meshes.end(); }

	private:
		MeshContainerType m_Meshes;
	};

}