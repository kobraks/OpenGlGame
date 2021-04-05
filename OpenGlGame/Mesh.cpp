#include "pch.h"
#include "Mesh.h"

#include "Buffer.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Game
{
	static glm::vec2 Convert(const aiVector2D &vector)
	{
		return glm::vec2(vector.x, vector.y);
	}

	static glm::vec3 Convert(const aiVector3D &vector)
	{
		return glm::vec3(vector.x, vector.y, vector.z);
	}

	static glm::vec4 Convert(const aiColor4D &vector)
	{
		return glm::vec4(vector.r, vector.g, vector.b, vector.a);
	}

	Mesh::Mesh(
		const std::string &name,
		const std::vector<Vertex> &vertices,
		const std::vector<uint32_t> &indices,
		Pointer<Material> material,
		const glm::mat4 &transform
		) : m_Material(material),
		    m_Name(name)
	{
		Create(vertices, indices, transform);
	}

	Mesh::Mesh(
		const std::vector<Vertex> &vertices,
		const std::vector<uint32_t> &indices,
		Pointer<Material> material,
		const glm::mat4 &transform
		) : m_Material(material)
	{
		Create(vertices, indices, transform);
	}

	Mesh::Mesh(const void *aiMesh)
	{
		const auto &mesh = static_cast<const ::aiMesh*>(aiMesh);
		m_Name           = mesh->mName.C_Str();

		LOG_INFO("Name of mesh: {}", m_Name);
		LOG_INFO("Vetex count: {}", mesh->mNumVertices);
		LOG_INFO("Faces count: {}", mesh->mNumFaces);

		auto &vertices = m_Vertices;
		auto &indices  = m_Indices;

		vertices.reserve(mesh->mNumVertices);

		if(!mesh->HasNormals())
			LOG_WARN("Mesh has no normal vectors");

		for(size_t i = 0; i < mesh->mNumVertices; ++i)
		{
			auto position = Convert(mesh->mVertices[i]);
			glm::vec4 color(1.f);
			glm::vec2 texCoords(0.f);
			glm::vec3 normal(0.f);

			if(mesh->HasVertexColors(0))
				color = Convert(mesh->mColors[0][i]);

			if(mesh->HasTextureCoords(0))
				texCoords = Convert(mesh->mTextureCoords[0][1]);

			if(mesh->HasNormals())
				normal = Convert(mesh->mNormals[i]);

			vertices.push_back(Vertex(position, texCoords, color, normal));
		}

		indices.reserve(mesh->mNumFaces * 3u);
		if(mesh->HasFaces())
		{
			for(size_t i = 0; i < mesh->mNumFaces; ++i)
			{
				auto &face = mesh->mFaces[i];

				for(size_t j = 0; j < face.mNumIndices; ++j)
					indices.push_back(face.mIndices[j]);
			}
		}

		CreateVertexArray(vertices, indices);
	}

	void Mesh::Create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const glm::mat4 &transform)
	{
		m_Transform = transform;

		m_Vertices = vertices;
		m_Indices  = indices;

		CreateVertexArray(vertices, indices);
	}

	void Mesh::CreateVertexArray(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
	{
		m_VertexArray = MakePointer<VertexArray>();

		auto vertexBuffer = MakePointer<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
		vertexBuffer->SetLayout(Vertex::GetLayout());

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(MakePointer<IndexBuffer>(indices.data(), indices.size()));

		glFlush();
	}
}
