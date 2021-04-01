#pragma once

#include "Types.h"
#include "GLEnums.h"

namespace Game
{
	class ShaderProgram;
	class VertexArray;
	class Model;
	class Camera;
	
	class Renderer
	{
	public:
		void BeginScane(const Camera& camera);
		void EndScene();

		void SetShader(const Pointer<ShaderProgram>& shader);
		
		void OnWindowResize(uint32_t width, uint32_t height);

		void Draw(const Pointer<VertexArray>& array, Primitive primitive = Primitive::Triangles, const int64_t count = -1);
		
		void Draw(const Pointer<VertexArray>& vertexArray, const glm::mat4& transform);
		void Draw(const Pointer<Model>& model, const glm::mat4& transform) const;

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			Pointer<ShaderProgram> Shader;
		};

		static Scope<SceneData> s_SceneData;
	};
}
