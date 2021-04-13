#pragma once

#include "Types.h"
#include "GLEnums.h"

#include <array>

#include "Transformable.h"
#include "Light.h"

namespace Game
{
	class ShaderProgram;
	class VertexArray;
	class UniformBuffer;
	
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
			std::array<LightInfo, MAX_LIGHTS> Lights;
			
			glm::mat4 ViewProjectionMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;

			Pointer<UniformBuffer> LightsBuffer;
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

		static void SetLight(const Light& light);
		static void BindLight(const LightInfo& light);
		static void BindLights();
		
		static void BindMaterial(const Pointer<Material> &material);

	private:
		static void Init();
		
		static void BindLight(Pointer<UniformBuffer> buffer, const LightInfo& light);
		
		static void BindMaterialTexture(
			Pointer<ShaderProgram> shader,
			const std::vector<Pointer<Texture>> &textures,
			const std::string &name,
			int32_t &textureUnit
			);
	};
}
