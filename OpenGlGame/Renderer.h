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
			Pointer<ShaderProgram> Shader;
		};

		static Scope<SceneData> s_SceneData;

	public:
		void BeginScene() {}
		void BeginScene(const Camera &camera);
		void EndScene();

		void SetShader(const Pointer<ShaderProgram> &shader);

		void OnWindowResize(uint32_t width, uint32_t height);

		void Draw(const Pointer<VertexArray> &array, Primitive primitive = Primitive::Triangles, int64_t count = -1);
		void Draw(const Pointer<VertexArray> &array, const std::vector<uint32_t> &indices, Primitive primitive = Primitive::Triangles, int64_t count = -1);

		void Draw(const Pointer<VertexArray> &vertexArray, const glm::mat4 &transform = glm::mat4(1.f));
		void Draw(const Pointer<VertexArray> &vertexArray, const std::vector<uint32_t> &indices, const glm::mat4 &transform = glm::mat4(1.f));

		void Draw(const Pointer<Model> &model, const glm::mat4 &transform = glm::mat4(1.f));
		void Draw(const Pointer<Mesh> &mesh, const glm::mat4 &transform = glm::mat4(1.f));

		void SetLight(const Light& light);
		void BindLight(const LightInfo& light);
		void BindLights();
		
		void BindMaterial(const Pointer<Material> &material);

	private:
		static void BindLight(Pointer<ShaderProgram> &shader, const LightInfo& light);
		
		static void BindMaterialTexture(
			Pointer<ShaderProgram> shader,
			const std::vector<Pointer<Texture>> &textures,
			const std::string &name,
			int32_t &textureUnit
			);
	};
}
