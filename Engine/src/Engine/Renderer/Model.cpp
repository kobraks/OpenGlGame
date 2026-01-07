#include "pch.h"
#include "Model.h"

#include "Engine/Renderer/Mesh.h"

#include "Assimp/Importer.hpp"
#include "Assimp/scene.h"
#include "Assimp/postprocess.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace Engine {
	namespace {
		static glm::mat4 ToGlmMat4(const aiMatrix4x4& m) {
			glm::mat4 r;
			r[0][0] = m.a1; r[1][0] = m.a2; r[2][0] = m.a3; r[3][0] = m.a4;
			r[0][0] = m.b1; r[1][0] = m.b2; r[2][0] = m.b3; r[3][0] = m.b4;
			r[0][0] = m.c1; r[1][0] = m.c2; r[2][0] = m.c3; r[3][0] = m.c4;
			r[0][0] = m.d1; r[1][0] = m.d2; r[2][0] = m.d3; r[3][0] = m.d4;

			return r;
		}

		std::vector<uint32_t> ProcessIndices(const aiMesh* mesh) {
			std::vector<uint32_t> indices;

			indices.reserve(mesh->mNumFaces * 3);
			for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
				const aiFace& face = mesh->mFaces[i];

				if (face.mNumIndices != 3) {
					LOG_ENGINE_WARN("Non-triangular face detected in mesh '{}', skipping face.", mesh->mName.C_Str());
					continue;
				}

				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			return indices;
		}

		std::vector<Vertex> ProcessVertices(const aiMesh* mesh, const glm::mat4& worldTransform, const glm::mat3& normalMatrix) {
			std::vector<Vertex> vertices;
			vertices.reserve(mesh->mNumVertices);

			for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
				Vertex vertex{};

				//Position
				{
					const aiVector3D p = mesh->mVertices[i];
					const glm::vec4 wp = worldTransform * glm::vec4(p.x, p.y, p.z, 1.f);
					vertex.Position = glm::vec3(wp);
				}

				//Normal (optional)
				if (mesh->HasNormals()) {
					const aiVector3D normal = mesh->mNormals[i];
					vertex.Normal = glm::normalize(normalMatrix * glm::vec3(normal.x, normal.y, normal.z));
				} else {
					vertex.Normal = glm::vec3(0.f, 1.f, 0.f);
				}

				//TexCoords (optional)
				//TODO

				// Tangent/Bitangent (optional)
				//TODO

				vertices.push_back(vertex);
			}

			return vertices;
		}

		static Ref<Mesh> ProcessMesh(const aiMesh* mesh, const glm::mat4& worldTransform) {
			const glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(worldTransform));

			return MakeRef<Mesh>(ProcessVertices(mesh, worldTransform, normalMatrix), ProcessIndices(mesh));
		}

		static void ProcessNode(const aiNode* node, const aiScene* scene, Ref<Model> model, const glm::mat4& parentTransform) {
			const glm::mat4 localTransform = ToGlmMat4(node->mTransformation);
			const glm::mat4 worldTransform = parentTransform * localTransform;

			for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
				const uint32_t meshIndex = node->mMeshes[i];
				const aiMesh* mesh = scene->mMeshes[meshIndex];

				Ref<Mesh> engineMesh = ProcessMesh(mesh, worldTransform);
				if (engineMesh)
					model->AddMesh(engineMesh);
			}

			for (uint32_t i = 0; i < node->mNumChildren; ++i) {
				ProcessNode(node->mChildren[i], scene, model, worldTransform);
			}
		}
	}

	Ref<Model> Model::Load(std::filesystem::path path) {
		if (!std::filesystem::exists(path)) {
			LOG_ENGINE_ERROR("Model file does not exist: {}", path.string());
			return nullptr;
		}

		Assimp::Importer importer;

		const uint32_t flags = aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_ImproveCacheLocality |
			aiProcess_SortByPType |
			aiProcess_OptimizeMeshes |
			aiProcess_OptimizeGraph |
			aiProcess_ValidateDataStructure |
			aiProcess_FlipUVs;

		const aiScene* scene = importer.ReadFile(path.string(), flags);

		if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)) {
			LOG_ENGINE_ERROR("Assimp failed to load '{}': {}", path.string(), importer.GetErrorString());
			return nullptr;
		}

		Ref<Model> model = MakeRef<Model>();
		ProcessNode(scene->mRootNode, scene, model, glm::mat4(1.0f));

		LOG_ENGINE_INFO("Loaded model '{}', meshes = {}", path.string(), model->GetMeshes().size());
		return model;
	}

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
