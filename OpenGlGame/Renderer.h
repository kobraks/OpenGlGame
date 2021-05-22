#pragma once

#include <glm/mat4x4.hpp>

#include "Types.h"
#include "GLEnums.h"

#include "Transformable.h"
#include "Light.h"

namespace Game
{
	class ShaderProgram;
	class VertexArray;
	
	class Model;
	class Camera;
	struct Material;
	class Texture;
	class Mesh;

	class Renderer
	{
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;

			Pointer<ShaderProgram> Shader;
		};

		static Scope<SceneData> s_SceneData;

	public:
		static void BeginScene();
		static void BeginScene(const Camera &camera, const glm::mat4& transform);
		static void EndScene();

		static void SetShader(const Pointer<ShaderProgram> &shader);

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void Draw(const Pointer<VertexArray> &array, Primitive primitive = Primitive::Triangles, int64_t count = -1);
		static void Draw(const Pointer<VertexArray> &array, const std::vector<uint32_t> &indices, Primitive primitive = Primitive::Triangles, int64_t count = -1);

		static void Draw(const Pointer<VertexArray> &vertexArray, const glm::mat4 &transform = glm::mat4(1.f));
		static void Draw(const Pointer<VertexArray> &vertexArray, const std::vector<uint32_t> &indices, const glm::mat4 &transform = glm::mat4(1.f));

		static void Draw(const Pointer<Model> &model, const glm::mat4 &transform = glm::mat4(1.f));
		static void Draw(const Pointer<Mesh> &mesh, const glm::mat4 &transform = glm::mat4(1.f));

		static void BindMaterial(const Pointer<Material> &material);

	private:
		static void Init();
		
		static void BindMaterialTexture(
			Pointer<ShaderProgram> shader,
			const std::vector<Pointer<Texture>> &textures,
			const std::string &name,
			int32_t &textureUnit
			);
	};
}
